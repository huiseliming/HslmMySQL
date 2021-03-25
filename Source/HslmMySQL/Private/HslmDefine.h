#pragma once

// Helper for display code file and line
#define UE__FUNC__ (FString(__FUNCTION__))
#define UE__LINE__ (FString::FromInt(__LINE__))
#define UE__FUNC__LINE__ (UE__FUNC__ + "(" + UE__LINE__ + ")")
