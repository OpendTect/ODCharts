/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicallout.h"
#include "uichartseries.h"
#include "uistring.h"

#include <QChart>
#include <QGraphicsItem>
#include <QPainter>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    using namespace QtCharts;
#endif

class ODCallout : public QGraphicsItem
{
public:
		ODCallout(QChart*);

    QRectF	boundingRect() const override;
    void	paint(QPainter*,
                      const QStyleOptionGraphicsItem*,QWidget*) override;
    void	updateGeometry();
    void	updateTextGeometry();

    QChart*	qchart_;
    QString	qtext_;
    QRectF	qtextrect_;
    QRectF	qrect_;
    QPointF	qanchor_;
    QFont	qfont_;
    bool	onright_	= true;
    qreal	offset_		= 10;
    QPen	linepen_;
    QBrush	fill_		= QColor(200,200,200,150);
};


uiCallout::uiCallout( uiChartSeries* series )
    : odcallout_(new ODCallout(series->getQSeries()->chart()))
{
}


uiCallout::~uiCallout()
{
    delete odcallout_;
}


void uiCallout::hide()
{
    odcallout_->hide();
}


void uiCallout::setText( const uiString& uistr )
{
    uistr.fillQString( odcallout_->qtext_ );
    odcallout_->updateTextGeometry();
}


void uiCallout::setAnchor( Geom::PointF pos, uiChartSeries* series )
{
    const QPointF qp( qreal(pos.x_), qreal(pos.y_) );
    const QRectF qrect = odcallout_->qchart_->plotArea();
    odcallout_->qanchor_ =
		odcallout_->qchart_->mapToPosition( qp,
				    series ? series->getQSeries() : nullptr );
    odcallout_->onright_ = qrect.center().x() >= odcallout_->qanchor_.x();
}


void uiCallout::setLine( const OD::Color& col, int width )
{
    QColor qcolor = QColor( QRgb(col.rgb()) );
    qcolor.setAlpha( 255-col.t() );
    odcallout_->linepen_ = QPen( QBrush(qcolor), width );
}


void uiCallout::setBackground( const OD::Color& col )
{
    QColor qcolor = QColor( col.rgb() );
    qcolor.setAlpha( 255-col.t() );
    odcallout_->fill_.setColor( qcolor );
}


void uiCallout::setZValue( int lvl )
{
    odcallout_->setZValue( lvl );
}


void uiCallout::show()
{
    odcallout_->show();
}


void uiCallout::update()
{
    odcallout_->updateGeometry();
}


// ODCallout
ODCallout::ODCallout( QChart* qchart )
    : QGraphicsItem(qchart)
    , qchart_(qchart)
    , linepen_(QBrush(Qt::black),1)
{}


QRectF ODCallout::boundingRect() const
{
    QPointF anchor = mapFromParent( qanchor_ );
    QRectF rect;
    rect.setLeft( qMin(qrect_.left(),anchor.x()) );
    rect.setRight( qMax(qrect_.right(),anchor.x()) );
    rect.setTop( qMin(qrect_.top(),anchor.y()) );
    rect.setBottom( qMax(qrect_.bottom(),anchor.y()) );
    return rect;
}


void ODCallout::paint( QPainter* painter,
		       const QStyleOptionGraphicsItem* option,
		       QWidget* widget )
{
    Q_UNUSED( option )
    Q_UNUSED( widget )
    QPainterPath path;
    path.addRoundedRect( qrect_, 5, 5 );
    const QPointF anchor = mapFromParent( qanchor_ ) ;

    QPointF point1, point2;
    const qreal x1 = onright_ ? anchor.x() + offset_ : anchor.x() - offset_;
    const qreal y1 = anchor.y() - offset_/2;
    point1.setX( x1 );
    point1.setY( y1 );

    const qreal x2 = onright_ ? anchor.x() + offset_ : anchor.x() - offset_;
    const qreal y2 = anchor.y() + offset_/2;
    point2.setX( x2 );
    point2.setY( y2 );

    path.moveTo( point1 );
    path.lineTo( anchor );
    path.lineTo( point2 );
    path = path.simplified();
    painter->setPen( linepen_ );
    painter->setBrush( fill_ );
    painter->drawPath(path);
    painter->drawText( qtextrect_, qtext_ );
}


void ODCallout::updateGeometry()
{
    prepareGeometryChange();
    const QPointF shift = onright_ ? QPoint(offset_,-qrect_.height()/2)
    			: QPoint(-offset_-qrect_.width(),-qrect_.height()/2);
    setPos( qanchor_ + shift );
}


void ODCallout::updateTextGeometry()
{
    QFontMetrics metrics( qfont_ );
    qtextrect_ = metrics.boundingRect( QRect(0,0,150,150), Qt::AlignLeft,
				       qtext_ );
    qtextrect_.translate( 5, 5 );
    prepareGeometryChange();
    qrect_ = qtextrect_.adjusted( -5, -5, 5, 5 );
}
