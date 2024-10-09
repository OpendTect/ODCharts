/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uichartlabel.h"
#include "uichart.h"
#include "uichartseries.h"
#include "uistring.h"

#include <QChart>
#include <QGraphicsItem>
#include <QPainter>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    using namespace QtCharts;
#endif

class ODChartLabel : public QGraphicsItem
{
public:
		ODChartLabel(QChart*);

    QRectF	boundingRect() const override;
    void	paint(QPainter*,
                      const QStyleOptionGraphicsItem*,QWidget*) override;
    void	updateGeometry();
    void	updateTextGeometry();

    QChart*		qchart_;
    QAbstractSeries*	qseries_ = nullptr;
    QString		qtext_;
    QRectF		qtextrect_;
    QRectF		qrect_;
    QPointF		qanchor_;
    QFont		qfont_;
    Qt::Alignment	align_;
    QPen		linepen_;
    QBrush		fill_		= QColor(200,200,200,150);
};


uiChartLabel::uiChartLabel( uiChart* chart, const uiString& uistr,
			    uiChartSeries* series )
    : odlabel_(new ODChartLabel(chart->getQChart()))
{
    setText( uistr );
    odlabel_->qseries_ = series ? series->getQSeries() : nullptr;

    mAttachCB( chart->plotAreaChanged, uiChartLabel::updateCB );
    mAttachCB( chart->needsRedraw, uiChartLabel::updateCB );
}


uiChartLabel::~uiChartLabel()
{
    detachAllNotifiers();
    delete odlabel_;
}


void uiChartLabel::show( bool yn )
{
    yn ? odlabel_->show() : odlabel_->hide();
}


void uiChartLabel::setText( const uiString& uistr )
{
    uistr.fillQString( odlabel_->qtext_ );
    odlabel_->updateTextGeometry();
}


void uiChartLabel::setAnchor( Geom::PointF pos, Alignment all )
{
    const QPointF qp( qreal(pos.x_), qreal(pos.y_) );
    odlabel_->qanchor_ = qp;
    odlabel_->align_ = sCast(Qt::AlignmentFlag,all.uiValue());
}


void uiChartLabel::updateCB( CallBacker* )
{
    odlabel_->updateGeometry();
}


// ODCChartLabel
ODChartLabel::ODChartLabel( QChart* qchart )
    : QGraphicsItem(qchart)
    , qchart_(qchart)
    , linepen_(QBrush(Qt::black),1)
{
    setZValue( 10 );
}


QRectF ODChartLabel::boundingRect() const
{
    const QPointF qp = qchart_->mapToPosition( qanchor_, qseries_ );
    QPointF anchor = mapFromParent( qp );
    QRectF rect;
    rect.setLeft( qMin(qrect_.left(),anchor.x()) );
    rect.setRight( qMax(qrect_.right(),anchor.x()) );
    rect.setTop( qMin(qrect_.top(),anchor.y()) );
    rect.setBottom( qMax(qrect_.bottom(),anchor.y()) );
    return rect;
}


void ODChartLabel::paint( QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget )
{
    Q_UNUSED( option )
    Q_UNUSED( widget )
    painter->setClipRect( mapRectFromParent(qchart_->plotArea()) );
    painter->drawText( qtextrect_, qtext_ );
}


void ODChartLabel::updateGeometry()
{
    prepareGeometryChange();
    const float shiftX = align_ & Qt::AlignLeft ? 0.f :
			 align_ & Qt::AlignHCenter ? -qrect_.width()/2 :
						     -qrect_.width();
    const float shiftY = align_ & Qt::AlignTop ? 0.f :
			 align_ & Qt::AlignVCenter ? -qrect_.height()/2 :
						     -qrect_.height();
    const QPointF qp = qchart_->mapToPosition( qanchor_, qseries_ );

     setPos( qp + QPointF(shiftX,shiftY) );
}


void ODChartLabel::updateTextGeometry()
{
    QFontMetrics metrics( qfont_ );
    qtextrect_ = metrics.boundingRect( QRect(0,0,150,150), Qt::AlignLeft,
				       qtext_ );
    qtextrect_.translate( 1, 1 );
    prepareGeometryChange();
    qrect_ = qtextrect_.adjusted( -1, -1, 1, 1 );
}
