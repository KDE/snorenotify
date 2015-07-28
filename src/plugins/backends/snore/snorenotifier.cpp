/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2014  Patrick von Reth <vonreth@kde.org>

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

#include "snorenotifier.h"
#include "notifywidget.h"
#include "snorenotifiersettings.h"
#include "libsnore/notification/notification_p.h"
#include "libsnore/snore_p.h"

#include <QThread>

using namespace Snore;

SnoreNotifier::SnoreNotifier():
    m_widgets(3),
    m_timer(new QTimer(this))
{
    for (int i = 0; i < m_widgets.size(); ++i) {
        NotifyWidget *w = new NotifyWidget(i, this);
        m_widgets[i] = w;
        connect(w, &NotifyWidget::dismissed, [this, w]() {
            Notification notification = w->notification();
            closeNotification(notification, Notification::DISMISSED);
            slotCloseNotification(notification);
        });

        connect(w, &NotifyWidget::invoked, [this, w]() {
            Notification notification = w->notification();
            slotNotificationActionInvoked(notification);
            closeNotification(notification, Notification::ACTIVATED);
            slotCloseNotification(notification);
        });
    }

    m_timer->setInterval(500);
    connect(m_timer, &QTimer::timeout, [this]() {
        if (m_queue.isEmpty()) {
            snoreDebug(SNORE_DEBUG) << "queue is empty";
            m_timer->stop();
        } else {
            for (NotifyWidget *w : m_widgets) {
                if (w->acquire()) {
                    Notification notification = m_queue.takeFirst();
                    w->display(notification);
                    notification.hints().setPrivateValue(this, "id", w->id());
                    slotNotificationDisplayed(notification);
                    if (m_queue.isEmpty()) {
                        m_timer->stop();
                        return;
                    }
                }
            }
        }
    });

}

SnoreNotifier::~SnoreNotifier()
{
    for (auto w : m_widgets) {
        w->deleteLater();
    }
}

void SnoreNotifier::slotNotify(Snore::Notification notification)
{
    auto display = [this](NotifyWidget * w, Snore::Notification notification) {
        w->display(notification);
        notification.hints().setPrivateValue(this, "id", w->id());
        slotNotificationDisplayed(notification);
    };

    if (notification.isUpdate()) {
        if (notification.old().hints().privateValue(this, "id").isValid()) {
            NotifyWidget *w = m_widgets[notification.old().hints().privateValue(this, "id").toInt()];
            if (w->isVisible() && w->notification().isValid() && w->notification().id() == notification.old().id()) {
                snoreDebug(SNORE_DEBUG) << "replacing notification" << w->notification().id() << notification.id();
                display(w, notification);
            }
        } else {
            for (int i = 0; i < m_queue.length(); ++i) {
                Notification n = m_queue.at(i);
                if (n.id() == notification.old().id()) {
                    snoreDebug(SNORE_DEBUG) << "replacing qued notification" << n.id() << notification.id();
                    m_queue.replace(i, notification);
                }
            }
        }
    } else {
        if (m_queue.isEmpty()) {
            for (NotifyWidget *w : m_widgets) {
                if (w->acquire()) {
                    display(w, notification);
                    return;
                }
            }
        }
        m_queue.append(notification);
        m_timer->start();
    }
}

void SnoreNotifier::slotCloseNotification(Snore::Notification notification)
{
    NotifyWidget *w = m_widgets[notification.hints().privateValue(this, "id").toInt()];
    w->release();
    //the timer will show the next
}

bool SnoreNotifier::canCloseNotification() const
{
    return true;
}

bool SnoreNotifier::canUpdateNotification() const
{
    return true;
}

PluginSettingsWidget *SnoreNotifier::settingsWidget()
{
    return new SnoreNotifierSettings(this);
}

void SnoreNotifier::setDefaultSettings()
{
    setDefaultSettingsValue(QLatin1String("Position"), Qt::TopRightCorner);
    SnoreBackend::setDefaultSettings();
}
