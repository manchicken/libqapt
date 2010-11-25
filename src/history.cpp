/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "history.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QDebug>

// APT includes
#include <apt-pkg/configuration.h>

namespace QApt {

class HistoryItemPrivate
{
    public:
        HistoryItemPrivate(const QString &data) : isValid(true)
        {
            parseData(data);
        };

        // Data
        QDateTime startDate;
        Package::State action;
        QStringList packageList;
        QString error;
        bool isValid;

        void parseData(const QString &data);
};

void HistoryItemPrivate::parseData(const QString &data)
{
    QStringList lines = data.split(QLatin1Char('\n'));

    int lineIndex = 0;
    bool dateFound = false;
    bool actionFound = false;
    bool packageListFound = false;
    bool errorFound = false;

    QStringList actionStrings;
    actionStrings << "Install" << "Upgrade" << "Downgrade" << "Remove" << "Purge";

    while (lineIndex < lines.size()) {
        QString line = lines.at(lineIndex);
        // skip empty lines and lines beginning with '#'
        if (line.isEmpty() || line.at(0) == '#') {
            lineIndex++;
            continue;
        }

        QStringList keyValue = line.split(QLatin1String(": "));
        int actionIndex = actionStrings.indexOf(keyValue.value(0));

        // Invalid
        if (keyValue.size() < 2) {
            isValid = false;
            lineIndex++;
            continue;
        }

        if (!dateFound && (keyValue.value(0) == QLatin1String("Start-Date"))) {
            startDate = QDateTime::fromString(keyValue.value(1), QLatin1String("yyyy-MM-dd  hh:mm:ss"));
        } else if (!actionFound && (actionIndex > -1)) {
            switch(actionIndex) {
            case 0:
                action = Package::ToInstall;
                break;
            case 1:
                action = Package::ToUpgrade;
                break;
            case 2:
                action = Package::ToDowngrade;
                break;
            case 3:
                action = Package::ToRemove;
                break;
            case 4:
                action = Package::ToPurge;
                break;
            default:
                break;
            }

            QString packages = keyValue.value(1);
            // Remove arch info
            packages.remove(QRegExp(":\\w+"));

            foreach (QString package, packages.split("),")) {
                package.append(QLatin1Char(')'));
                packageList << package;
            }
        } else if (!actionFound && (keyValue.value(0) == QLatin1String("Error"))) {
            error = keyValue.value(1);
        }

        lineIndex++;
    }
}

HistoryItem::HistoryItem(const QString &data)
       : d_ptr(new HistoryItemPrivate(data))
{
}

HistoryItem::~HistoryItem()
{
    delete d_ptr;
}

QDateTime HistoryItem::startDate() const
{
    Q_D(const HistoryItem);

    return d->startDate;
}

Package::State HistoryItem::action() const
{
    Q_D(const HistoryItem);

    return d->action;
}

QStringList HistoryItem::packageList() const
{
    Q_D(const HistoryItem);

    return d->packageList;
}

QString HistoryItem::errorString() const
{
    Q_D(const HistoryItem);

    return d->error;
}

bool HistoryItem::isValid() const
{
    Q_D(const HistoryItem);

    return d->isValid;
}



class HistoryPrivate
{
    public:
        HistoryPrivate(const QString &fileName) : historyFilePath(fileName)
        {
            init();
        };

        ~HistoryPrivate()
        {
            qDeleteAll(historyItemList);
        };

        // Data
        QString historyFilePath;
        HistoryItemList historyItemList;

        void init();
};

void HistoryPrivate::init()
{
    qDeleteAll(historyItemList);
    historyItemList.clear();

    QString data;

    QFileInfo historyFile(historyFilePath);
    QString directoryPath = historyFile.absoluteDir().absolutePath();
    QDir logDirectory(directoryPath);
    QStringList logFiles = logDirectory.entryList(QDir::Files, QDir::Name);

    foreach (QString file, logFiles) {
        file.prepend(directoryPath + QLatin1Char('/'));
        if (file.contains(QLatin1String("history"))) {
            if (file.endsWith(QLatin1String(".gz"))) {
                QProcess gunzip;
                gunzip.start(QLatin1String("gunzip"), QStringList() << QLatin1String("-c") << file);
                gunzip.waitForFinished();

                data.append(gunzip.readAll());
            } else {
                QFile historyFile(file);

                if (historyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    data.append(historyFile.readAll());
                }
            }
        }
    }

    data = data.trimmed();
    QStringList stanzas = data.split(QLatin1String("\n\n"));
    foreach(const QString &stanza, stanzas) {
        HistoryItem *historyItem = new HistoryItem(stanza);
        if (historyItem->isValid()) {
            historyItemList << historyItem;
        }
    }
}

History::History(QObject *parent)
       : QObject(parent)
       , d_ptr(new HistoryPrivate(QLatin1String(_config->FindFile("Dir::Log::History").c_str())))
{
}

History::~History()
{
    delete d_ptr;
}

HistoryItemList History::historyItems() const
{
    Q_D(const History);

    return d->historyItemList;
}

void History::reload()
{
    Q_D(History);

    d->init();
}
 
}