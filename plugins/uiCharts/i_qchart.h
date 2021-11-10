#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		June 2021
________________________________________________________________________

-*/

#include "uichart.h"
#include "geometry.h"

#include <QChart>

using namespace QtCharts;

class i_chartMsgHandler : public  QObject
{
    Q_OBJECT

    friend class uiChart;

protected:
    i_chartMsgHandler( uiChart* chart, QChart* qchart )
	: chart_(chart)
	, qchart_(qchart)
    {
	connect( qchart_, SIGNAL(plotAreaChanged(const QRectF&)),
		 this, SLOT(plotAreaChanged(const QRectF&)) );
    }

private:
    uiChart*		chart_;
    QChart*		qchart_;

private slots:
    void	plotAreaChanged(const QRectF&)
		{ chart_->plotAreaChanged.trigger(); }
};
