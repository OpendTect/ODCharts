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
#include "chartutils.h"
#include "i_qchart.h"

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

int numAxes( OD::Orientation orient ) const
{
    const Qt::Orientations qorient =
			orient==OD::Vertical ? Qt::Vertical : Qt::Horizontal;
    return axes( qorient ).size();
}
};


uiChart::uiChart()
    : plotAreaChanged(this)
    , axisRangeChanged(this)
    , odchart_(new ODChart)
{
    msghandler_ = new i_chartMsgHandler( this, odchart_ );
}


uiChart::~uiChart()
{
    delete odchart_;
    delete msghandler_;
    detachAllNotifiers();
}


void uiChart::addAxis( uiChartAxis* axis, OD::Edge all )
{
    Qt::Alignment qall =
	all==OD::Top ? Qt::AlignTop
		     : all==OD::Bottom ? Qt::AlignBottom
				       : all==OD::Left ? Qt::AlignLeft
						       : Qt::AlignRight;
    odchart_->addAxis( axis->getQAxis(), qall );
    mAttachCB( axis->rangeChanged, uiChart::axisRangeChgCB );
}


void uiChart::addSeries( uiChartSeries* series )
{
    odchart_->addSeries( series->getQSeries() );
}


OD::Color uiChart::backgroundColor() const
{
    QColor qcol = odchart_->backgroundBrush()!=Qt::SolidPattern ? Qt::white
					 : odchart_->backgroundBrush().color();
    return fromQColor( qcol );
}


void uiChart::displayLegend( bool yn )
{
    yn ? odchart_->legend()->show() : odchart_->legend()->hide();
}


Geom::RectI uiChart::margins() const
{
    const QMargins qm = odchart_->margins();
    return Geom::RectI( qm.left(), qm.top(), qm.right(), qm.bottom() );
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


int uiChart::numAxes( OD::Orientation orient ) const
{
    return odchart_->numAxes( orient );
}


Geom::RectF uiChart::plotArea() const
{
    const QRectF qrect = odchart_->plotArea();
    return Geom::RectF( qrect.left(), qrect.top(), qrect.right(),
			qrect.bottom() );
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
    if ( !axis )
	return;

    odchart_->removeAxis( axis->getQAxis() );
    mDetachCB( axis->rangeChanged, uiChart::axisRangeChgCB );

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


void uiChart::setBackgroundColor( const OD::Color& col )
{
    if ( col==OD::Color::NoColor() )
	odchart_->setBackgroundBrush( QBrush(Qt::NoBrush) );
    else
    {
	QColor qcol;
	toQColor( qcol, col );
	odchart_->setBackgroundBrush( QBrush(qcol) );
    }
}


void uiChart::setPlotArea( const Geom::RectF& rect )
{
    const QRectF qrect( rect.left(), rect.top(), rect.right(), rect.bottom() );
    odchart_->setPlotArea( qrect );
}


void uiChart::setMargins( int left, int top, int right, int bottom )
{
    odchart_->setMargins( QMargins(left,top,right,bottom) );
}


void uiChart::setTitle( const char* title )
{
    odchart_->setTitle( title );
}


BufferString uiChart::title() const
{
    return BufferString( odchart_->title() );
}


void uiChart::axisRangeChgCB( CallBacker* )
{
    axisRangeChanged.trigger();
}
