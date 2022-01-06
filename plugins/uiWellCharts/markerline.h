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
#include "wellbase.h"

class uiLogChart;
class uiChartLabel;

mExpClass(uiWellCharts) MarkerLine : public WellData
{ mODTextTranslationClass(MarkerLine)
public:
			MarkerLine();
			MarkerLine(const MultiID&,const char*);
			~MarkerLine();

    void		addTo(uiLogChart&);
    void		addTo(uiLogChart&,const IOPar&);
    void		addTo(uiLogChart&,const OD::LineStyle&);
    void		removeFrom(uiLogChart&);
    void		setZType(uiWellCharts::ZType,bool force) override;

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;

    MultiID		wellID() const		{ return wellid_; }
    BufferString	markerName() const	{ return markername_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&);

protected:
    void		addMarker(uiLogChart&);
    bool		initMarker();
    const Well::Track*	wellTrack() const;
    const Well::D2TModel*	wellD2TModel() const;

    BufferString 	markername_;
    float 		dah_;
    float		zpos_;

    OD::LineStyle	linestyle_;
    uiLineSeries*	series_ = nullptr;
    uiChartLabel*	label_ = nullptr;
};
