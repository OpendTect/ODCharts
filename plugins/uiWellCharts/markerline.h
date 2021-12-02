#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "draw.h"
#include "multiid.h"
#include "uichartseries.h"
#include "uistring.h"

class uiLogChart;
class uiChartLabel;

mExpClass(uiWellCharts) MarkerLine
{ mODTextTranslationClass(MarkerLine)
public:
			MarkerLine();
			MarkerLine(const MultiID&,const char*);
			~MarkerLine();

    void		addTo(uiLogChart&);
    void		addTo(uiLogChart&,const IOPar&);
    void		addTo(uiLogChart&,const OD::LineStyle&);
    void		removeFrom(uiLogChart&);

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;

    MultiID		wellID() const		{ return wellid_; }
    BufferString	markerName() const	{ return markername_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

protected:
    void		addMarker(uiLogChart&);
    bool		initMarker();

    MultiID		wellid_;
    BufferString	wellname_;
    BufferString 	markername_;
    float 		dah_;
    OD::LineStyle	linestyle_;
    uiLineSeries*	series_ = nullptr;
    uiChartLabel*	label_ = nullptr;
};
