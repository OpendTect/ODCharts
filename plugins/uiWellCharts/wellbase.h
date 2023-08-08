#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
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

    MultiID			wellID() const	 { return wd_->multiID(); }
    BufferString		wellName() const { return wd_->name(); }
    uiWellCharts::ZType		zType() const	 { return ztype_; }

    virtual void		setZType(uiWellCharts::ZType,bool);
    float			zToDah(float,uiWellCharts::ZType) const;
    float			dahToZ(float,uiWellCharts::ZType) const;
    Interval<float>		dahToZ(const Interval<float>&,
				       uiWellCharts::ZType) const;

    void			fillPar(IOPar&) const;
    void			usePar(const IOPar&);
    virtual ConstRefMan<Well::Data>	getWD() const;

protected:
    bool			initWell(const MultiID&);
    bool			initWell(const char*);
    void			copyFrom(const WellData&);

    uiWellCharts::ZType		ztype_;
    mutable ConstRefMan<Well::Data>	wd_;
};
