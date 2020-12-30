/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uichartseries.h"

#include <QLineSeries>
#include <QScatterSeries>

using namespace QtCharts;

uiChartSeries::uiChartSeries( QAbstractSeries* series )
    : qabstractseries_(series)
{
}


uiChartSeries::~uiChartSeries()
{
}


void uiChartSeries::setName( const char* nm )
{
    qabstractseries_->setName( nm );
}


BufferString uiChartSeries::getName() const
{
    BufferString str( qabstractseries_->name() );
    return str;
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


void uiXYSeries::clear()
{
    qxyseries_->clear();
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


// uiScatterSeries
uiScatterSeries::uiScatterSeries()
    : uiXYSeries(new QScatterSeries)
{
    qscatterseries_ = dynamic_cast<QScatterSeries*>(qxyseries_);
}


uiScatterSeries::~uiScatterSeries()
{
}
