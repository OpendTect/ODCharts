/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		November 2020
________________________________________________________________________

-*/

#include "uichart.h"
#include "uichartaxes.h"
#include "uichartseries.h"

#include <QChart>
#include <QLegend>

using namespace QtCharts;

class ODChart : public QChart
{
public:
ODChart()
    : QChart()
{}


~ODChart()
{}

};


uiChart::uiChart()
    : odchart_(new ODChart)
{
}


uiChart::~uiChart()
{
    delete odchart_;
}


void uiChart::addAxis( uiChartAxis* axis, OD::Edge all )
{
    Qt::Alignment qall =
	all==OD::Top ? Qt::AlignTop
		     : all==OD::Bottom ? Qt::AlignBottom
				       : all==OD::Left ? Qt::AlignLeft
						       : Qt::AlignRight;
    odchart_->addAxis( axis->getQAxis(), qall );
}


void uiChart::addSeries( uiChartSeries* series )
{
    odchart_->addSeries( series->getQSeries() );
}


void uiChart::displayLegend( bool yn )
{
    yn ? odchart_->legend()->show() : odchart_->legend()->hide();
}


Geom::PointF uiChart::mapToPosition( const Geom::PointF& value,
				     uiChartSeries* series )
{
    const QPointF qpos = odchart_->mapToPosition( QPoint(value.x, value.y),
			series ? series->getQSeries() : nullptr );
    return Geom::PointF( qpos.x(), qpos.y() );
}


Geom::PointF uiChart::mapToValue( const Geom::PointF& pos,
				  uiChartSeries* series )
{
    const QPointF qpos = odchart_->mapToValue( QPoint(pos.x, pos.y),
			series ? series->getQSeries() : nullptr );
    return Geom::PointF( qpos.x(), qpos.y() );
}


void uiChart::removeAllAxes( OD::Orientation orient, uiChartSeries* series )
{
    const Qt::Orientations qorient =
		orient==OD::Vertical ? Qt::Vertical : Qt::Horizontal;
    auto qaxes = odchart_->axes( qorient,
				 series ? series->getQSeries() : nullptr );
    for ( auto* qaxis : qaxes )
	odchart_->removeAxis( qaxis );
}


void uiChart::removeAllSeries()
{
    odchart_->removeAllSeries();
}


void uiChart::removeAxis( uiChartAxis* axis )
{
    if ( axis )
	odchart_->removeAxis( axis->getQAxis() );
}


void uiChart::removeSeries( uiChartSeries* series )
{
    if ( series )
	odchart_->removeSeries( series->getQSeries() );
}


void uiChart::setAcceptHoverEvents( bool yn )
{
    odchart_->setAcceptHoverEvents( yn );
}


void uiChart::setTitle( const char* title )
{
    odchart_->setTitle( title );
}


BufferString uiChart::title() const
{
    return BufferString( odchart_->title() );
}
