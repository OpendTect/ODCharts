#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartseries.h"
#include "geometry.h"

#include <QXYSeries>

//! Helper class for uiAction to QXYSeries Qt's messages.

QT_BEGIN_NAMESPACE

class i_xySeriesMsgHandler : public  QObject
{
Q_OBJECT
friend class uiXYChartSeries;

protected:
i_xySeriesMsgHandler( uiXYChartSeries* series, QXYSeries* qseries )
    : series_(series)
    , qseries_(qseries)
{
    connect( qseries_, &QXYSeries::pressed,
	     this, &i_xySeriesMsgHandler::pressed );
    connect( qseries_, &QXYSeries::released,
	     this, &i_xySeriesMsgHandler::released );
    connect( qseries_, &QXYSeries::clicked,
	     this, &i_xySeriesMsgHandler::clicked );
    connect( qseries_, &QXYSeries::doubleClicked,
	     this, &i_xySeriesMsgHandler::doubleClicked );
    connect( qseries_, &QXYSeries::hovered,
	     this, &i_xySeriesMsgHandler::hovered );
}


~i_xySeriesMsgHandler()
{}

private:

    uiXYChartSeries*	series_;
    QXYSeries*		qseries_;

private slots:

void pressed( const QPointF& p )
{
    series_->pressed.trigger( Geom::PointF(p.x(),p.y()) );
}

void released( const QPointF& p )
{
    series_->released.trigger( Geom::PointF(p.x(),p.y()) );
}

void clicked( const QPointF& p )
{
    series_->clicked.trigger( Geom::PointF(p.x(),p.y()) );
}

void doubleClicked( const QPointF& p )
{
    series_->doubleClicked.trigger( Geom::PointF(p.x(),p.y()) );
}

void hovered( const QPointF& p, bool state )
{
    if ( state )
	series_->hoverOn.trigger( Geom::PointF(p.x(),p.y()) );
    else
	series_->hoverOff.trigger( Geom::PointF(p.x(),p.y()) );
}

};

QT_END_NAMESPACE
