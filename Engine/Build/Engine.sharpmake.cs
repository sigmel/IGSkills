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
		}
	}
}