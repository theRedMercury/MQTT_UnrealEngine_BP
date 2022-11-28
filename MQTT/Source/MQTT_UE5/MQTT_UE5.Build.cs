// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MQTT_UE5 : ModuleRules
{
	public MQTT_UE5(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        BuildVersion Version;
        if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version))
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "Slate", "SlateCore", "DeveloperSettings", });
        }

        PublicIncludePaths.AddRange(new string[] { });

        PrivateIncludePaths.AddRange(new string[] { });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });

        /*
         * THIRDPARTY LIB 
         * 
         * Mosquitto
         */
        PublicDefinitions.Add("WITH_MYTHIRDPARTYLIBRARY=1");

        string LibKey = "libs";
        string IncludeKey = "includes";
        string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Source/ThirdParty/mosquitto")).ToString();
        string BinaryPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Binaries", Target.Platform.ToString())).ToString();

        string OsKey = Target.Platform == UnrealTargetPlatform.Linux ? "lnx64" : "win64";
        string LibExt = Target.Platform == UnrealTargetPlatform.Linux ? ".so" : ".lib";
        string DllExt = Target.Platform == UnrealTargetPlatform.Linux ? ".so" : ".dll";

        string Mosquitto = Target.Platform == UnrealTargetPlatform.Linux ? "libmosquitto" : "mosquitto";
        string Mosquittopp = Target.Platform == UnrealTargetPlatform.Linux ? "libmosquittopp" : "mosquittopp";

        // Include libs
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, Mosquitto + LibExt));
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, LibKey, OsKey, Mosquittopp + LibExt));

        // Include path
        bLegacyPublicIncludePaths = true;
        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, IncludeKey, OsKey));

        // DLL
        if (Target.bBuildEditor)  // In Editor
        {
            RuntimeDependencies.Add(Path.Combine(BinaryPath, Mosquitto + DllExt));
            RuntimeDependencies.Add( Path.Combine(BinaryPath, Mosquittopp + DllExt));

            if (!System.IO.Directory.Exists(BinaryPath))
            {
                System.IO.Directory.CreateDirectory(BinaryPath);
            }

            File.Copy(Path.Combine(ThirdPartyPath, LibKey, OsKey, Mosquitto + DllExt), Path.Combine(BinaryPath, Mosquitto + DllExt), true);
            File.Copy(Path.Combine(ThirdPartyPath, LibKey, OsKey, Mosquittopp + DllExt), Path.Combine(BinaryPath, Mosquittopp + DllExt), true);

            if (OsKey == "win64")
            {
                File.Copy(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libcrypto-1_1-x64.dll"), Path.Combine(BinaryPath, "libcrypto-1_1-x64.dll"), true);
                File.Copy(Path.Combine(ThirdPartyPath, LibKey, OsKey, "libssl-1_1-x64.dll"), Path.Combine(BinaryPath, "libssl-1_1-x64.dll"), true);
            }
        }
        else // Create package : cooked project
        {
            RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", Mosquitto + DllExt), Path.Combine(BinaryPath, Mosquitto + DllExt));
            RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", Mosquittopp + DllExt), Path.Combine(BinaryPath, Mosquittopp + DllExt));

            if (OsKey == "win64")
            {
                RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libcrypto-1_1-x64.dll"), Path.Combine(BinaryPath, "libcrypto-1_1-x64.dll"));
                RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", "libssl-1_1-x64.dll"), Path.Combine(BinaryPath, "libssl-1_1-x64.dll"));
            }
        }
    }
}
