// Some copyright should be here...

using UnrealBuildTool;
using System.IO;

public class MQTT : ModuleRules
{
	public MQTT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] {});
				
		PrivateIncludePaths.AddRange(new string[] { });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", });

        PrivateDependencyModuleNames.AddRange(new string[] {"CoreUObject", "Engine", "Slate", "SlateCore", "DeveloperSettings", });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });

        string LibKey = "libs";
        string IncludeKey = "includes";
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Source/ThirdParty/mosquitto")).ToString();

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string OsKey = "win64";

            // Include libs
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, "mosquitto.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, "mosquittopp.lib"));

            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, IncludeKey, OsKey));

            // DLL
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, LibKey, OsKey, "mosquitto.dll")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, LibKey, OsKey, "mosquittopp.dll")));
        }
        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            string OsKey = "lnx64";

            // Include libs
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libmosquitto.so"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libmosquittopp.so"));

            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, IncludeKey, OsKey));

            // DLL
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libmosquitto.so")));
            RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libmosquittopp.so")));
        }
    }
}
