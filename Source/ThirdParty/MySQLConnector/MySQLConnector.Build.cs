// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class MySQLConnector : ModuleRules
{
    public MySQLConnector(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string MySQLConnectorLibName = "mysqlcppconn.lib";
            string MySQLConnectorLibName2 = "mysqlcppconn8.lib";
            string MySQLConnectorDllName = "mysqlcppconn8-2-vs14.dll";
            string MySQLConnectorIncludePath = Path.Combine(ModuleDirectory, "Windows", "include");
            string MySQLConnectorLibraryPath = Path.Combine(ModuleDirectory, "Windows", "lib64");


            PublicIncludePaths.Add(MySQLConnectorIncludePath);
            PublicLibraryPaths.Add(MySQLConnectorLibraryPath);

            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(MySQLConnectorLibraryPath, MySQLConnectorLibName));
            PublicAdditionalLibraries.Add(Path.Combine(MySQLConnectorLibraryPath, MySQLConnectorLibName2));
            
            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add(MySQLConnectorDllName);

            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/MySQLConnector/Win64/" + MySQLConnectorDllName);
        }
    }
}
