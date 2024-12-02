using Sharpmake;

namespace IGSkills
{
	[Generate]
	public class EngineLibProject : BaseEngineProject
	{
		public EngineLibProject()
		{
			Name = "IGSkillsEngineLib";
		}

		[Configure]
		public override void ConfigureAll(Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);
			
			conf.TargetLibraryPath = "[project.BasePath]/generated/lib/[conf.Name]";

			conf.IncludePaths.Add("[project.BasePath]/Source/Public");

			conf.IncludePrivatePaths.Add("[project.BasePath]/Source/Private");

			// The library wants LIBRARY_COMPILE defined when it compiles the
			// library, so that it knows whether it must use dllexport or dllimport.
			conf.Defines.Add("LIBRARY_COMPILE");

			conf.Defines.Add("RENDERAPI_DIRECTX12");

			conf.LibraryFiles.Add("d3d12");
			conf.LibraryFiles.Add("dxgi");
			conf.LibraryFiles.Add("d3dcompiler");

			// Include all engine content for the game
			conf.EventPostBuildExe.Add(
				new Configuration.BuildStepCopy(
					"[project.BasePath]/Content",
					"output/Content",
					mirror: true
				)
			);

			if (target.OutputType == OutputType.Dll)
			{
				// We want this to output a shared library. (DLL)
				conf.Output = Configuration.OutputType.Dll;

				// This library project expects LIBRARY_DLL symbol to be defined
				// when used as a DLL. While we could define it in the executable,
				// it is better to put it as an exported define. That way, any
				// projects with a dependency on this one will have LIBRARY_DLL
				// automatically defined by Sharpmake.
				conf.ExportDefines.Add("LIBRARY_DLL");

				// Exported defines are not necessarily defines as well, so we need
				// to add LIBRARY_DLL as an ordinary define too.
				conf.Defines.Add("LIBRARY_DLL");
			}
			else if (target.OutputType == OutputType.Lib)
			{
				// We want this to output a static library. (LIB)
				conf.Output = Configuration.OutputType.Lib;
			}

			conf.AddPublicDependency<ZLib>(target);
			conf.AddPublicDependency<LibPNG>(target);
		}
	}

	[Export]
	public class ZLib : ExportProject
	{
		public string OutputPath = "[project.BasePath]/External/zlib-1.2.11/contrib/vstudio/vc14/x64";

		public override void ConfigureAll(Configuration conf, Target target)
		{
			conf.IncludePaths.Add("[project.BasePath]/External/zlib-1.2.11");
		}

		public override void ConfigureDebug(Configuration conf, Target target)
		{
			if (target.OutputType == OutputType.Dll)
			{
				conf.LibraryPaths.Add("[project.OutputPath]/ZlibDllDebug");
				conf.LibraryFiles.Add("zlibwapi.lib");
				conf.TargetCopyFiles.Add("[project.OutputPath]/ZlibDllDebug/zlibwapi.dll");
			}
			else
			{
				conf.LibraryPaths.Add("[project.OutputPath]/ZlibStatDebug");
				conf.LibraryFiles.Add("zlibstat.lib");
			}
		}

		public override void ConfigureRelease(Configuration conf, Target target)
		{
			if (target.OutputType == OutputType.Dll)
			{
				conf.LibraryPaths.Add("[project.OutputPath]/ZlibDllRelease");
				conf.LibraryFiles.Add("zlibwapi.lib");
				conf.TargetCopyFiles.Add("[project.OutputPath]/ZlibDllRelease/zlibwapi.dll");
			}
			else
			{
				conf.LibraryPaths.Add("[project.OutputPath]/ZlibStatRelease");
				conf.LibraryFiles.Add("zlibstat.lib");
			}
		}
	}

	[Export]
	public class LibPNG : ExportProject
	{
		public string OutputPath = "[project.BasePath]/External/lpng1644/projects/vstudio";

		public override void ConfigureAll(Configuration conf, Target target)
		{
			conf.IncludePaths.Add("[project.BasePath]/External/lpng1644");
		}

		public override void ConfigureDebug(Configuration conf, Target target)
		{
			if (target.OutputType == OutputType.Dll)
			{
				conf.LibraryPaths.Add("[project.OutputPath]/Debug");
				conf.LibraryFiles.Add("libpng16.lib");
				conf.TargetCopyFiles.Add("[project.OutputPath]/Debug/libpng16.dll");
			}
			else
			{
				conf.LibraryPaths.Add("[project.OutputPath]/Debug Library");
				conf.LibraryFiles.Add("libpng16.lib");
			}
		}

		public override void ConfigureRelease(Configuration conf, Target target)
		{
			if (target.OutputType == OutputType.Dll)
			{
				conf.LibraryPaths.Add("[project.OutputPath]/Release");
				conf.LibraryFiles.Add("libpng16.lib");
				conf.TargetCopyFiles.Add("[project.OutputPath]/Release/libpng16.dll");
			}
			else
			{
				conf.LibraryPaths.Add("[project.OutputPath]/Release Library");
				conf.LibraryFiles.Add("libpng16.lib");
			}
		}
	}
}