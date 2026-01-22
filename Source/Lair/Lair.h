// Lair.h
// LAIR Game Module Header

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * LAIR Game Module
 * Board game adaptation for Unreal Engine 5
 */
class FLairModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
