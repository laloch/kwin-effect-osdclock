/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2007 Rivo Laks <rivolaks@hot.ee>
Copyright (C) 2018 David Strobach <lalochcz@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#include "osdclock_config.h"

// KConfigSkeleton
#include "osdclockconfig.h"
#include <kwineffects_interface.h>

#include <KLocalizedString>
#include <KAboutData>
#include <KPluginFactory>

using namespace KWin;

K_PLUGIN_FACTORY_WITH_JSON(OsdClockEffectConfigFactory,
                           "osdclock_config.json",
                           registerPlugin<OsdClockEffectConfig>();)

OsdClockEffectConfig::OsdClockEffectConfig(QWidget* parent, const QVariantList& args) :
    KCModule(KAboutData::pluginData(QStringLiteral("kwin_osdclock")), parent, args)
{
    m_ui = new Ui::OsdClockEffectConfigForm;
    m_ui->setupUi(this);

    addConfig(OsdClockConfig::self(), this);

    load();
}

OsdClockEffectConfig::~OsdClockEffectConfig()
{
    delete m_ui;
}

void OsdClockEffectConfig::save()
{
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin_osdclock"));
}

#include "osdclock_config.moc"
