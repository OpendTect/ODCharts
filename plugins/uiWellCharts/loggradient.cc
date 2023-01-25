/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "loggradient.h"
#include "uigradientimg.h"

#include "chartutils.h"
#include "logdata.h"
#include "welllog.h"

#include <QImage>

LogGradient::LogGradient( const MultiID& wellid, const char* lognm )
    : LogData(wellid,lognm)
{
    ctmapper_.setup_.range_ = disprange_;
}


LogGradient::~LogGradient()
{
}


void LogGradient::setColTabSequence( const ColTab::Sequence& ctseq,
				     bool updateimg )
{
    ctseq_ = ctseq;

    if ( updateimg )
	update();
}


const char* LogGradient::sequenceName() const
{
    return ctseq_.name();
}


void LogGradient::setColTabMapperSetup( const ColTab::MapperSetup& su,
					bool updateimg )
{
    ctmapper_.setup_ = su;
    ctmapper_.setup_.type_ = ColTab::MapperSetup::Fixed;
    LogData::setDisplayRange( su.range_ );

    if ( updateimg )
	update();
}


void LogGradient::updateImg( const StepInterval<float>& zrange )
{
    const int height = zrange.nrSteps() + 1;
    if ( height==qimg_->height() && zrange.isEqual(imgzrange_,mDefEpsF) )
	return;

    if ( height!=qimg_->height() )
    {
	deleteAndNullPtr( qimg_ );
	qimg_ = new QImage( 1, height, QImage::Format_ARGB32_Premultiplied );
    }

    imgzrange_ = zrange;
    imgzrange_.sort();
    update();
}


void LogGradient::setZType( uiWellCharts::ZType ztype, bool force )
{
    if ( !force && zType()==ztype )
	return;

    LogData::setZType( ztype, force );
    update();
}


void LogGradient::setDisplayRange( const Interval<float>& range )
{
    LogData::setDisplayRange( range );
    ctmapper_.setup_.range_ = disprange_;
}


FileMultiString LogGradient::toString() const
{
    FileMultiString fms;
    fms += FileMultiString( logName() );
    fms += FileMultiString( ctseq_.name() );
    fms += ctmapper_.setup_.range_.start;
    fms += ctmapper_.setup_.range_.stop;
    return fms;
}


void LogGradient::fromString( const FileMultiString& str )
{
    logname_ = str[0];
    initLog();
    ctseq_ = ColTab::Sequence( str[1] );
    LogData::setDisplayRange( str.getFValue(2), str.getFValue(3) );
    ctmapper_.setup_.type_ = ColTab::MapperSetup::Fixed;
    ctmapper_.setup_.range_ = disprange_;
}


void LogGradient::update()
{
    ConstRefMan<Well::Data> wd = getWD();
    const Well::Log* log = wd ? wd->getLog( logname_ ) : nullptr;
    if ( !qimg_ || !log || imgzrange_==StepInterval<float>() )
	return;

    QColor qcol;
    for ( int idx=0; idx<=imgzrange_.nrSteps(); idx++ )
    {
	const float dah = zToDah( imgzrange_.atIndex(idx), ztype_ );
	const float logval = log->getValue( dah );
	if ( mIsUdf(logval) )
	    qcol = QColorConstants::Transparent;
	else
	{
	    OD::Color color = ctseq_.color( ctmapper_.position(logval) );
	    toQColor( qcol, color, true );
	}
	qimg_->setPixelColor( 0, idx, qcol );
    }
}
