/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2020
________________________________________________________________________

-*/


#include "logseries.h"

#include "welllog.h"

LogSeries::LogSeries()
    : uiLineSeries()
{
}


LogSeries::~LogSeries()
{}


void LogSeries::setWellLog( const Well::Log& log )
{
    log_ = &log;
    updateSeries();
}


void LogSeries::updateSeries()
{
    if ( !log_ )
	return;

    clear();
    for ( int idx=0; idx<log_->size(); idx++ )
    {
	add( log_->value(idx), log_->dah(idx) );
    }
}

