// Fill out your copyright notice in the Description page of Project Settings.


#include "LOBGGameInstance.h"

FString ULOBGGameInstance::GetUserID()
{
	return UserID;
}

void ULOBGGameInstance::SetUserID(FString NewUserID)
{
	UserID = NewUserID;
}
