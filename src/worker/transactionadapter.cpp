/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -i ../downloadprogress.h -c TransactionAdaptor -a transactionadapter.h:transactionadapter.cpp org.kubuntu.qaptworker2.transaction.xml
 *
 * qdbusxml2cpp is Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "transactionadapter.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class TransactionAdaptor
 */

TransactionAdaptor::TransactionAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

TransactionAdaptor::~TransactionAdaptor()
{
    // destructor
}

QString TransactionAdaptor::debconfPipe() const
{
    // get the value of property debconfPipe
    return qvariant_cast< QString >(parent()->property("debconfPipe"));
}

qulonglong TransactionAdaptor::downloadETA() const
{
    // get the value of property downloadETA
    return qvariant_cast< qulonglong >(parent()->property("downloadETA"));
}

QApt::DownloadProgress TransactionAdaptor::downloadProgress() const
{
    // get the value of property downloadProgress
    return qvariant_cast< QApt::DownloadProgress >(parent()->property("downloadProgress"));
}

qulonglong TransactionAdaptor::downloadSpeed() const
{
    // get the value of property downloadSpeed
    return qvariant_cast< qulonglong >(parent()->property("downloadSpeed"));
}

int TransactionAdaptor::error() const
{
    // get the value of property error
    return qvariant_cast< int >(parent()->property("error"));
}

QString TransactionAdaptor::errorDetails() const
{
    // get the value of property errorDetails
    return qvariant_cast< QString >(parent()->property("errorDetails"));
}

int TransactionAdaptor::exitStatus() const
{
    // get the value of property exitStatus
    return qvariant_cast< int >(parent()->property("exitStatus"));
}

QString TransactionAdaptor::filePath() const
{
    // get the value of property filePath
    return qvariant_cast< QString >(parent()->property("filePath"));
}

int TransactionAdaptor::frontendCaps() const
{
    // get the value of property frontendCaps
    return qvariant_cast< int >(parent()->property("frontendCaps"));
}

bool TransactionAdaptor::isCancellable() const
{
    // get the value of property isCancellable
    return qvariant_cast< bool >(parent()->property("isCancellable"));
}

bool TransactionAdaptor::isCancelled() const
{
    // get the value of property isCancelled
    return qvariant_cast< bool >(parent()->property("isCancelled"));
}

bool TransactionAdaptor::isPaused() const
{
    // get the value of property isPaused
    return qvariant_cast< bool >(parent()->property("isPaused"));
}

QString TransactionAdaptor::locale() const
{
    // get the value of property locale
    return qvariant_cast< QString >(parent()->property("locale"));
}

QVariantMap TransactionAdaptor::packages() const
{
    // get the value of property packages
    return qvariant_cast< QVariantMap >(parent()->property("packages"));
}

int TransactionAdaptor::progress() const
{
    // get the value of property progress
    return qvariant_cast< int >(parent()->property("progress"));
}

QString TransactionAdaptor::proxy() const
{
    // get the value of property proxy
    return qvariant_cast< QString >(parent()->property("proxy"));
}

int TransactionAdaptor::role() const
{
    // get the value of property role
    return qvariant_cast< int >(parent()->property("role"));
}

int TransactionAdaptor::status() const
{
    // get the value of property status
    return qvariant_cast< int >(parent()->property("status"));
}

QString TransactionAdaptor::statusDetails() const
{
    // get the value of property statusDetails
    return qvariant_cast< QString >(parent()->property("statusDetails"));
}

QString TransactionAdaptor::transactionId() const
{
    // get the value of property transactionId
    return qvariant_cast< QString >(parent()->property("transactionId"));
}

QStringList TransactionAdaptor::untrustedPackages() const
{
    // get the value of property untrustedPackages
    return qvariant_cast< QStringList >(parent()->property("untrustedPackages"));
}

int TransactionAdaptor::userId() const
{
    // get the value of property userId
    return qvariant_cast< int >(parent()->property("userId"));
}

void TransactionAdaptor::cancel()
{
    // handle method call org.kubuntu.qaptworker2.transaction.cancel
    QMetaObject::invokeMethod(parent(), "cancel");
}

void TransactionAdaptor::provideMedium(const QString &medium)
{
    // handle method call org.kubuntu.qaptworker2.transaction.provideMedium
    QMetaObject::invokeMethod(parent(), "provideMedium", Q_ARG(QString, medium));
}

void TransactionAdaptor::replyUntrustedPrompt(bool approved)
{
    // handle method call org.kubuntu.qaptworker2.transaction.replyUntrustedPrompt
    QMetaObject::invokeMethod(parent(), "replyUntrustedPrompt", Q_ARG(bool, approved));
}

void TransactionAdaptor::resolveConfigFileConflict(const QString &currentPath, bool replace)
{
    // handle method call org.kubuntu.qaptworker2.transaction.resolveConfigFileConflict
    QMetaObject::invokeMethod(parent(), "resolveConfigFileConflict", Q_ARG(QString, currentPath), Q_ARG(bool, replace));
}

void TransactionAdaptor::run()
{
    // handle method call org.kubuntu.qaptworker2.transaction.run
    QMetaObject::invokeMethod(parent(), "run");
}

void TransactionAdaptor::setFrontendCaps(int caps)
{
    // handle method call org.kubuntu.qaptworker2.transaction.setFrontendCaps
    QMetaObject::invokeMethod(parent(), "setFrontendCaps", Q_ARG(int, caps));
}

void TransactionAdaptor::setProperty(int property, const QDBusVariant &value)
{
    // handle method call org.kubuntu.qaptworker2.transaction.setProperty
    QMetaObject::invokeMethod(parent(), "setProperty", Q_ARG(int, property), Q_ARG(QDBusVariant, value));
}

