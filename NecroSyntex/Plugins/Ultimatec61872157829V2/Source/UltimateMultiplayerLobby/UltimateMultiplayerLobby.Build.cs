// Copyright (c) 2024 Betide Studio. All Rights Reserved.

using System;
using EpicGames.Core;
using UnrealBuildTool;

public class UltimateMultiplayerLobby : ModuleRules
{
	public UltimateMultiplayerLobby(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
				"InputCore",
				"UMG",
				"Slate",
				"SlateCore",
				"Json",
				"JsonUtilities",
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"CinematicCamera",
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		if (ConfigurePlugins(this, Target))
		{
			foreach (string PublicDefinition in PublicDefinitions)
			{
				Console.WriteLine("PublicDefinition: " + PublicDefinition);
			}
			
			if(PublicDefinitions.Contains("USING_STEAM_INTEGRATION_KIT=1"))
			{
				Console.WriteLine("Using Steam Integration Kit");
				PublicDependencyModuleNames.AddRange(
					new string[]
					{
						"SteamIntegrationKit",
						"SteamSdk"
					});
			}
			else if(PublicDefinitions.Contains("USING_ONLINESUBSYSTEMSTEAM=1"))
			{
				Console.WriteLine("Using OnlineSubsystemSteam");
				PublicDependencyModuleNames.AddRange(
					new string[]
					{
						"OnlineSubsystemSteam",
						"Steamworks"
					});
			}
			else
			{
				Console.WriteLine("Using Steam with OnlineSubsystemSteam plugin");
				PublicDependencyModuleNames.AddRange(
					new string[]
					{
						"OnlineSubsystemSteam",
						"Steamworks"
					});
			}
		}
		else
		{
			Console.WriteLine("Not using Steam");
			PublicDefinitions.Add("USING_STEAM=0");
			PublicDefinitions.Add("USING_STEAM_INTEGRATION_KIT=0");
			PublicDefinitions.Add("USING_ONLINESUBSYSTEMSTEAM=0");
		}
	}
	
	static public bool ConfigurePlugins(ModuleRules Rules, ReadOnlyTargetRules Target)
	{
		if (Target.bUsesSteam == true)
		{
			Rules.PublicDefinitions.Add("USING_STEAM=1");
			Rules.PublicDefinitions.Add("USING_ONLINESUBSYSTEMSTEAM=1");
		}

		JsonObject RawObject;
		if (JsonObject.TryRead(Target.ProjectFile, out RawObject))
		{
			JsonObject[] pluginObjects;
			if (RawObject.TryGetObjectArrayField("Plugins", out pluginObjects))
			{
				foreach (JsonObject pluginObject in pluginObjects)
				{
					string pluginName;
					pluginObject.TryGetStringField("Name", out pluginName);

					bool pluginEnabled;
					pluginObject.TryGetBoolField("Enabled", out pluginEnabled);
					Console.WriteLine("Plugin: " + pluginName + " Enabled: " + pluginEnabled);
					if (pluginName == "SteamIntegrationKit" && pluginEnabled)
					{
						if (Target.bUsesSteam == false)
						{
							Rules.PublicDefinitions.Add("USING_STEAM=1");
							Rules.PublicDefinitions.Add("USING_STEAM_INTEGRATION_KIT=1");
							Rules.PublicDefinitions.Add("USING_ONLINESUBSYSTEMSTEAM=0");
							return true;
						}
					}
					if (pluginName == "OnlineSubsystemSteam" && pluginEnabled)
					{
						if (Target.bUsesSteam == false)
						{
							Rules.PublicDefinitions.Add("USING_STEAM=1");
							Rules.PublicDefinitions.Add("USING_ONLINESUBSYSTEMSTEAM=1");
							Rules.PublicDefinitions.Add("USING_STEAM_INTEGRATION_KIT=0");
							//return true
							return true;
						}
					}
				}
			}
		}
		if (Target.bUsesSteam)
		{
			return true;
		}
		return false;
	}
}
