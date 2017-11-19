/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <QtWidgets>
#include <QPrinter>
#include "bgi_airwire.h"
#include "../items/bi_airwire.h"
#include "../board.h"
#include "../boardlayerstack.h"

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

BGI_AirWire::BGI_AirWire(BI_AirWire& airwire) noexcept :
    BGI_Base(), mAirWire(airwire), mLayer(nullptr)
{
    mLayer = getLayer(GraphicsLayer::sBoardAirWires);
    updateCacheAndRepaint();
}

BGI_AirWire::~BGI_AirWire() noexcept
{
}

/*****************************************************************************************
 *  Getters
 ****************************************************************************************/

bool BGI_AirWire::isSelectable() const noexcept
{
    return mLayer && mLayer->isVisible();
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void BGI_AirWire::updateCacheAndRepaint() noexcept
{
    prepareGeometryChange();

    mLineF.setP1(mAirWire.getP1().toPxQPointF());
    mLineF.setP2(mAirWire.getP2().toPxQPointF());

    mShape = QPainterPath();
    mShape.moveTo(mAirWire.getP1().toPxQPointF());
    mShape.lineTo(mAirWire.getP2().toPxQPointF());
    QPainterPathStroker ps;
    ps.setCapStyle(Qt::RoundCap);
    ps.setWidth(Length(100000).toPx());
    mShape = ps.createStroke(mShape);
    mBoundingRect = mShape.boundingRect();

    update();
}

/*****************************************************************************************
 *  Inherited from QGraphicsItem
 ****************************************************************************************/

void BGI_AirWire::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    bool highlight = false; // TODO

    // draw line
    if (mLayer && mLayer->isVisible()) {
        QPen pen(mLayer->getColor(highlight), 0, Qt::SolidLine, Qt::RoundCap);
        painter->setPen(pen);
        painter->drawLine(mLineF);
    }

#ifdef QT_DEBUG
    GraphicsLayer* layer = getLayer(GraphicsLayer::sDebugGraphicsItemsBoundingRects); Q_ASSERT(layer);
    if (layer && layer->isVisible()) {
        // draw bounding rect
        painter->setPen(QPen(layer->getColor(highlight), 0));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(mBoundingRect);
    }
#endif
}

/*****************************************************************************************
 *  Private Methods
 ****************************************************************************************/

GraphicsLayer* BGI_AirWire::getLayer(const QString& name) const noexcept
{
    return mAirWire.getBoard().getLayerStack().getLayer(name);
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
