/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "DebInstaller.h"

#include <QtCore/QFileInfo>
#include <QtCore/QStringBuilder>
#include <QtGui/QStackedWidget>

#include <KApplication>
#include <KIcon>
#include <KLocale>
#include <KPushButton>
#include <KMessageBox>
#include <KDebug>

#include <apt-pkg/debversion.h>
#include <apt-pkg/pkgsystem.h>
#include <apt-pkg/version.h>

#include "../../src/backend.h"
#include "../../src/config.h"
#include "../../src/dependencyinfo.h"
#include "../../src/transaction.h"

#include "DebCommitWidget.h"
#include "DebViewer.h"

DebInstaller::DebInstaller(QWidget *parent, const QString &debFile)
    : KDialog(parent)
    , m_backend(new QApt::Backend(this))
    , m_trans(nullptr)
    , m_commitWidget(nullptr)
{
    if (!m_backend->init())
        initError();

    QFileInfo fi(debFile);
    m_debFile = new QApt::DebFile(fi.absoluteFilePath());

    initGUI();
}

void DebInstaller::initError()
{
    QString details = m_backend->initErrorMessage();

    QString text = i18nc("@label",
                         "The package system could not be initialized, your "
                         "configuration may be broken.");
    QString title = i18nc("@title:window", "Initialization error");

    KMessageBox::detailedError(this, text, details, title);
    exit(-1);
}

void DebInstaller::initGUI()
{
    setButtons(KDialog::Cancel | KDialog::Apply);
    setButtonText(KDialog::Apply, i18nc("@label", "Install Package"));
    m_applyButton = button(KDialog::Apply);
    m_cancelButton = button(KDialog::Cancel);

    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(installDebFile()));

    m_stack = new QStackedWidget(this);
    setMainWidget(m_stack);

    m_commitWidget = new DebCommitWidget(this);
    m_stack->addWidget(m_commitWidget);

    m_debViewer = new DebViewer(m_stack);
    m_debViewer->setBackend(m_backend);
    m_stack->addWidget(m_debViewer);

    if (!m_debFile->isValid()) {
        QString text = i18nc("@label",
                             "Could not open <filename>%1</filename>. It does not appear to be a "
                             "valid Debian package file.", m_debFile->filePath());
        KMessageBox::error(this, text, QString());
        KApplication::instance()->quit();
        return;
    }

    setWindowTitle(i18nc("@title:window",
                         "Package Installer - %1", m_debFile->packageName()));
    m_debViewer->setDebFile(m_debFile);
    bool canInstall = checkDeb();

    m_applyButton->setEnabled(canInstall);
    m_debViewer->setStatusText(m_statusString);

    if (!m_versionInfo.isEmpty()){
        m_debViewer->setVersionTitle(m_versionTitle);
        m_debViewer->setVersionInfo(m_versionInfo);
    } else {
        m_debViewer->hideVersionInfo();
    }
}

void DebInstaller::transactionStatusChanged(QApt::TransactionStatus status)
{
    switch (status) {
    case QApt::DownloadingStatus:
    case QApt::CommitChangesRole:
        m_stack->setCurrentWidget(m_commitWidget);
        break;
    case QApt::FinishedStatus:
        if (m_trans->role() == QApt::CommitChangesRole) {
            // Dependencies installed, now go for the deb file
            m_trans = m_backend->installFile(*m_debFile);
            setupTransaction(m_trans);
            m_trans->run();
        } else
            setButtons(KDialog::Close);
    default:
        break;
    }
}

//void DebInstaller::workerEvent(QApt::WorkerEvent event)
//{
//    switch (event) {
//    case QApt::PackageDownloadStarted:
//        if (m_commitWidget) {
//            m_stack->setCurrentWidget(m_commitWidget);
//            m_commitWidget->showProgress();
//            m_commitWidget->setHeaderText(i18nc("@info Header label used when packages are downloading",
//                                                "<title>Downloading Dependencies</title>"));

//            connect(m_backend, SIGNAL(downloadProgress(int,int,int)),
//                m_commitWidget, SLOT(updateDownloadProgress(int,int,int)));
//        }
//        break;
//    case QApt::PackageDownloadFinished:
//        if (m_commitWidget) {
//            disconnect(m_backend, SIGNAL(downloadProgress(int,int,int)),
//                       m_commitWidget, SLOT(updateDownloadProgress(int,int,int)));
//        }
//        break;
//    case QApt::CommitChangesStarted:
//        if (m_commitWidget) {
//            m_commitWidget->setHeaderText(i18nc("@info Header label used when packages are installing",
//                                                "<title>Installing Dependencies</title>"));

