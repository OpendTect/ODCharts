#pragma once
/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		May 2021
________________________________________________________________________

-*/

#include "uichartaxes.h"

#include <QValueAxis>

using namespace QtCharts;

class i_valueAxisMsgHandler : public  QObject
{
    Q_OBJECT

    friend class uiValueAxis;

protected:
			i_valueAxisMsgHandler( uiValueAxis* axis,
					       QValueAxis* qaxis )
			    : axis_(axis)
			    , qaxis_(qaxis)
			{
			   connect( qaxis_, SIGNAL(rangeChanged(qreal,qreal)),
				    this, SLOT(snapRange(qreal,qreal)) );
			}

private:
    uiValueAxis*	axis_;
    QValueAxis*		qaxis_;

private slots:
    void		snapRange( qreal min, qreal max )
			{
			    axis_->snapRange( min, max );
			    Interval<float> rg( axis_->range() );
			    rg.sort();
			    axis_->rangeChanged.trigger( rg );
			}
};
