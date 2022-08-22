#pragma once
/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "uiwellchartsmod.h"

#include "enums.h"

namespace uiWellCharts
{
    enum FillLimit	{ Track, Baseline, Curve };
			mDeclareNameSpaceEnumUtils(uiWellCharts,FillLimit)

    enum Scale		{ Linear, Log10 };
			mDeclareNameSpaceEnumUtils(uiWellCharts,Scale)

    enum ZType		{ MD, TVD, TVDSS, TVDSD, TWT };
			mDeclareNameSpaceEnumUtils(uiWellCharts,ZType)
};
