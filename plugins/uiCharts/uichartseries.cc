/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uichartseries.h"

#include <QLineSeries>

using namespace QtCharts;

uiChartSeries::uiChartSeries( QAbstractSeries* series )
    : qabstractseries_(series)
{
}


uiChartSeries::~uiChartSeries()
{
}


// uiXYSeries
uiXYSeries::uiXYSeries( QXYSeries* series )
    : uiChartSeries(series)
{
    qxyseries_ = dynamic_cast<QXYSeries*>(qabstractseries_);
}


uiXYSeries::~uiXYSeries()
{
}


void uiXYSeries::add( float x, float y )
{
    qxyseries_->append( qreal(x), qreal(y) );
}


// uiLineSeries
uiLineSeries::uiLineSeries()
    : uiXYSeries(new QLineSeries)
{
    qlineseries_ = dynamic_cast<QLineSeries*>(qxyseries_);
}


uiLineSeries::~uiLineSeries()
{
}

