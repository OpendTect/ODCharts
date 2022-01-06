#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		September 2021
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"
#include "logdata.h"
#include "uigradientimg.h"

#include "coltabsequence.h"
#include "coltabmapper.h"
#include "separstr.h"

namespace Well { class Log; }

mExpClass(uiWellCharts) LogGradient : public LogData, public uiVertGradientImg
{
public:
			LogGradient(const MultiID&,const char* lognm=nullptr);
			LogGradient(const LogGradient&) = delete;
			~LogGradient();
    void		operator=(const LogGradient&) = delete;

    void		setColTabSequence(const ColTab::Sequence&,
					  bool update=false);
    const char*		sequenceName() const;
    void		setColTabMapperSetup(const ColTab::MapperSetup&,
					     bool update=false);

    void		setZType(uiWellCharts::ZType,bool force) override;
    void		setDisplayRange(const Interval<float>& range) override;
    FileMultiString	toString() const override;
    void		fromString(const FileMultiString&) override;

    void		updateImg(const StepInterval<float>&) override;

protected:
    void		update();

    ColTab::Sequence	ctseq_;
    ColTab::Mapper	ctmapper_;
    StepInterval<float>	imgzrange_;
};
