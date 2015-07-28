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

#include "snorebackend.h"
#include "../snore.h"
#include "../snore_p.h"
#include "../application.h"
#include "../notification/notification.h"
#include "../notification/notification_p.h"

#include <QTimer>
#include <QThread>
#include <QMetaMethod>

using namespace Snore;

SnoreBackend::SnoreBackend()
{
    connect(this, &SnoreBackend::enabledChanged, [this](bool enabled) {
        if (enabled) {
            connect(SnoreCorePrivate::instance(), &SnoreCorePrivate::applicationRegistered, this, &SnoreBackend::slotRegisterApplication, Qt::QueuedConnection);
            connect(SnoreCorePrivate::instance(), &SnoreCorePrivate::applicationDeregistered, this, &SnoreBackend::slotDeregisterApplication, Qt::QueuedConnection);

            connect(this, &SnoreBackend::notificationClosed, SnoreCorePrivate::instance(), &SnoreCorePrivate::slotNotificationClosed, Qt::QueuedConnection);
            connect(SnoreCorePrivate::instance(), &SnoreCorePrivate::notify, this, &SnoreBackend::slotNotify, Qt::QueuedConnection);

            connect(SnoreCorePrivate::instance(), &SnoreCorePrivate::scheduleNotification, this, &SnoreBackend::scheduleNotification, Qt::QueuedConnection);

            for (const Application &a : SnoreCore::instance().aplications()) {
                slotRegisterApplication(a);
            }
        } else {
            for (const Application &a : SnoreCore::instance().aplications()) {
                slotDeregisterApplication(a);
            }
            disconnect(SnoreCorePrivate::instance(), &SnoreCorePrivate::applicationRegistered, this, &SnoreBackend::slotRegisterApplication);
            disconnect(SnoreCorePrivate::instance(), &SnoreCorePrivate::applicationDeregistered, this, &SnoreBackend::slotDeregisterApplication);

            disconnect(this, &SnoreBackend::notificationClosed, SnoreCorePrivate::instance(), &SnoreCorePrivate::slotNotificationClosed);
            disconnect(SnoreCorePrivate::instance(), &SnoreCorePrivate::notify, this, &SnoreBackend::slotNotify);

            disconnect(SnoreCorePrivate::instance(), &SnoreCorePrivate::scheduleNotification, this, &SnoreBackend::scheduleNotification);

        }
    });
}

SnoreBackend::~SnoreBackend()
{
    snoreDebug(SNORE_DEBUG) << "Deleting" << name();
}

void SnoreBackend::requestCloseNotification(Notification notification, Notification::CloseReasons reason)
{
    if (notification.isValid()) {
        if (canCloseNotification()) {
            slotCloseNotification(notification);
            closeNotification(notification, reason);
        }
    }
}

void SnoreBackend::closeNotification(Notification n, Notification::CloseReasons reason)
{
    if (!n.isValid()) {
        return;
    }
    n.removeActiveIn(this);
    if (n.isUpdate()) {
        n.old().removeActiveIn(this);
    }
    n.data()->setCloseReason(reason);
    snoreDebug(SNORE_DEBUG) << n << reason;
    emit notificationClosed(n);
}

void SnoreBackend::slotCloseNotification(Notification notification)
{
    Q_UNUSED(notification)
}

bool SnoreBackend::canCloseNotification() const
{
    return false;
}

bool SnoreBackend::canUpdateNotification() const
{
    return false;
}

QList<Notification> SnoreBackend::scheduledNotifications() {
    return m_scheduledNotifications.values();
}

void SnoreBackend::removeScheduledNotification(Notification notification) {
    if (m_timerForNotificationId.contains(notification.id())) {
        m_timerForNotificationId[notification.id()]->stop();
        m_scheduledNotifications.remove(notification.id());
        m_timerForNotificationId.remove(notification.id());
    }
    emit scheduledNotificationsChanged(scheduledNotifications());
}

void SnoreBackend::scheduleNotification(Notification notification) {
    if (!notification.deliveryDate().isValid()) {
        snoreDebug(SNORE_WARNING) << "Schedule notification without time";
        slotNotify(notification);
        return;
    }
    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    auto duration = QDateTime::currentDateTime().msecsTo(notification.deliveryDate());

    // Always emit notification timer doesn't trigger when interaval is negative
    if (duration < 0) {
        snoreDebug(SNORE_WARNING) << "Notification date is in past by " << duration << " miliseconds";
        duration = 0;
    }
    snoreDebug(SNORE_DEBUG) << "Scheduling timer with duration " << duration / 1000 << " seconds";
    timer->setInterval(duration);
    auto notificationId = notification.id();
    connect(timer, &QTimer::timeout, this, [&](){
        // Deliver scheduled notification
        this->slotNotify(m_scheduledNotifications[notificationId]);
        m_scheduledNotifications.remove(notificationId);
        m_timerForNotificationId.remove(notificationId);
        delete m_timerForNotificationId[notificationId];
        emit scheduledNotificationsChanged(scheduledNotifications());
        });
    m_timerForNotificationId.insert(notificationId, timer);
    m_scheduledNotifications.insert(notificationId, notification);
    timer->start();
    emit scheduledNotificationsChanged(scheduledNotifications());
}

void SnoreBackend::slotRegisterApplication(const Application &application)
{
    Q_UNUSED(application);
}

void SnoreBackend::slotDeregisterApplication(const Application &application)
{
    Q_UNUSED(application);
}

void SnoreBackend::slotNotificationDisplayed(Notification notification)
{
    notification.addActiveIn(this);
    SnoreCorePrivate::instance()->slotNotificationDisplayed(notification);
}

void SnoreBackend::slotNotificationActionInvoked(Notification notification, const Action &action)
{
    notification.data()->setActionInvoked(action);
    SnoreCorePrivate::instance()->slotNotificationActionInvoked(notification);
}

