#ifndef GROWLCONSTANTS_H
#define GROWLCONSTANTS_H

#include "libsnore/snoreglobals.h"

namespace GrowlConstants {
static const Snore::SettingsKey Host = {QStringLiteral("Host"), Snore::GlobalSetting};
static const Snore::SettingsKey Password = {QStringLiteral("Password"), Snore::GlobalSetting};

}

#endif // GROWLCONSTANTS_H
