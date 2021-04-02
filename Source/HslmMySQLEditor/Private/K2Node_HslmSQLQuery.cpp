// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_HslmSQLQuery.h"
#include "SQLQueryProxy.h"

#define LOCTEXT_NAMESPACE "K2Node_AIMoveTo"

UK2Node_HslmSQLQuery::UK2Node_HslmSQLQuery(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(USQLQueryProxy, CreateSQLQueryProxyObject);
	ProxyFactoryClass = USQLQueryProxy::StaticClass();
	ProxyClass = USQLQueryProxy::StaticClass();
}

FText UK2Node_HslmSQLQuery::GetMenuCategory() const
{
	return LOCTEXT( "HslmSQLQuery", "Hslm|K2Node" );
}

FText UK2Node_HslmSQLQuery::GetTooltipText() const
{
	return LOCTEXT("HslmSQLQuery_Tooltip", "Async mysql query for blueprin graph");
}

FText UK2Node_HslmSQLQuery::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("HslmSQLQuery", "Hslm SQLQuery");
}

#undef LOCTEXT_NAMESPACE


