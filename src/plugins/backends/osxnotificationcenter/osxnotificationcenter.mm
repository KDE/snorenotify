#include "osxnotificationcenter.h"
#include "libsnore/plugins/snorebackend.h"
#include "libsnore/notification/notification_p.h"
#include "libsnore/utils.h"
#include "libsnore/snore.h"
#include "libsnore/log.h"

#import <QThread.h>
#import <QApplication.h>
#import <QMap>
#include <Foundation/Foundation.h>

using namespace Snore;

QMap<int, Notification> id_to_notification;
NSMutableDictionary * id_to_nsnotification;
NSSet * old_notifications;


void emitNotificationClicked(Notification notification) {
    emit SnoreCore::instance().actionInvoked(notification);
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
    int notification_id = [notification.userInfo[@"id"] intValue];
    [center removeDeliveredNotification: notification];
    if (not id_to_notification.contains(notification_id)) {
            snoreDebug(SNORE_WARNING) << "User clicked on notification that was not recognized";
            return;
    }
    Notification snore_notification = id_to_notification[notification_id];
    snore_notification.data()->setCloseReason(Notification::ACTIVATED);
    emitNotificationClicked(snore_notification);
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


// store some variables that are needed (since obj-c++ does not allow having obj-c classes as c++ members)
namespace {

    NSString *NSStringFromQString(QString qstr) {
        return [NSString stringWithUTF8String: qstr.toUtf8().data()];
    }
}

UserNotificationItemClass * delegate;

OSXNotificationCenter::OSXNotificationCenter() : SnoreBackend("OSX Notification Center", false, false, false)
{
    id_to_nsnotification = [[[NSMutableDictionary alloc] init] autorelease];
    old_notifications = [NSSet setWithArray: [NSUserNotificationCenter defaultUserNotificationCenter].scheduledNotifications];
    delegate = new UserNotificationItemClass();
}

OSXNotificationCenter::~OSXNotificationCenter()
{
    delete delegate;
}

bool OSXNotificationCenter::initialize()
{
    return SnoreBackend::initialize();
}

QList<Notification> OSXNotificationCenter::scheduledNotifications() {
    QList<Notification> result;
    NSArray * scheduled_nsnotifications = [NSUserNotificationCenter defaultUserNotificationCenter].scheduledNotifications;
    for (NSUserNotification *notification in scheduled_nsnotifications)
    {
        if (not [old_notifications containsObject: notification]) {
            result.push_back(id_to_notification[[notification.userInfo[@"id"] intValue]]);
        }
    }
    return result;
}

void OSXNotificationCenter::removeScheduledNotification(Notification notification) {
    NSString * notification_id = [NSString stringWithFormat:@"%d",notification.id()];
    NSUserNotification * ns_notification = id_to_nsnotification[notification_id];
    if (ns_notification) {
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeScheduledNotification: ns_notification];
        emit scheduledNotificationsChanged(scheduledNotifications());
    }
    else {
        snoreDebug(SNORE_WARNING) << "Non existing NSNotification";
        NSMutableString *content = [[NSMutableString alloc] init];
        for (NSString *aKey in id_to_nsnotification.allKeys) {
            [content appendFormat:@"%@ : %@\n",aKey,[id_to_nsnotification valueForKey:aKey]];
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
    bool valid_time = notification.deliveryDate().isValid();
    NSUserNotification *osx_notification = [[[NSUserNotification alloc] init] autorelease];
    NSString * notification_id = [NSString stringWithFormat:@"%d",notification.id()];
    osx_notification.title = NSStringFromQString(Utils::toPlainText(notification.title()));
    osx_notification.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:notification_id, @"id", nil];
    if (valid_time) {
        osx_notification.deliveryDate = [NSDate dateWithTimeIntervalSince1970:notification.deliveryDate().toTime_t()];
    }
    osx_notification.informativeText = NSStringFromQString(Utils::toPlainText(notification.text()));


    // Add notification to mapper from id to Nofification / NSUserNotification
    id_to_notification.insert(notification.id(), notification);
    [id_to_nsnotification setObject:osx_notification forKey: notification_id];
    if (valid_time) {
        [[NSUserNotificationCenter defaultUserNotificationCenter] scheduleNotification: osx_notification];
        emit scheduledNotificationsChanged(scheduledNotifications());
    }
    else {
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification: osx_notification];
    }

    slotNotificationDisplayed(notification);
}

