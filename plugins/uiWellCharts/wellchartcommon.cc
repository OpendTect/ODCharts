/*+
________________________________________________________________________

 (C) dGB Beheer B.V.; (LICENSE) http://opendtect.org/OpendTect_license.txt
 Author:	Nanne Hemstra
 Date:		August 2021
________________________________________________________________________

-*/


#include "wellchartcommon.h"

mDefineNameSpaceEnumUtils( uiWellCharts, FillLimit, "Log Fill Limit" )
{
    "Track",
    "Baseline",
    "Curve",
    nullptr
};


template<>
void EnumDefImpl<uiWellCharts::FillLimit>::init()
{
    uistrings_ += mEnumTr("Track");
    uistrings_ += mEnumTr("Baseline");
    uistrings_ += mEnumTr("Curve");
}
