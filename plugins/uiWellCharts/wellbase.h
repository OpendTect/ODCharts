#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		December 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "multiid.h"
#include "ranges.h"
#include "wellchartcommon.h"

namespace Well { class Track; class D2TModel; }

mExpClass(uiWellCharts) WellData
{
public:
				WellData();
				WellData(const MultiID&);
				WellData(const WellData&) = delete;
    virtual			~WellData();

    void			operator=(const WellData&) = delete;

    MultiID			wellID() const		{ return wellid_; }
    BufferString		wellName() const	{ return wellname_; }
    uiWellCharts::ZType		zType() const		{ return ztype_; }
    const Well::Track*		wellTrack() const;
    const Well::D2TModel*	wellD2TModel() const;

    virtual void		setZType(uiWellCharts::ZType,bool);
    float			zToDah(float, uiWellCharts::ZType);
    float			dahToZ(float, uiWellCharts::ZType);
    Interval<float>		dahToZ(const Interval<float>&,
				       uiWellCharts::ZType);

    void			fillPar(IOPar&) const;
    void			usePar(const IOPar&);

protected:
    bool			initWell();
    bool			initWell(const char*);
    void			copyFrom(const WellData&);


    uiWellCharts::ZType		ztype_;
    MultiID			wellid_;
    BufferString 		wellname_;
};
