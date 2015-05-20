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
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "snore.h"
#include "snore_p.h"

#include <QAbstractButton>
#include <QTabWidget>
#include <QFormLayout>
#include <QCheckBox>

using namespace Snore;

SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    initTabs();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::initTabs()
{
    SnorePlugin::PluginTypes types = SnoreCore::instance().value("PluginTypes", LOCAL_SETTING).value<SnorePlugin::PluginTypes>();
    if (types == SnorePlugin::NONE) {
        types = SnorePlugin::ALL;
    }
    auto addWidgets = [&](QTabWidget * target, QWidget * container, SnorePlugin::PluginTypes type) {
        bool enabled = false;
        target->clear();
        if (types & type) {
            for (PluginSettingsWidget *widget : SnoreCore::instance().settingWidgets(type)) {
                target->addTab(widget, widget->name());
                m_tabs.append(widget);
                enabled = true;
            }
        }
        if (enabled) {
            if (ui->tabWidget->indexOf(container) == -1) {
                ui->tabWidget->addTab(container, container->property("TAB_NAME").toString());
            }
        } else {
            int index = ui->tabWidget->indexOf(container);
            container->setProperty("TAB_NAME", ui->tabWidget->tabText(index));
            ui->tabWidget->removeTab(index);
        }
    };
    addWidgets(ui->tabWidget_backends, ui->tab_backends, SnorePlugin::BACKEND);
    addWidgets(ui->tabWidget_secondary_backends, ui->tab_secondaryBackends, SnorePlugin::SECONDARY_BACKEND);
    addWidgets(ui->tabWidget_frontends, ui->tab_frontends,  SnorePlugin::FRONTEND);
    addWidgets(ui->tabWidget_plugins, ui->tab_plugins, SnorePlugin::PLUGIN);
}

void Snore::SettingsDialog::on_pushButton_clicked()
{
    Application app = SnoreCorePrivate::instance()->defaultApplication();
    Notification noti(app, app.defaultAlert(), tr("Hello World"),
                      QString("<i>%1</i><br>"
                              "<a href=\"https://github.com/Snorenotify/Snorenotify\">%2</a><br>").arg(tr("This is Snore"), tr("Project Website")),
                      app.icon());
    noti.addAction(Action(1, tr("Test Action")));
    SnoreCore::instance().broadcastNotification(noti);
}

void SettingsDialog::load()
{
    snoreDebug(SNORE_DEBUG) << "loading";
    if (SnoreCore::instance().value("PluginTypes", LOCAL_SETTING).value<SnorePlugin::PluginTypes>() & SnorePlugin::BACKEND) {
        ui->primaryBackendComboBox->clear();
        QStringList list = SnoreCore::instance().pluginNames(SnorePlugin::BACKEND);
        ui->primaryBackendComboBox->setVisible(true);
        ui->primaryBackendLabel->setVisible(true);
        ui->primaryBackendComboBox->addItems(list);
        ui->primaryBackendComboBox->setCurrentIndex(list.indexOf(SnoreCore::instance().value("PrimaryBackend", LOCAL_SETTING).toString()));
    } else {
        ui->primaryBackendComboBox->setVisible(false);
        ui->primaryBackendLabel->setVisible(false);
    }
    ui->timeoutSpinBox->setValue(SnoreCore::instance().value("Timeout", LOCAL_SETTING).toInt());
    for (auto widget : m_tabs) {
        widget->loadSettings();
    }
}

void SettingsDialog::save()
{
    snoreDebug(SNORE_DEBUG) << "saving";
    bool dirty = false;
    for (auto w : m_tabs) {
        w->saveSettings();
        dirty |= w->isDirty();
    }
    dirty |= SnoreCore::instance().value("PrimaryBackend", LOCAL_SETTING).toString() != ui->primaryBackendComboBox->currentText();
    dirty |= SnoreCore::instance().value("Timeout", LOCAL_SETTING).toInt() != ui->timeoutSpinBox->value();
    SnoreCore::instance().setValue("PrimaryBackend", ui->primaryBackendComboBox->currentText(), LOCAL_SETTING);
    SnoreCore::instance().setValue("Timeout", ui->timeoutSpinBox->value(), LOCAL_SETTING);

    if (dirty) {
        SnoreCorePrivate::instance()->syncSettings();
    }
}

void SettingsDialog::setVisible(bool b)
{
    if (b) {
        load();
    } else {
        save();
    }
    QWidget::setVisible(b);
}

void SettingsDialog::accept()
{
    save();
}

void SettingsDialog::reset()
{
    load();
}

