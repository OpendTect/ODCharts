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

    uiMainWin*		createLogViewWinCB(uiParent*,
					const ObjectSet<Well::Data>&,
					const BufferStringSet& lognms,
					const BufferStringSet& markernms,
					const DBKeySet& sel_ids,
					const BufferStringSet& sel_lognms,
					const BufferStringSet& sel_markernms,
				        const CallBack& filtercb)
					override;
    uiMainWin*		createLogViewWinCB(uiParent*,
					const ObjectSet<Well::Data>&,
					const MnemonicSelection&,
					const BufferStringSet& markernms,
					const DBKeySet& sel_ids,
					const MnemonicSelection& sel_mns,
					const BufferStringSet& sel_markernms,
				        const CallBack& filtercb)
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
