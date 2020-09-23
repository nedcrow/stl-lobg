// Fill out your copyright notice in the Description page of Project Settings.


#include "LOBGGameInstance.h"

FString ULOBGGameInstance::GetUserID()
{
	if (UserID.IsEmpty())
	{
		return FString::Printf(TEXT("%dUser"), FMath::RandRange(1, 100));
	}
	return UserID;
}

void ULOBGGameInstance::SetUserID(FString NewUserID)
{
	UserID = NewUserID;
}
