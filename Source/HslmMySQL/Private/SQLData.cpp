#include "SQLData.h"
#include "HslmMySQL.h"
#include "HslmDefine.h"
FSQLData::FSQLData(FString InString, ESQLData InSQLData)
{
	if (InSQLData == ESQLData::String)
	{
		SQLData = MakeShared<FSQLDataString>(InString);
	}
	else if (InSQLData == ESQLData::Decimal)
	{
		SQLData = MakeShared<FSQLDataDecimal>(InString);
	}
	else
	{
		UE_LOG(LogHslmMySQL, Fatal, TEXT("[%s] [tan90]"), *UE__FUNC__LINE__)
	}
}
