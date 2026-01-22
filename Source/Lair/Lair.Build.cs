// Lair.Build.cs
// Module build configuration for LAIR game

using UnrealBuildTool;

public class Lair : ModuleRules
{
	public Lair(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});

		// Uncomment if using enhanced input
		// PublicDependencyModuleNames.Add("EnhancedInput");
	}
}