//            connect(m_backend, SIGNAL(commitProgress(QString,int)),
//                    m_commitWidget, SLOT(updateCommitProgress(QString,int)));
//        }
//        break;
//    case QApt::CommitChangesFinished:
//        if (m_commitWidget) {
//            m_commitWidget->hideProgress();

//            disconnect(m_backend, SIGNAL(commitProgress(QString,int)),
//                       m_commitWidget, SLOT(updateCommitProgress(QString,int)));

//            m_backend->installFile(*m_debFile);
//        }
//        break;
//    case QApt::DebInstallStarted:
//        if (m_commitWidget) {
//            m_commitWidget->hideProgress();
//            m_stack->setCurrentWidget(m_commitWidget);
//        }
//        break;
//    case QApt::DebInstallFinished:
//        if (m_commitWidget) {
//            m_commitWidget->updateTerminal(i18nc("@label Message that the install is done",
//                                                "Done"));
//            m_commitWidget->setHeaderText(i18nc("@info Header label used when the install is done",
//                                                "<title>Done</title>"));
//        }
//        setButtons(KDialog::Close);
//        break;
//    case QApt::InvalidEvent:
//    default:
//        break;
//    }
//}

void DebInstaller::errorOccurred(QApt::ErrorCode error)
{
    switch (error) {
    case QApt::AuthError:
    case QApt::LockError:
        m_applyButton->setEnabled(true);
        m_cancelButton->setEnabled(true);
        break;
    default:
        break;
    }
}

void DebInstaller::installDebFile()
{
    m_applyButton->setEnabled(false);
    m_cancelButton->setEnabled(false);

    if (m_backend->markedPackages().size()) {
        m_trans = m_backend->commitChanges();
    } else {
        m_trans = m_backend->installFile(*m_debFile);
    }

    setupTransaction(m_trans);
    m_trans->run();
}

void DebInstaller::setupTransaction(QApt::Transaction *trans)
{

}

bool DebInstaller::checkDeb()
{
    QStringList arches = m_backend->architectures();
    arches.append(QLatin1String("all"));
    QString debArch = m_debFile->architecture();

    // Check if we support the arch at all
    if (debArch != m_backend->nativeArchitecture()) {
        if (!arches.contains(debArch)) {
            // Wrong arch
            m_statusString = i18nc("@info", "Error: Wrong architecture '%1'", debArch);
            m_statusString.prepend(QLatin1String("<font color=\"#ff0000\">"));
            m_statusString.append(QLatin1String("</font>"));
            return false;
        }

        // We support this foreign arch
        m_foreignArch = debArch;
    }

    compareDebWithCache();

    QApt::PackageList conflicts = checkConflicts();
    if (!conflicts.isEmpty()) {
        return false;
    }

    QApt::Package *willBreak = checkBreaksSystem();
    if (willBreak) {
        m_statusString = i18nc("@info Error string when installing would break an existing package",
                               "Error: Breaks the existing package \"%1\"",
                               willBreak->name());
        m_statusString.prepend(QLatin1String("<font color=\"#ff0000\">"));
        m_statusString.append(QLatin1String("</font>"));
        return false;
    }

    if (!satisfyDepends()) {
        // create status message
        m_statusString = i18nc("@info", "Error: Cannot satisfy dependencies");
        m_statusString.prepend(QLatin1String("<font color=\"#ff0000\">"));
        m_statusString.append(QLatin1String("</font>"));
        return false;
    }

    int toInstall = m_backend->markedPackages().size();
    m_debViewer->showDetailsButton(toInstall);
    if (toInstall) {
        m_statusString = i18ncp("@label A note saying that additional packages are needed",
                                "Requires the installation of %1 additional package.",
                                "Requires the installation of %1 additional packages",
                                toInstall);
    } else {
        m_statusString = i18nc("@info", "All dependencies are satisfied.");
    }

    return true;
}

void DebInstaller::compareDebWithCache()
{
    QApt::Package *pkg = m_backend->package(m_debFile->packageName());

    if (!pkg) {
        return;
    }

    QString version = m_debFile->version();

    int res = QApt::Package::compareVersion(m_debFile->version(), pkg->availableVersion());

    if (res == 0 && !pkg->isInstalled()) {
        m_versionTitle = i18nc("@info", "The same version is available in a software channel.");
        m_versionInfo = i18nc("@info", "It is recommended to install the software from the channel instead");
    } else if (res > 0) {
        m_versionTitle = i18nc("@info", "An older version is available in a software channel.");
        m_versionInfo = i18nc("@info", "It is recommended to install the version from the "
                                       "software channel, since it usually has more support.");
    } else if (res < 0) {
        m_versionTitle = i18nc("@info", "A newer version is available in a software channel.");
        m_versionInfo = i18nc("@info", "It is strongly advised to install the version from the "
                                       "software channel, since it usually has more support.");
    }
}

