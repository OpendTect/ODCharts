#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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
    virtual		~MarkerLine();

    void		addTo(uiLogChart&,bool show_wellnm=true);
    void		addTo(uiLogChart&,const IOPar&);
    void		addTo(uiLogChart&,const OD::LineStyle&,
			      bool show_wellnm=true);
    void		removeFrom(uiLogChart&);
    void		setZType(uiWellCharts::ZType,bool force) override;

    float		getZ() const		{ return zpos_; }

    void		setLineStyle(const OD::LineStyle&,bool usetransp=false);
    OD::LineStyle	lineStyle() const;

    BufferString	markerName() const	{ return markername_; }

    void		fillPar(IOPar&) const;
    void		usePar(const IOPar&, bool styleonly=false);

protected:
    void		addMarker(uiLogChart&,bool show_wellnm);
    bool		initMarker();

    BufferString 	markername_;
    float 		dah_;
    float		zpos_;
    bool		valid_;

    OD::LineStyle	linestyle_;
    uiLineSeries*	series_ = nullptr;
    uiChartLabel*	label_ = nullptr;
};
