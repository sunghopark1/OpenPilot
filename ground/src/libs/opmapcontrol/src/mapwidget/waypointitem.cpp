/**
******************************************************************************
*
* @file       waypointitem.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
*             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
* @brief      A graphicsItem representing a WayPoint
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
*
*****************************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "waypointitem.h"
namespace mapcontrol
{
    WayPointItem::WayPointItem(const internals::PointLatLng &coord,int const& altitude, MapGraphicItem *map):coord(coord),reached(false),map(map),description(""),shownumber(true),isDragging(false),altitude(altitude)
    {
        text=0;
        numberI=0;
        picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        RefreshToolTip();
        number=WayPointItem::snumber;
        ++WayPointItem::snumber;
        this->setFlag(QGraphicsItem::ItemIsMovable,true);
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        this->setFlag(QGraphicsItem::ItemIsSelectable,true);
        transf.translate(picture.width()/2,picture.height());
        this->setTransform(transf);
        SetShowNumber(shownumber);
        RefreshPos();

    }
    WayPointItem::WayPointItem(const internals::PointLatLng &coord,int const& altitude, const QString &description, MapGraphicItem *map):coord(coord),reached(false),map(map),description(description),shownumber(true),isDragging(false),altitude(altitude)
    {
        text=0;
        numberI=0;
        picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        RefreshToolTip();
        number=WayPointItem::snumber;
        ++WayPointItem::snumber;
        this->setFlag(QGraphicsItem::ItemIsMovable,true);
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
        this->setFlag(QGraphicsItem::ItemIsSelectable,true);
        transf.translate(picture.width()/2,picture.height());
        this->setTransform(transf);
        SetShowNumber(shownumber);
        RefreshPos();
    }

    QRectF WayPointItem::boundingRect() const
    {
        return QRectF(-picture.width()/2,-picture.height(),picture.width(),picture.height());
        //return QRectF(0,0,50,50);
    }
    void WayPointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
//        if(!isDragging)
//        {
//            core::Point point=map->FromLatLngToLocal(coord);
//            this->setPos(point.X(),point.Y());
//        }
        painter->drawPixmap(-picture.width()/2,-picture.height(),picture);
        if(this->isSelected())
            painter->drawRect(QRectF(-picture.width()/2,-picture.height(),picture.width()-1,picture.height()-1));
    }
    void WayPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button()==Qt::LeftButton)
        {
            text=new QGraphicsSimpleTextItem(this);
            text->setPen(QPen(Qt::red));
            text->setPos(10,-picture.height());
            RefreshToolTip();
            isDragging=true;
        }
        QGraphicsItem::mousePressEvent(event);
    }
    void WayPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
        if(event->button()==Qt::LeftButton)
        {
            delete text;
            coord=map->FromLocalToLatLng(this->pos().x(),this->pos().y());
            isDragging=false;
            RefreshToolTip();
        }
        QGraphicsItem::mouseReleaseEvent(event);
    }
    void WayPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {

        if(isDragging)
        {
            coord=map->FromLocalToLatLng(this->pos().x(),this->pos().y());
            QString coord_str = " " + QString::number(coord.Lat(), 'f', 6) + "   " + QString::number(coord.Lng(), 'f', 6);
            text->setText(coord_str);
        }
            QGraphicsItem::mouseMoveEvent(event);
    }
    void WayPointItem::SetAltitude(const int &value)
    {
        altitude=value;
        RefreshToolTip();

        emit WPValuesChanged(this);
        this->update();
    }
    void WayPointItem::SetCoord(const internals::PointLatLng &value)
    {
        coord=value;
        emit WPValuesChanged(this);
        RefreshPos();
        RefreshToolTip();
        this->update();
    }
    void WayPointItem::SetDescription(const QString &value)
    {
        description=value;
        RefreshToolTip();
        emit WPValuesChanged(this);
        this->update();
    }
    void WayPointItem::SetNumber(const int &value)
    {
        emit WPNumberChanged(number,value,this);
        number=value;
        RefreshToolTip();
        numberI->setText(QString::number(number));
        this->update();
    }
    void WayPointItem::SetReached(const bool &value)
    {
        reached=value;
        emit WPValuesChanged(this);
        if(value)
            picture.load(QString::fromUtf8(":/markers/images/bigMarkerGreen.png"));
        else
            picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        this->update();

    }
    void WayPointItem::SetShowNumber(const bool &value)
    {
        shownumber=value;
        if((numberI==0) && value)
        {
            numberI=new QGraphicsSimpleTextItem(this);
            numberI->setPen(QPen(Qt::blue));
            numberI->setPos(0,-10-picture.height());
            numberI->setText(QString::number(number));
        }
        else if (!value && numberI)
        {
            delete numberI;
        }
        this->update();
    }
    void WayPointItem::WPDeleted(const int &onumber)
    {
        if(number>onumber) --number;
        numberI->setText(QString::number(number));
        RefreshToolTip();
        this->update();
    }
    void WayPointItem::WPInserted(const int &onumber, WayPointItem *waypoint)
    {
        if(waypoint!=this)
        {
            if(onumber<=number) ++number;
            numberI->setText(QString::number(number));
            RefreshToolTip();
            this->update();
        }
    }
    void WayPointItem::WPRenumbered(const int &oldnumber, const int &newnumber, WayPointItem *waypoint)
    {
        if (waypoint!=this)
        {
            if(((oldnumber>number) && (newnumber<=number)))
            {
                ++number;
                numberI->setText(QString::number(number));
                RefreshToolTip();
            }
            else if (((oldnumber<number) && (newnumber>number)))
            {
                --number;
                numberI->setText(number==0? "0":QString::number(number));
                RefreshToolTip();
            }
            else if (newnumber==number)
            {
                ++number;
                numberI->setText(QString::number(number));
                RefreshToolTip();
            }
            this->update();
        }
    }
    int WayPointItem::type() const
    {
        // Enable the use of qgraphicsitem_cast with this item.
        return Type;
    }

    WayPointItem::~WayPointItem()
    {
        --WayPointItem::snumber;
    }
    void WayPointItem::RefreshPos()
    {
        core::Point point=map->FromLatLngToLocal(coord);
        this->setPos(point.X(),point.Y());
    }
    void WayPointItem::RefreshToolTip()
    {
        QString coord_str = " " + QString::number(coord.Lat(), 'f', 6) + "   " + QString::number(coord.Lng(), 'f', 6);
        setToolTip(QString("WayPoint Number:%1\nDescription:%2\nCoordinate:%4\nAltitude:%5").arg(QString::number(WayPointItem::number)).arg(description).arg(coord_str).arg(QString::number(altitude)));
    }

    int WayPointItem::snumber=0;
}
