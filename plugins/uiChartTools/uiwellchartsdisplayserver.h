#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "dgbcommon.h"
#include "uiwelldisplayserver.h"
#include "uiwelllogtoolsgrp.h"

class uiLogViewTable;

mExpClass(uiChartTools) uiWellChartsDisplayServer : public uiWellDisplayServer
{
mdGBTextTranslationClass(uiWellChartsDisplayServer)
public:
			uiWellChartsDisplayServer();
			~uiWellChartsDisplayServer();

    uiDialog*		createMultiWellDisplay(uiParent*, const DBKeySet&,
					    const BufferStringSet&) override;
    uiWellLogToolWinGrp*	createWellLogToolGrp(uiParent*,
			const ObjectSet<uiWellLogToolWin::LogData>&) override;
};


mExpClass(uiWellChartsTools) uiWellChartsLogToolWinGrp
					: public uiWellLogToolWinGrp
{
mdGBTextTranslationClass(uiWellChartsDisplayServer)
public:
			uiWellChartsLogToolWinGrp(uiParent*,
				const ObjectSet<uiWellLogToolWin::LogData>&);
			~uiWellChartsLogToolWinGrp();

    void		displayLogs() override;

protected:
    uiLogViewTable*	logviewtbl_;
};
