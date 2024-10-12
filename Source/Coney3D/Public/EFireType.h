// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EFireType.generated.h"

UENUM(BlueprintType, Category = "Weapon - Operation")
enum EFireType
{
	none UMETA(DisplayName = "None"),
	single UMETA(DisplayName = "Semi-auto"),
	rapidfire UMETA(DisplayName = "Full-auto"),
};
