/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2006 Lubos Lunak <l.lunak@kde.org>
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

#ifndef KWIN_OSDCLOCK_H
#define KWIN_OSDCLOCK_H

#include <QTime>
#include <QFont>

#include <kwineffects.h>
#include <kwingltexture.h>

class QTimer;

class OsdClockEffect
    : public KWin::Effect
{
    Q_OBJECT
    Q_PROPERTY(qreal alpha READ configuredAlpha)
    Q_PROPERTY(int x READ configuredX)
    Q_PROPERTY(int y READ configuredY)
    Q_PROPERTY(QRect clockTextRect READ configuredClockTextRect)
    Q_PROPERTY(int textAlign READ configuredTextAlign)
    Q_PROPERTY(QFont textFont READ configuredTextFont)
    Q_PROPERTY(QColor textColor READ configuredTextColor)
public:
    OsdClockEffect();
    virtual void reconfigure(ReconfigureFlags);
    virtual void prePaintScreen(KWin::ScreenPrePaintData& data, int time);
    virtual void paintScreen(int mask, QRegion region, KWin::ScreenPaintData& data);
    virtual void paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data);
    virtual void postPaintScreen();
    enum { NOWHERE, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

    // for properties
    qreal configuredAlpha() const {
        return alpha;
    }
    int configuredX() const {
        return x;
    }
    int configuredY() const {
        return y;
    }
    QRect configuredClockTextRect() const {
        return clockTextRect;
    }
    int configuredTextAlign() const {
        return textAlign;
    }
    QFont configuredTextFont() const {
        return textFont;
    }
    QColor configuredTextColor() const {
        return textColor;
    }
private Q_SLOTS:
    void numberScreensChanged();
    void screenGeometryChanged(const QSize &size);
private:
    void paintGL(const QMatrix4x4 &projectionMatrix);
#ifdef KWIN_HAVE_XRENDER_COMPOSITING
    void paintXrender();
#endif
    void paintQPainter();
    void repaintTimerTick();
    QImage clockTextImage(QDateTime t);
    QImage img;
    QDateTime lastRenderTime;
    double alpha;
    int x;
    int y;
    QScopedPointer<KWin::GLTexture> clockText;
    int textPosition;
    QFont textFont;
    QColor textColor;
    QRect clockTextRect;
    int textAlign;
    QTimer *repaintTimer;
};

#endif
