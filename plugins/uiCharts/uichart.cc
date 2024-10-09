/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichart.h"
#include "uichartaxes.h"
#include "uichartseries.h"
#include "chartutils.h"
#include "i_qchart.h"

#include <QChart>
#include <QLegend>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    using namespace QtCharts;
#endif

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
    , needsRedraw(this)
    , odchart_(new ODChart)
{
    msghandler_ = new i_chartMsgHandler( this, odchart_ );
}


uiChart::~uiChart()
{
    delete odchart_;
    delete msghandler_;
    deepErase( axes_ );
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
    axes_ += axis;
    mAttachCB( axis->rangeChanged, uiChart::axisRangeChgCB );
}


void uiChart::addSeries( uiChartSeries* series )
{
    odchart_->addSeries( series->getQSeries() );
}


OD::Color uiChart::backgroundColor() const
{
    QColor qcol = odchart_->backgroundBrush().style()!=Qt::SolidPattern ? Qt::white
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
    const QPointF qpos = odchart_->mapToPosition( QPoint(value.x_, value.y_),
			series ? series->getQSeries() : nullptr );
    return Geom::PointF( qpos.x(), qpos.y() );
}


Geom::PointF uiChart::mapToValue( const Geom::PointF& pos,
				  uiChartSeries* series )
{
    const QPointF qpos = odchart_->mapToValue( QPoint(pos.x_, pos.y_),
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


uiChartAxis* uiChart::getAxis( OD::Orientation orient, int idx )
{
    int iax = 0;
    for ( auto* axis : axes_ )
    {
	if ( axis->orientation()==orient )
	{
	    if ( iax==idx)
		return axis;

	    iax++;
	}
    }

    return nullptr;
}


void uiChart::removeAllAxes( OD::Orientation orient, uiChartSeries* series )
{
    const Qt::Orientations qorient =
		orient==OD::Vertical ? Qt::Vertical : Qt::Horizontal;
    auto qaxes = odchart_->axes( qorient,
				 series ? series->getQSeries() : nullptr );
    for ( auto* qaxis : std::as_const(qaxes) )
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


void uiChart::setTitle( const uiString& title )
{
    QString qstr;
    title.fillQString( qstr );
    odchart_->setTitle( qstr );
}


BufferString uiChart::title() const
{
    return BufferString( odchart_->title() );
}


void uiChart::axisRangeChgCB( CallBacker* )
{
    needsRedraw.trigger();
}

QChart* uiChart::getQChart()
{
    return sCast(QChart*,odchart_);
}
