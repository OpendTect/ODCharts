#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uichartseries.h"
#include "geometry.h"

#include <QXYSeries>

using namespace QtCharts;

class i_xySeriesMsgHandler : public  QObject
{
    Q_OBJECT

    friend class uiXYChartSeries;

protected:
    i_xySeriesMsgHandler( uiXYChartSeries* series, QXYSeries* qseries )
	: series_(series)
	, qseries_(qseries)
    {
	connect( qseries_, SIGNAL(clicked(const QPointF&)),
		 this, SLOT(clicked(const QPointF&)) );
	connect( qseries_, SIGNAL(doubleClicked(const QPointF&)),
		 this, SLOT(doubleClicked(const QPointF&)) );
	connect( qseries_, SIGNAL(hovered(const QPointF&,bool)),
		 this, SLOT(hovered(const QPointF&,bool)) );
    }

private:
    uiXYChartSeries*	series_;
    QXYSeries*		qseries_;

private slots:
    void	clicked(const QPointF& p)
		{ series_->clicked.trigger( Geom::PointF(p.x(),p.y()) ); }

    void	doubleClicked(const QPointF& p)
		{ series_->doubleClicked.trigger( Geom::PointF(p.x(),p.y()) ); }

    void	hovered(const QPointF& p, bool state)
		{
		    if ( state )
			series_->hoverOn.trigger( Geom::PointF(p.x(),p.y()) );
		    else
			series_->hoverOff.trigger( Geom::PointF(p.x(),p.y()) );
		}
};
