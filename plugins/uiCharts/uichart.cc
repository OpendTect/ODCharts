/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uichart.h"

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
