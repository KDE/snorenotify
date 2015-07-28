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

#ifndef NOTIFYWIDGET_H
#define NOTIFYWIDGET_H

#include <QSharedMemory>
#include "libsnore/notification/notification.h"

#include <QtQuick/QtQuick>

class SnoreNotifier;

typedef struct {
    bool free;
    QTime date;

} SHARED_MEM_TYPE;

inline int SHARED_MEM_TYPE_REV()
{
    return 1;
}

class NotifyWidget : public QQuickView
{
    Q_OBJECT

public:
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(Qt::Corner corner READ corner)
    Q_PROPERTY(qlonglong wid READ wid)
    explicit NotifyWidget(int pos, const SnoreNotifier *parent);
    ~NotifyWidget();

    void display(const Snore::Notification &notification);

    bool acquire();
    bool release();

    Snore::Notification &notification();

    int id();
    Qt::Corner corner();
    qlonglong wid();

Q_SIGNALS:
    void invoked();
    void dismissed();

private Q_SLOTS:
    void slotDismissed();

    void slotInvoked();

private:
    QColor computeBackgrondColor(const QImage &img);

    Snore::Notification m_notification;
    QObject *m_qmlNotification;
    int m_id;
    const SnoreNotifier *m_parent;
    QSharedMemory m_mem;
    bool m_ready;
};

#endif // NOTIFYWIDGET_H
