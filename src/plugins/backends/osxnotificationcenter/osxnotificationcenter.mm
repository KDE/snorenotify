#include "osxnotificationcenter.h"
#include "libsnore/plugins/snorebackend.h"
#include "libsnore/notification/notification_p.h"
#include "libsnore/utils.h"
#include "libsnore/snore.h"
#include "libsnore/log.h"
#include <QDebug.h>
#import <QThread.h>
#import <QApplication.h>
#import <QMap>
#include <Foundation/Foundation.h>
#import <objc/runtime.h>
using namespace Snore;

QMap<int, Notification> m_IdToNotification;
NSMutableDictionary * m_IdToNSNotification;
NSSet * m_oldNotifications;

void emitNotificationClicked(Notification notification) {
    emit SnoreCore::instance().actionInvoked(notification);
}

//
// Overcome need for bundle identifier to display notification
//

#pragma mark - Swizzle NSBundle

@implementation NSBundle(swizle)
// Overriding bundleIdentifier works, but overriding NSUserNotificationAlertStyle does not work.
- (NSString *)__bundleIdentifier
{
    if (self == [NSBundle mainBundle] && ![[self __bundleIdentifier] length]) {
        return @"com.apple.Terminal";
    } else {
        return [self __bundleIdentifier];
    }
}

@end
BOOL installNSBundleHook()
{
    Class cls = objc_getClass("NSBundle");
    if (cls) {
        method_exchangeImplementations(class_getInstanceMethod(cls, @selector(bundleIdentifier)),
                                       class_getInstanceMethod(cls, @selector(__bundleIdentifier)));
        return YES;
    }
    return NO;
}


//
// Enable reaction when user clicks on NSUserNotification
//

@interface UserNotificationItem : NSObject<NSUserNotificationCenterDelegate> { }

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification;
@end

@implementation UserNotificationItem
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification {
    Q_UNUSED(center);
    Q_UNUSED(notification);
    return YES;
}
- (void) userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification
{
    
    snoreDebug(SNORE_DEBUG) << "User clicked on notification";
    int notificationId = [notification.userInfo[@"id"] intValue];
    [center removeDeliveredNotification: notification];
    if (not m_IdToNotification.contains(notificationId)) {
        snoreDebug(SNORE_WARNING) << "User clicked on notification that was not recognized";
        return;
    }
    auto snoreNotification = m_IdToNotification.take(notificationId);
    snoreNotification.data()->setCloseReason(Notification::ACTIVATED);
    emitNotificationClicked(snoreNotification);
}
@end

class UserNotificationItemClass
{
public:
    UserNotificationItemClass() {
        item = [UserNotificationItem alloc];
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_8) {
            [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:item];
        }
    }
    ~UserNotificationItemClass() {
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_8) {
            [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:nil];
        }
        [item release];
    }
    UserNotificationItem *item;
};


UserNotificationItemClass * delegate = 0;
OSXNotificationCenter::OSXNotificationCenter()
{
    installNSBundleHook();
    m_IdToNSNotification = [[NSMutableDictionary alloc] init];
    m_oldNotifications = [NSSet setWithArray: [NSUserNotificationCenter defaultUserNotificationCenter].scheduledNotifications];
    if (not delegate) {
        delegate = new UserNotificationItemClass();
    }

}

OSXNotificationCenter::~OSXNotificationCenter()
{
    delete delegate;
}


QList<Notification> OSXNotificationCenter::scheduledNotifications() {
    QList<Notification> result;
    NSArray * scheduled_nsnotifications = [NSUserNotificationCenter defaultUserNotificationCenter].scheduledNotifications;
    for (NSUserNotification * notification in scheduled_nsnotifications)
    {
        if (not [m_oldNotifications containsObject: notification]) {
            result.push_back(m_IdToNotification[[notification.userInfo[@"id"] intValue]]);
        }
    }
    return result;
}

void OSXNotificationCenter::removeScheduledNotification(Notification notification) {
    NSString * notification_id = [NSString stringWithFormat:@"%d",notification.id()];
    NSUserNotification * ns_notification = m_IdToNSNotification[notification_id];
    if (ns_notification) {
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeScheduledNotification: ns_notification];
        emit scheduledNotificationsChanged(scheduledNotifications());
    }
    else {
        snoreDebug(SNORE_WARNING) << "Non existing NSNotification";
        NSMutableString *content = [[NSMutableString alloc] init];
        for (NSString *aKey in m_IdToNSNotification.allKeys) {
            [content appendFormat:@"%@ : %@\n",aKey,[m_IdToNSNotification valueForKey:aKey]];
        }
        snoreDebug(SNORE_WARNING) << "Content of id_to_nsnotification: " << [content UTF8String];
    }
}

void OSXNotificationCenter::scheduleNotification(Snore::Notification notification) {
    if (not notification.deliveryDate().isValid()) {
        snoreDebug(SNORE_WARNING) << "Schedule notification without delivery date";
    }
    slotNotify(notification);
}

void OSXNotificationCenter::slotNotify(Snore::Notification notification)
{
    qDebug() << "Displaying notification " << notification.text();
    bool validTime = notification.deliveryDate().isValid();
    NSUserNotification * osxNotification = [[[NSUserNotification alloc] init] autorelease];
    NSString * notificationId = [NSString stringWithFormat:@"%d",notification.id()];
    osxNotification.title = notification.title().toNSString();
    osxNotification.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:notificationId, @"id", nil];
    osxNotification.informativeText = notification.text().toNSString();

    if (validTime) {
        osxNotification.deliveryDate = [NSDate dateWithTimeIntervalSince1970:notification.deliveryDate().toTime_t()];
    }
    // Add notification to mapper from id to Nofification / NSUserNotification
    m_IdToNotification.insert(notification.id(), notification);
    [m_IdToNSNotification setObject:osxNotification forKey: notificationId];
    if (validTime) {
        [[NSUserNotificationCenter defaultUserNotificationCenter] scheduleNotification: osxNotification];
        emit scheduledNotificationsChanged(scheduledNotifications());
    }
    else {
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: osxNotification];
    }
}


