#ifndef FREEDESKTOPNOTIFICATION_H
#define FREEDESKTOPNOTIFICATION_H
#include "libsnore/plugins/snorebackend.h"
#include "notificationinterface.h"

class  FreedesktopBackend: public Snore::SnoreBackend
{
    Q_OBJECT
    Q_INTERFACES(Snore::SnoreBackend)
    Q_PLUGIN_METADATA(IID "org.Snore.NotificationBackend/1.0" FILE "plugin.json")
public:
    FreedesktopBackend();
    ~FreedesktopBackend() = default;

    bool canCloseNotification() const override;
    bool canUpdateNotification() const override;

public Q_SLOTS:
    void slotNotify(Snore::Notification notification) override;
    void slotCloseNotification(Snore::Notification notification) override;

    void slotActionInvoked(const uint &id, const QString &actionID);
    void slotNotificationClosed(const uint &id, const uint &reason);

private:
    org::freedesktop::Notifications *m_interface;
    QHash<uint, Snore::Notification> m_dbusIdMap;
    bool m_supportsRichtext;

};

#endif // FREEDESKTOPNOTIFICATION_H
