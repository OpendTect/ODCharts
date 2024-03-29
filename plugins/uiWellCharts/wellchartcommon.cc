/*+
________________________________________________________________________

 Copyright:	(C) 1995-2022 dGB Beheer B.V.
 License:	https://dgbes.com/licensing
________________________________________________________________________

-*/

#include "wellchartcommon.h"
#include "uistrings.h"

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


mDefineNameSpaceEnumUtils( uiWellCharts, Scale, "Log chart scale type" )
{
    "Linear",
    "Log10",
    nullptr
};


template<>
void EnumDefImpl<uiWellCharts::Scale>::init()
{
    uistrings_ += mEnumTr("Linear");
    uistrings_ += mEnumTr("Log10");
}


mDefineNameSpaceEnumUtils( uiWellCharts, ZType, "Log chart Z type" )
{
    "MD",
    "TVD",
    "TVDSS",
    "TVDSD",
    "TWT",
    nullptr
};


template<>
void EnumDefImpl<uiWellCharts::ZType>::init()
{
    uistrings_ += uiStrings::sMD();
    uistrings_ += uiStrings::sTVD();
    uistrings_ += uiStrings::sTVDSS();
    uistrings_ += uiStrings::sTVDSD();
    uistrings_ += uiStrings::sTWT();
}
