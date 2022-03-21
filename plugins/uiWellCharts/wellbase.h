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
#include "ptrman.h"
#include "ranges.h"
#include "wellchartcommon.h"
#include "welldata.h"

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

    virtual void		setZType(uiWellCharts::ZType,bool);
    float			zToDah(float,uiWellCharts::ZType) const;
    float			dahToZ(float,uiWellCharts::ZType) const;
    Interval<float>		dahToZ(const Interval<float>&,
				       uiWellCharts::ZType) const;

    void			fillPar(IOPar&) const;
    void			usePar(const IOPar&);

protected:
    bool			initWell();
    bool			initWell(const char*);
    void			copyFrom(const WellData&);
    virtual ConstRefMan<Well::Data>	getWD() const;


    uiWellCharts::ZType		ztype_;
    MultiID			wellid_;
    BufferString 		wellname_;
};
