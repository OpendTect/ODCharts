#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartaxes.h"

#include <QValueAxis>

#if QT_VERSION < x060000
    using namespace QtCharts;
#endif

class i_valueAxisMsgHandler : public  QObject
{
Q_OBJECT
friend class uiValueAxis;

protected:
i_valueAxisMsgHandler( uiValueAxis* axis, QValueAxis* qaxis )
    : axis_(axis)
    , qaxis_(qaxis)
{
   connect( qaxis_, &QValueAxis::rangeChanged,
	    this, &i_valueAxisMsgHandler::snapRange );
}


~i_valueAxisMsgHandler()
{}

private:
    uiValueAxis*	axis_;
    QValueAxis*		qaxis_;

private slots:
void snapRange( qreal min, qreal max )
{
    axis_->snapRange( min, max );
    Interval<float> rg( axis_->range() );
    rg.sort();
    axis_->rangeChanged.trigger( rg );
}

};
