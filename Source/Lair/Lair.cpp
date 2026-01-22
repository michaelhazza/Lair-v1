// Lair.cpp
// LAIR Game Module Implementation

#include "Lair.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FLairModule, Lair, "Lair");

void FLairModule::StartupModule()
{
	// Module startup logic
	UE_LOG(LogTemp, Log, TEXT("LAIR Module Started"));
}

void FLairModule::ShutdownModule()
{
	// Module shutdown logic
	UE_LOG(LogTemp, Log, TEXT("LAIR Module Shutdown"));
}
