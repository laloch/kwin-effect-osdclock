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

#include "osdclock.h"

// KConfigSkeleton
#include "osdclockconfig.h"

#include <kwinconfig.h>

#include <kwinglutils.h>
#ifdef KWIN_HAVE_XRENDER_COMPOSITING
#include <kwinxrenderutils.h>
#include <xcb/render.h>
#endif

#include <KLocalizedString>
#include <math.h>
#include <QPainter>
#include <QVector2D>
#include <QTimer>

#include "blur.h"

using namespace KWin;

OsdClockEffect::OsdClockEffect()
{
    reconfigure(ReconfigureAll);

    connect(effects, &EffectsHandler::numberScreensChanged,
            this, &OsdClockEffect::numberScreensChanged);
    connect(effects, &EffectsHandler::screenGeometryChanged,
            this, &OsdClockEffect::screenGeometryChanged);

    repaintTimer = new QTimer(this);
    repaintTimer->setInterval(1000);
    connect(repaintTimer, &QTimer::timeout, this, &OsdClockEffect::repaintTimerTick);
    repaintTimer->start();
}

void OsdClockEffect::numberScreensChanged()
{
    reconfigure(ReconfigureAll);
}

void OsdClockEffect::screenGeometryChanged(const QSize &size)
{
    reconfigure(ReconfigureAll);
}

void OsdClockEffect::reconfigure(ReconfigureFlags)
{
    OsdClockConfig::self()->read();
    alpha = OsdClockConfig::alpha();
    x = OsdClockConfig::x();
    y = OsdClockConfig::y();
    const QSize screenSize = effects->virtualScreenSize();

    int textPosition = OsdClockConfig::textPosition();
    textFont = OsdClockConfig::textFont();
    textColor = OsdClockConfig::textColor();
    double textAlpha = OsdClockConfig::textAlpha();

    if (!textColor.isValid())
        textColor = QPalette().color(QPalette::Active, QPalette::WindowText);
    textColor.setAlphaF(textAlpha);

    QFontMetrics fm(textFont);
    int textWidth = fm.width("00:00");
    //int textHeigt = fm.height();

    switch(textPosition) {
    case TOP_LEFT:
        clockTextRect = QRect(0 + x, 0 + y, textWidth, 100);
        textAlign = Qt::AlignTop | Qt::AlignLeft;
        break;
    case TOP_RIGHT:
        clockTextRect = QRect(screenSize.width() - textWidth - x, 0 + y , textWidth, 100);
        textAlign = Qt::AlignTop | Qt::AlignLeft;
        break;
    case BOTTOM_LEFT:
        clockTextRect = QRect(0 + x, screenSize.height() - 100 - y, textWidth, 100);
        textAlign = Qt::AlignBottom | Qt::AlignLeft;
        break;
    case BOTTOM_RIGHT:
        clockTextRect = QRect(screenSize.width() - textWidth - x, screenSize.height() - 100 - y, textWidth, 100);
        textAlign = Qt::AlignBottom | Qt::AlignLeft;
        break;
    case NOWHERE:
    default:
        clockTextRect = QRect();
        break;
    }
    lastRenderTime = QDateTime();
    effects->addRepaintFull();
}

void OsdClockEffect::prePaintScreen(ScreenPrePaintData& data, int time)
{
    effects->prePaintScreen(data, time);
    data.paint += clockTextRect;
}

void OsdClockEffect::paintWindow(EffectWindow* w, int mask, QRegion region, WindowPaintData& data)
{
    effects->paintWindow(w, mask, region, data);
}

void OsdClockEffect::paintScreen(int mask, QRegion region, ScreenPaintData& data)
{
    effects->paintScreen(mask, region, data);
    if (effects->isOpenGLCompositing()) {
        paintGL(data.projectionMatrix());
        glFinish(); // make sure all rendering is done
    }
#ifdef KWIN_HAVE_XRENDER_COMPOSITING
    if (effects->compositingType() == XRenderCompositing) {
        paintXrender();
        xcb_flush(xcbConnection());   // make sure all rendering is done
    }
#endif
    if (effects->compositingType() == QPainterCompositing) {
        paintQPainter();
    }
}

void OsdClockEffect::paintGL(const QMatrix4x4 &projectionMatrix)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    if (clockTextRect.isValid()) {
        clockText.reset(new GLTexture(img));
        clockText->bind();
        ShaderBinder binder(ShaderTrait::MapTexture);
        QMatrix4x4 mvp = projectionMatrix;
        mvp.translate(clockTextRect.x(), clockTextRect.y());
        binder.shader()->setUniform(GLShader::ModelViewProjectionMatrix, mvp);
        clockText->render(QRegion(clockTextRect), clockTextRect);
        clockText->unbind();
    }

    // Paint paint sizes
    glDisable(GL_BLEND);
}

#ifdef KWIN_HAVE_XRENDER_COMPOSITING
/*
 Differences between OpenGL and XRender:
 - differently specified rectangles (X: width/height, O: x2,y2)ew GLTexture(img
 - XRender uses pre-multiplied alpha
*/
void OsdClockEffect::paintXrender()
{
    if (clockTextRect.isValid()) {
        XRenderPicture textPic(img);
        xcb_render_composite(xcbConnection(), XCB_RENDER_PICT_OP_OVER, textPic, XCB_RENDER_PICTURE_NONE,
                        effects->xrenderBufferPicture(), 0, 0, 0, 0, clockTextRect.x(), clockTextRect.y(), img.width(), img.height());
        effects->addRepaint(clockTextRect);
    }
}
#endif

void OsdClockEffect::paintQPainter()
{
    QPainter *painter = effects->scenePainter();
    painter->save();

    QColor color(255, 255, 255);
    color.setAlphaF(alpha);

    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter->setPen(Qt::black);
    painter->drawText(clockTextRect, textAlign, QDateTime::currentDateTime().toString("HH:mm"));

    painter->restore();
}

void OsdClockEffect::postPaintScreen()
{
    effects->postPaintScreen();
}

QImage OsdClockEffect::clockTextImage(QDateTime t)
{
    QImage im(clockTextRect.width(), clockTextRect.height(), QImage::Format_ARGB32);
    im.fill(Qt::transparent);
    QPainter painter(&im);
    QPainterPath path, path2;
    QPen pen;
    if (textColor.value() < 127)
      pen = QPen(textColor.lighter(200));
    else
      pen = QPen(textColor.darker(300));
    QFont smallFont(textFont);
    pen.setWidth(2);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(textFont);
    painter.setPen(pen);
    painter.setBrush(textColor);
    path.addText(0, 55, textFont, t.toString("HH:mm"));
    painter.drawPath(path);
    smallFont.setPointSizeF(smallFont.pointSizeF() * 0.30);
    path2.addText(12, 85, smallFont, t.toString("yyyy-MM-dd"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawPath(path2);
    QImage blur(im);
    Blur::expblur(blur, 5, 16, 7);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, blur);
    painter.end();
    return im;
}

void OsdClockEffect::repaintTimerTick()
{
    if (!lastRenderTime.isValid() ||
        lastRenderTime.time().minute() != QTime::currentTime().minute())
    {
        lastRenderTime = QDateTime::currentDateTime();
        img = clockTextImage(lastRenderTime);
        effects->addRepaint(clockTextRect);
    }
}
