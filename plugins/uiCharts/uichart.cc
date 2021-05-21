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
    odchart_->removeAxis( axis->getQAxis() );
}


void uiChart::removeSeries( uiChartSeries* series )
{
    odchart_->removeSeries( series->getQSeries() );
}


void uiChart::setTitle( const char* title )
{
    odchart_->setTitle( title );
}


BufferString uiChart::title() const
{
    return BufferString( odchart_->title() );
}
