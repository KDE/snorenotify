#ifndef SOUNDCONSTANTS_H
#define SOUNDCONSTANTS_H

#include "libsnore/snoreglobals.h"

namespace SoundConstants {
static const Snore::SettingsKey Volume = {QStringLiteral("Volume"), Snore::GlobalSetting};
static const Snore::SettingsKey SoundKey = {QStringLiteral("Sound"), Snore::LocalSetting};

}
#endif // SOUNDCONSTANTS_H
