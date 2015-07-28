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

#ifndef SNORE_PLUGINS_H
#define SNORE_PLUGINS_H
#include "libsnore/snore_exports.h"
#include "libsnore/snoreglobals.h"
#include "libsnore/notification/notification.h"
#include "pluginsettingswidget.h"

#include <QHash>

namespace Snore
{

class PluginContainer;

class SNORE_EXPORT SnorePlugin : public QObject
{
    Q_OBJECT
public:
    enum PluginType {
        NONE                = 0,
        BACKEND             = 1 << 0,
        SECONDARY_BACKEND   = 1 << 1,
        FRONTEND            = 1 << 2,
        PLUGIN              = 1 << 3,
        ALL                 = ~0
    };

    Q_DECLARE_FLAGS(PluginTypes, PluginType)
    Q_ENUMS(PluginType)

    static PluginTypes typeFromString(const QString &t);
    static QString typeToString(const PluginTypes t);
    static QList<PluginTypes> types();

    SnorePlugin();
    virtual ~SnorePlugin();

    /**
     * Sets the enabled state of the plugin to @param enabled .
     */
    void setEnabled(bool enabled);

    /**
     * Enables the plugin.
     */
    void enable();

    /**
     * Disables the plugin.
     */
    void disable();

    /**
     * Returns whether the Plugin is enabled.
     */
    bool isEnabled() const;

    /**
     * Returns the name of the plugin.
     */
    const QString &name() const;

    /**
     * Returns the plugin type.
     */
    PluginTypes type() const;

    /**
     * Returns the name of the plugin type.
     */
    const QString typeName() const;

    virtual bool isReady();

    /**
     * Returns the error string or an empty string.
     */
    QString errorString() const;

    QVariant settingsValue(const QString &key, SettingsType type = GLOBAL_SETTING) const;
    void setSettingsValue(const QString &key, const QVariant &settingsValue, SettingsType type = GLOBAL_SETTING);
    void setDefaultSettingsValue(const QString &key, const QVariant &settingsValue, SettingsType type = GLOBAL_SETTING);

    virtual PluginSettingsWidget *settingsWidget();

Q_SIGNALS:
    void enabledChanged(bool enabled);
    void error(const QString &error);

protected:
    /**
     * Returns the version suffix used for the plugin settings.
     */
    virtual QString settingsVersion() const;

    /**
     * Set default setting values for the Plugin.
     */
    virtual void setDefaultSettings();

    void setErrorString(const QString &error);

private:
    QString normaliseKey(const QString &key) const;
    void setDefaultSettingsPlugin();

    bool m_enabled = false;
    PluginContainer *m_container = nullptr;
    QString m_error;

    friend class PluginContainer;

};

}
Q_DECLARE_OPERATORS_FOR_FLAGS(Snore::SnorePlugin::PluginTypes)
Q_DECLARE_METATYPE(Snore::SnorePlugin::PluginTypes)

Q_DECLARE_INTERFACE(Snore::SnorePlugin,
                    "org.Snore.SnorePlugin/1.0")

SNORE_EXPORT QDebug operator<<(QDebug, const Snore::SnorePlugin::PluginTypes &);
SNORE_EXPORT QDebug operator<<(QDebug, const Snore::SnorePlugin *);

SNORE_EXPORT QDataStream &operator<<(QDataStream &out, const Snore::SnorePlugin::PluginTypes &type);
SNORE_EXPORT QDataStream &operator>>(QDataStream &in, Snore::SnorePlugin::PluginTypes &type);

#endif//SNORE_PLUGINS_H
