/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/

#include "uichartfillx.h"
#include "chartutils.h"
#include "uichart.h"
#include "uichartseries.h"
#include "uistring.h"

#include <QChart>
#include <QGraphicsItem>
#include <QLineSeries>
#include <QLogValueAxis>
#include <QPainter>
#include <QValueAxis>

using namespace QtCharts;

mDefineEnumUtils( uiChartFillx, FillDir, "Fill direction" )
    { "Left", "Right", nullptr };

mDefineEnumUtils( uiChartFillx, FillType, "Fill type" )
    { "None", "Color", "Gradient", "Pattern", nullptr };


class ODChartFillx : public QGraphicsItem
{
public:
		ODChartFillx();
		ODChartFillx(QChart*);
		~ODChartFillx();

    QRectF	boundingRect() const;
    void	paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);
    void	updateGeometry();
    QPainterPath		makePath(bool qlines=true);
    Interval<float>		getAxisRange(bool qlines=true);

    QChart*			qchart_;
    ObjectSet<QLineSeries>	qlines_;
    ObjectSet<QLineSeries>	qbaselines_;
    float			baseline_ = mUdf(float);
    QColor			qcolor_;
    QRectF			qrect_;
    QPainterPath		qpath_;
    uiChartFillx::FillDir	filldir_;
    uiChartFillx::FillType	filltype_;
};


uiChartFillx::uiChartFillx()
    : odfillx_(new ODChartFillx)
{
    setFillType( None, false );
    setFillDir( Left, false );
    setColor( OD::Color::NoColor(), false );
    setBaseLine( mUdf(float), false );
}


uiChartFillx::~uiChartFillx()
{
    detachAllNotifiers();
    delete odfillx_;
}


void uiChartFillx::addTo( uiChart* chart,
			  const ObjectSet<uiLineSeries>& lines,
			  FillDir fdir )
{
    odfillx_->qchart_ = chart->getQChart();
    odfillx_->setParentItem( odfillx_->qchart_ );
    for ( auto* series : lines )
	odfillx_->qlines_ += series->getQLineSeries();

    setFillDir( fdir, false );
    setBaseLine( mUdf(float), true );
    mAttachCB( chart->plotAreaChanged, uiChartFillx::updateCB );
    mAttachCB( chart->needsRedraw, uiChartFillx::updateCB );
}


void uiChartFillx::addTo( uiChart* chart, const ObjectSet<uiLineSeries>& lines,
			  OD::Color col, FillDir fdir )
{
    addTo( chart, lines, fdir );
    setFillType( Color, false );
    setColor( col );
}


void uiChartFillx::setFillDir( uiChartFillx::FillDir fdir, bool update )
{
    odfillx_->filldir_ = fdir;
    if ( update )
	updateCB( nullptr );
}


uiChartFillx::FillDir uiChartFillx::fillDir() const
{
    return odfillx_->filldir_;
}


void uiChartFillx::setFillType( FillType ftype, bool update )
{
    odfillx_->filltype_ = ftype;
    if ( update )
	updateCB( nullptr );
}


uiChartFillx::FillType uiChartFillx::fillType() const
{
    return odfillx_->filltype_;
}


void uiChartFillx::setColor( OD::Color col, bool update )
{
    toQColor( odfillx_->qcolor_, col );
    if ( update )
	updateCB( nullptr );
}


OD::Color uiChartFillx::color() const
{
    return fromQColor( odfillx_->qcolor_ );
}


void uiChartFillx::setBaseLine( float base, bool update )
{
    odfillx_->baseline_ = base;
    odfillx_->qbaselines_.setEmpty();
    if ( update )
	updateCB( nullptr );
}


float uiChartFillx::baseLineValue() const
{
    return odfillx_->baseline_;
}


void uiChartFillx::setBaseLine( const ObjectSet<uiLineSeries>& lines,
				bool update )
{
    if ( !lines.isEmpty() )
    {
	odfillx_->baseline_ = mUdf(float);
	odfillx_->qbaselines_.setEmpty();
	for ( auto* series : lines )
	    odfillx_->qbaselines_ += series->getQLineSeries();
    }
    if ( update )
	updateCB( nullptr );
}


bool uiChartFillx::hasBaseLineSeries() const
{
    return !odfillx_->qbaselines_.isEmpty();
}


