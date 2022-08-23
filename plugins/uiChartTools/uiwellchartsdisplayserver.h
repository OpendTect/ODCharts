#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uicharttoolsmod.h"

#include "uiwelldisplayserver.h"
#include "uiwelllogtoolsgrp.h"

class uiLogViewTable;

mExpClass(uiChartTools) uiWellChartsDisplayServer : public uiWellDisplayServer
{
mODTextTranslationClass(uiWellChartsDisplayServer)
public:
			uiWellChartsDisplayServer();
			~uiWellChartsDisplayServer();

    uiDialog*		createMultiWellDisplay(uiParent*, const DBKeySet&,
					    const BufferStringSet&) override;
    uiWellLogToolWinGrp*	createWellLogToolGrp(uiParent*,
			const ObjectSet<uiWellLogToolWin::LogData>&) override;
};


mExpClass(uiChartTools) uiWellChartsLogToolWinGrp
					: public uiWellLogToolWinGrp
{
mODTextTranslationClass(uiWellChartsDisplayServer)
public:
			uiWellChartsLogToolWinGrp(uiParent*,
				const ObjectSet<uiWellLogToolWin::LogData>&);
			~uiWellChartsLogToolWinGrp();

    void		displayLogs() override;

protected:
    uiLogViewTable*	logviewtbl_;
};
