#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichart.h"

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
    connect( qchart_, &QChart::plotAreaChanged,
	     this, &i_chartMsgHandler::plotAreaChanged );
}


~i_chartMsgHandler()
{}

private:
    uiChart*		chart_;
    QChart*		qchart_;

private slots:
    void	plotAreaChanged(const QRectF&)
		{ chart_->plotAreaChanged.trigger(); }
};