void uiChartFillx::updateCB( CallBacker* )
{
    odfillx_->updateGeometry();
}


// ODChartFillx
ODChartFillx::ODChartFillx()
    : QGraphicsItem()
{
}


ODChartFillx::~ODChartFillx()
{}


QRectF ODChartFillx::boundingRect() const
{
    return qrect_;
}


void ODChartFillx::paint( QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget )
{
    Q_UNUSED( option )
    Q_UNUSED( widget )
    painter->save();
    painter->setPen( QPen(Qt::NoPen) );
    if ( filltype_==uiChartFillx::Color && qcolor_.isValid() )
	painter->setBrush( qcolor_ );

    painter->setClipRect( mapRectFromParent(qchart_->plotArea()) );
    painter->drawPath( qpath_ );

    painter->restore();
}


Interval<float> ODChartFillx::getAxisRange( bool qlines )
{
    Interval<float> res;

    ObjectSet<QLineSeries>& series = qlines ? qlines_ : qbaselines_;
    QAbstractAxis* axis = qchart_->axes( Qt::Horizontal, series.first() )[0];
    auto* qvaxis = dynamic_cast<QValueAxis*>( axis );
    auto* qlvaxis = dynamic_cast<QLogValueAxis*>( axis );
    if ( qvaxis )
	qvaxis->isReverse() ? res.set( qvaxis->max(), qvaxis->min() )
			    : res.set( qvaxis->min(), qvaxis->max() );
    if ( qlvaxis )
	qlvaxis->isReverse() ? res.set( qlvaxis->max(), qlvaxis->min() )
			     : res.set( qlvaxis->min(), qlvaxis->max() );

     return res;
}


QPainterPath ODChartFillx::makePath( bool qlines )
{
    QPainterPath path;
    ObjectSet<QLineSeries>& series = qlines ? qlines_ : qbaselines_;
    for ( auto* part : series )
    {
	path.moveTo( qchart_->mapToPosition(part->at(0), part) );
	for ( int idx=0; idx<part->count(); idx++ )
	    path.lineTo( qchart_->mapToPosition(part->at(idx), part) );

	QPointF p1 = part->at( part->count()-1 );
	QPointF p2 = part->at( 0 );

	Interval<float> rng = getAxisRange( qlines );
	if ( filldir_==uiChartFillx::Left )
	    p1.rx() = qlines ? rng.start : rng.stop;
	else
	    p1.rx() = qlines ? rng.stop : rng.start;

	p2.rx() = p1.x();
	path.lineTo( qchart_->mapToPosition(p1, part) );
	path.lineTo( qchart_->mapToPosition(p2, part) );
	path.closeSubpath();
    }
    return path;
}


void ODChartFillx::updateGeometry()
{
    QPainterPath path;

    if ( filltype_==uiChartFillx::Color && qcolor_.isValid() )
    {
	path = makePath( true );

	Interval<float> rng = getAxisRange( true );
	if ( !mIsUdf(baseline_) && rng.includes(baseline_, true) )
	{
	    QLineSeries* first = qlines_.first();
	    QLineSeries* last = qlines_.last();
	    QPointF br1 = first->at( 0 );
	    br1.rx() = baseline_;
	    QPointF br2 = last->at( last->count()-1 );
	    br2.rx() = baseline_;
	    QPointF br3 = br2;
	    QPointF br4 = br1;

	    if ( filldir_==uiChartFillx::Left )
		br3.rx() = rng.stop;
	    else
		br3.rx() = rng.start;
	    br4.rx() = br3.x();
	    QPainterPath baserect( qchart_->mapToPosition(br1, first) );
	    baserect.lineTo( qchart_->mapToPosition(br2, first) );
	    baserect.lineTo( qchart_->mapToPosition(br3, first) );
	    baserect.lineTo( qchart_->mapToPosition(br4, first) );
	    baserect.closeSubpath();
	    path &= baserect;
	}
	else if ( !qbaselines_.isEmpty() )
	{
	    QPainterPath other = makePath( false );
	    path &= other;
	}
    }

    if ( path.boundingRect().height() <= INT_MAX &&
	 path.boundingRect().width() <= INT_MAX)
    {
        prepareGeometryChange();
        qpath_ = path;
        qrect_ = path.boundingRect();
        update();
    }
}


