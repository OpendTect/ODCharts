/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uichart.h"
#include "uichartseries.h"

#include <QChart>

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


void uiChart::addSeries( uiChartSeries& series )
{
    odchart_->addSeries( series.getSeries() );
    odchart_->createDefaultAxes();
    // TODO: make separate function for reverting axes
    odchart_->axes(Qt::Vertical).first()->setReverse();
}
