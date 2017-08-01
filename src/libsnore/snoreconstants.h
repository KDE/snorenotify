#ifndef SNORECONSTANTS_H
#define SNORECONSTANTS_H

#include "snoreglobals.h"

#include <QString>

namespace Snore {
namespace Constants {
namespace SettingsKeys {
static const SettingsKey PrimaryBackend = {QStringLiteral("PrimaryBackend"), GlobalSetting};
static const SettingsKey Timeout = {QStringLiteral("Timeout"), GlobalSetting};
static const SettingsKey Silent = {QStringLiteral("Silent"), LocalSetting};
static const SettingsKey Enabled = {QStringLiteral("Enabled"), LocalSetting};
static const SettingsKey PluginTypes = {QStringLiteral("PluginTypes"), LocalSetting};

}

}
}

#endif // SNORECONSTANTS_H
