/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2015  Patrick von Reth <vonreth@kde.org>

    SnoreNotify is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SnoreNotify is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with SnoreNotify.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "nma.h"
#include "nmasettings.h"

#include"libsnore/utils.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

using namespace Snore;

void NotifyMyAndroid::slotNotify(Notification notification)
{
    QString key = value("ApiKey").toString();
    if (key.isEmpty()) {
        return;
    }

    QNetworkRequest request(QUrl("https://www.notifymyandroid.com/publicapi/notify"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));

    // TODO: use toHTML?
    QString data(QString("apikey=%1&application=%2&event=%3&description=%4&priority=%5&content-type=text/html")
                 .arg(key,
                      notification.application().name(),
                      notification.title(),
                      notification.text(Utils::HREF | Utils::BOLD | Utils::BREAK |
                                        Utils::UNDERLINE | Utils::FONT | Utils::ITALIC),
                      QString::number(notification.priority())));


    QNetworkReply *reply =  m_manager.post(request, data.toUtf8().constData());
    connect(reply, &QNetworkReply::finished, [reply]() {
        snoreDebug(SNORE_DEBUG) << reply->error();
        snoreDebug(SNORE_DEBUG) << reply->readAll();
        reply->close();
        reply->deleteLater();
    });

}

bool NotifyMyAndroid::initialize()
{
    setDefaultValue("ApiKey", "");
    return SnoreSecondaryBackend::initialize();
}

PluginSettingsWidget *NotifyMyAndroid::settingsWidget()
{
    return new NotifyMyAndroidSettings(this);
}
