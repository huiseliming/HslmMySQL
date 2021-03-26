// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using Tools.DotNETCommon;
using UnrealBuildTool;

public class MySQLConnector : ModuleRules
{
    
    public string GetUProjectPath()
    {
        //Change this according to your module's relative location to your project file. If there is any better way to do this I'm interested!
        //Assuming Source/ThirdParty/YourLib/
        return Directory.GetParent(ModuleDirectory).Parent.Parent.FullName;
    }

    private void CopyToBinaries(string Filepath, ReadOnlyTargetRules Target)
    {
        
        string binariesDir = Path.Combine(GetUProjectPath(), "Binaries", Target.Platform.ToString());
        string filename = Path.GetFileName(Filepath);
        if (!Directory.Exists(binariesDir))
            Directory.CreateDirectory(binariesDir);
        //Console.WriteLine(Filepath);
        //Console.WriteLine(Path.Combine(binariesDir, filename));
        if (!File.Exists(Path.Combine(binariesDir, filename)))
        {
            File.Copy(Filepath, Path.Combine(binariesDir, filename), true);
        }
    }
    
    public MySQLConnector(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string SSLDllName = "libssl-1_1-x64.dll";
            string CryptoDllName = "libcrypto-1_1-x64.dll";
            //JDBC
            string MySQLConnectorLibName = "mysqlcppconn.lib";
            string MySQLConnectorDllName = "mysqlcppconn8-2-vs14.dll";
            //X DevAPI?
            //string MySQLConnectorLibName = "mysqlcppconn8.lib";
            //string MySQLConnectorDllName = "mysqlcppconn-9-vs14.dll";
            string MySQLConnectorIncludePath = Path.Combine(ModuleDirectory, "Windows", "include");
            string MySQLConnectorLibraryPath = Path.Combine(ModuleDirectory, "Windows", "lib64");


            PublicIncludePaths.Add(MySQLConnectorIncludePath);
            //PublicLibraryPaths.Add(MySQLConnectorLibraryPath);
            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(MySQLConnectorLibraryPath, MySQLConnectorLibName));
            
            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add(CryptoDllName);
            PublicDelayLoadDLLs.Add(SSLDllName);
            PublicDelayLoadDLLs.Add(MySQLConnectorDllName);

            // Ensure that the DLL is staged along with the executable
            // Copy Dll For Run Standalone (Packaged)
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/MySQLConnector/Win64/" + CryptoDllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/MySQLConnector/Win64/" + SSLDllName);
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/MySQLConnector/Win64/" + MySQLConnectorDllName);
            // Copy Dll For Run Editor
            CopyToBinaries(Path.Combine(MySQLConnectorLibraryPath, SSLDllName), Target);
            CopyToBinaries(Path.Combine(MySQLConnectorLibraryPath, CryptoDllName), Target);
            CopyToBinaries(Path.Combine(MySQLConnectorLibraryPath, MySQLConnectorDllName), Target);
        }
    }
}
