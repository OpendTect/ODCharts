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

    uiMainWin*		createMultiWellDisplay(uiParent*, const DBKeySet&,
					    const BufferStringSet&) override;
    uiWellLogToolWinGrp* createWellLogToolGrp(uiParent*,
			const ObjectSet<WellLogToolData>&) override;

    uiMainWin*		createLogViewWin(uiParent*,
					const ObjectSet<Well::Data>&,
					const BufferStringSet& lognms,
					const BufferStringSet& markernms)
					override;
    uiMainWin*		createLogViewWin(uiParent*,
					const ObjectSet<Well::Data>&,
					const MnemonicSelection&,
					const BufferStringSet& markernms)
					override;

    uiLogDisplayGrp*	createLogDisplayGrp(uiParent*) override;
};


mExpClass(uiChartTools) uiWellChartsLogToolWinGrp
					: public uiWellLogToolWinGrp
{
mODTextTranslationClass(uiWellChartsDisplayServer)
public:
			uiWellChartsLogToolWinGrp(uiParent*,
				const ObjectSet<WellLogToolData>&);
			~uiWellChartsLogToolWinGrp();

    void		displayLogs() override;
    void		displayMarkers(const BufferStringSet& mrknms) override;

protected:
    uiLogViewTable*	logviewtbl_;
};
