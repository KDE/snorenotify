#ifndef PUSHOVERCONSTANTS_H
#define PUSHOVERCONSTANTS_H

#include "libsnore/snoreglobals.h"

namespace PushoverConstants {
static const Snore::SettingsKey UserKey = {QStringLiteral("UserKey"), Snore::GlobalSetting};
static const Snore::SettingsKey Devices = {QStringLiteral("Devices"), Snore::LocalSetting};
static const Snore::SettingsKey Sound = {QStringLiteral("Sound"), Snore::LocalSetting};

//frontend
static const Snore::SettingsKey Secret = {QStringLiteral("Secret"), Snore::LocalSetting};
static const Snore::SettingsKey DeviceName = {QStringLiteral("DeviceName"), Snore::LocalSetting};
static const Snore::SettingsKey DeviceID = {QStringLiteral("DeviceID"), Snore::LocalSetting};

}
#endif // PUSHOVERCONSTANTS_H
