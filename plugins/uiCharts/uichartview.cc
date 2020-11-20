/*+
 * (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 * AUTHOR   : Raman Singh
 * DATE     : June 2008
-*/

#include "uichartview.h"
#include "uichart.h"
#include "uiobjbodyimpl.h"

#include <QChart>
#include <QChartView>

using namespace QtCharts;

class ODChartView : public uiObjBodyImpl<uiChartView,QChartView>
{
public:

ODChartView( uiChartView& hndle, uiParent* p, const char* nm )
    : uiObjBodyImpl<uiChartView,QChartView>(hndle,p,nm)
{
}


~ODChartView()
{
}

};


uiChartView::uiChartView( uiParent* p, const char* nm )
    : uiObject(p,nm,mkbody(p,nm))
{
    delete odchartview_->chart();
    uichart_ = new uiChart();
    odchartview_->setChart( uichart_->getQChart() );
}


uiChartView::~uiChartView()
{
    odchartview_->setChart( nullptr );
    delete uichart_;
}


ODChartView& uiChartView::mkbody( uiParent* p, const char* nm )
{
    odchartview_ = new ODChartView( *this, p, nm );
    return *odchartview_;
}

