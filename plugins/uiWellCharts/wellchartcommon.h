#pragma once

/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
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