QString DebInstaller::maybeAppendArchSuffix(const QString &pkgName, bool checkingConflicts)
{
    // Trivial cases where we don't append
    if (m_foreignArch.isEmpty())
        return pkgName;

    QApt::Package *pkg = m_backend->package(pkgName);
    if (!pkg || pkg->architecture() == QLatin1String("all"))
        return pkgName;

    // Real multiarch checks
    QString multiArchName = pkgName % ':' % m_foreignArch;
    QApt::Package *multiArchPkg = m_backend->package(multiArchName);

    // Check for a new dependency, we'll handle that later
    if (!multiArchPkg)
        return multiArchName;

    // Check the multi arch state
    QApt::MultiArchType type = multiArchPkg->multiArchType();

    // Add the suffix, unless it's a pkg that can satify foreign deps
    if (type == QApt::MultiArchForeign)
        return pkgName;

    // If this is called as part of a conflicts check, any not-multiarch
    // enabled package is a conflict implicitly
    if (checkingConflicts && type == QApt::MultiArchSame)
        return pkgName;

    return multiArchName;
}

QApt::PackageList DebInstaller::checkConflicts()
{
    QApt::PackageList conflictingPackages;
    QList<QApt::DependencyItem> conflicts = m_debFile->conflicts();

    QApt::Package *pkg = 0;
    QString packageName;
    bool ok = true;
    foreach(const QApt::DependencyItem &item, conflicts) {
        foreach (const QApt::DependencyInfo &info, item) {
            packageName = maybeAppendArchSuffix(info.packageName(), true);
            pkg = m_backend->package(packageName);

            if (!pkg) {
                // FIXME: Virtual package, must check provides
                continue;
            }

            string pkgVer = pkg->version().toStdString();
            string depVer = info.packageVersion().toStdString();

            ok = _system->VS->CheckDep(pkgVer.c_str(),
                                       info.relationType(),
                                       depVer.c_str());

            if (ok) {
                // Group satisfied
                break;
            }
        }

        if (!ok && pkg) {
            conflictingPackages.append(pkg);
        }
    }

    return conflictingPackages;
}

QApt::Package *DebInstaller::checkBreaksSystem()
{
    QApt::PackageList systemPackages = m_backend->availablePackages();
    string debVer = m_debFile->version().toStdString();

    foreach (QApt::Package *pkg, systemPackages) {
        if (!pkg->isInstalled()) {
            continue;
        }

        // Check for broken depends
        foreach(const QApt::DependencyItem &item, pkg->depends()) {
            foreach (const QApt::DependencyInfo &dep, item) {
                if (dep.packageName() != m_debFile->packageName()) {
                    continue;
                }

                string depVer = dep.packageVersion().toStdString();

                if (!_system->VS->CheckDep(debVer.c_str(), dep.relationType(),
                                           depVer.c_str())) {
                    return pkg;
                }
            }
        }

        // Check for existing conflicts against the .deb
        // FIXME: Check provided virtual packages too
        foreach(const QApt::DependencyItem &item, pkg->conflicts()) {
            foreach (const QApt::DependencyInfo &conflict, item) {
                if (conflict.packageName() != m_debFile->packageName()) {
                    continue;
                }

                string conflictVer = conflict.packageVersion().toStdString();

                if (_system->VS->CheckDep(debVer.c_str(),
                                          conflict.relationType(),
                                          conflictVer.c_str())) {
                    return pkg;
                }
            }
        }
    }

    return 0;
}

bool DebInstaller::satisfyDepends()
{
    QApt::Package *pkg = 0;
    QString packageName;

    foreach(const QApt::DependencyItem &item, m_debFile->depends()) {
        bool oneSatisfied = false;
        foreach (const QApt::DependencyInfo &dep, item) {
            packageName = maybeAppendArchSuffix(dep.packageName());
            pkg = m_backend->package(packageName);
            if (!pkg) {
                // FIXME: virtual package handling
                continue;
            }

            string debVersion = dep.packageVersion().toStdString();

            // If we're installed, see if we already satisfy the dependency
            if (pkg->isInstalled()) {
                string pkgVersion = pkg->installedVersion().toStdString();

                if (_system->VS->CheckDep(pkgVersion.c_str(),
                                          dep.relationType(),
                                          debVersion.c_str())) {
                    oneSatisfied = true;
                    break;
                }
            }

            // else check if cand ver will satisfy, then mark
            string candVersion = pkg->availableVersion().toStdString();

            if (!_system->VS->CheckDep(candVersion.c_str(),
                                       dep.relationType(),
                                       debVersion.c_str())) {
                continue;
            }

            pkg->setInstall();

            if (!pkg->wouldBreak()) {
                oneSatisfied = true;
                break;
            }
        }

        if (!oneSatisfied) {
            return false;
        }
    }

    return true;
}
