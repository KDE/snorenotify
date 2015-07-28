/*
    SnoreNotify is a Notification Framework based on Qt
    Copyright (C) 2015  Patrick von Reth <vonreth@kde.org>

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
#include "toastysettings.h"

#include "plugins/plugins.h"

#include <QLineEdit>

ToastySettings::ToastySettings(Snore::SnorePlugin *plugin, QWidget *parent) :
    Snore::PluginSettingsWidget(plugin, parent),
    m_lineEdit(new QLineEdit)
{
    addRow(tr("Device ID:"), m_lineEdit);
}

ToastySettings::~ToastySettings()
{
}

void ToastySettings::load()
{
    m_lineEdit->setText(settingsValue(QLatin1String("DeviceID")).toString());
}

void ToastySettings::save()
{
    setSettingsValue(QLatin1String("DeviceID"), m_lineEdit->text());
}
