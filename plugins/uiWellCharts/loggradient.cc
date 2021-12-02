/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
________________________________________________________________________

-*/


#include "loggradient.h"
#include "uigradientimg.h"

#include "chartutils.h"
#include "welllog.h"

#include <QImage>

LogGradient::LogGradient( const MultiID& wellid, const char* lognm )
{
    wellid_ = wellid;
    const FixedString lognmstr = lognm;
    if ( !lognmstr.isEmpty() )
    {
	logname_ = lognm;
	initLog();
	ctmapper_.setup_.range_ = disprange_;
    }
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
    if ( height==qimg_->height() && zrange.isEqual(zrange_,mDefEpsF) )
	return;

    if ( height!=qimg_->height() )
    {
	deleteAndZeroPtr( qimg_ );
	qimg_ = new QImage( 1, height, QImage::Format_ARGB32_Premultiplied );
    }

    zrange_ = zrange;
    zrange_.sort();
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
    const Well::Log* log = wellLog();
    if ( !qimg_ || !log || zrange_==StepInterval<float>() )
	return;

    QColor qcol;
    for ( int idx=0; idx<=zrange_.nrSteps(); idx++ )
    {
	const float dah = zrange_.atIndex( idx );
	const float logval = log->getValue( dah );
	if ( mIsUdf(logval) )
	    qcol = QColorConstants::Transparent;
	else
	{
	    Color color = ctseq_.color( ctmapper_.position(logval) );
	    toQColor( qcol, color, true );
	}
	qimg_->setPixelColor( 0, idx, qcol );
    }
}
