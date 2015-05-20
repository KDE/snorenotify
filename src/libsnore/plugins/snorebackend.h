/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2013-2014  Patrick von Reth <vonreth@kde.org>

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

#ifndef SNORE_BACKEND_H
#define SNORE_BACKEND_H
#include "libsnore/snore_exports.h"
#include "libsnore/plugins/plugins.h"
#include "libsnore/notification/notification.h"
#include "libsnore/snore.h"
#include <QMap>
#include <QTimer>

namespace Snore
{

class SNORE_EXPORT SnoreBackend : public SnorePlugin
{
    Q_OBJECT
    Q_INTERFACES(Snore::SnorePlugin)
public:
    SnoreBackend(const  QString &name, bool canCloseNotification, bool supportsRichtext, bool canUpdateNotifications = false);
    virtual ~SnoreBackend();
    virtual bool initialize() override;
    virtual bool deinitialize() override;

    void requestCloseNotification(Snore::Notification notification, Notification::CloseReasons reason);

    bool canCloseNotification() const;
    bool canUpdateNotification() const;
    bool supportsRichtext() const;
    
    //handle scheduled notifications
    virtual QList<Notification> scheduledNotifications();
    virtual void removeScheduledNotification(Notification notification);
    virtual void scheduleNotification(Notification notification);

signals:
    void scheduledNotificationsChanged(QList<Notification> notifications);
    void notificationClosed(Snore::Notification);

public slots:
    virtual void slotRegisterApplication(const Snore::Application &application);
    virtual void slotDeregisterApplication(const Snore::Application &application);
    virtual void slotNotify(Snore::Notification notification) = 0;
    virtual void slotCloseNotification(Snore::Notification notification);

protected slots:
    void slotNotificationDisplayed(Notification notification);
    void slotNotificationActionInvoked(Notification notification, const Action &action = Action());

protected:
    void closeNotification(Snore::Notification, Snore::Notification::CloseReasons);
    void setSupportsRichtext(bool b);

private:
    bool m_canCloseNotification;
    bool m_supportsRichtext;
    bool m_canUpdateNotification;
    QMap<int,Notification> m_scheduled_notifications;
    QMap<int,QTimer *> m_timer_for_notification_id;

};

}
Q_DECLARE_INTERFACE(Snore::SnoreBackend,
                    "org.Snore.NotificationBackend/1.0")

namespace Snore
{
class SnoreCore;

class SNORE_EXPORT SnoreSecondaryBackend : public SnorePlugin
{
    Q_OBJECT
    Q_INTERFACES(Snore::SnorePlugin Snore::SnorePlugin)
public:
    SnoreSecondaryBackend(const  QString &name, bool supportsRhichtext);
    virtual ~SnoreSecondaryBackend();
    virtual bool initialize();
    virtual bool deinitialize();

    bool supportsRichtext();

public slots:
    virtual void slotNotify(Snore::Notification notification);
    virtual void slotNotificationDisplayed(Snore::Notification notification);

protected:
    bool m_supportsRichtext;

};

}

Q_DECLARE_INTERFACE(Snore::SnoreSecondaryBackend,
                    "org.Snore.SecondaryNotificationBackend/1.0")

#endif//SNORE_BACKEND_H
