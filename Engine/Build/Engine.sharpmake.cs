using Sharpmake;

namespace IGSkills
{
	[Sharpmake.Generate]
	public class EngineLibProject : Project
	{
		public string BasePath = @"[project.SharpmakeCsPath]/../";

		public EngineLibProject()
		{
			Name = "IGSkillsEngineLib";

			AddTargets(new Target(
				Platform.win64,
				DevEnv.vs2022,
				Optimization.Debug | Optimization.Release,
				OutputType.Lib | OutputType.Dll
			));

			SourceRootPath = "[project.BasePath]/Source";

			IsFileNameToLower = false;
		}

		[Configure()]
		public void Configure(Configuration conf, Target target)
		{
			conf.Name = "[target.Optimization]_[target.OutputType]";
			conf.ProjectPath = "[project.BasePath]/generated/projects";
			conf.TargetLibraryPath = "[project.BasePath]/generated/lib";
			conf.IntermediatePath = @"[conf.ProjectPath]/obj/[project.Name]/[target.Platform]_[conf.Name]_[target.DevEnv]";

			conf.Defines.Add("_HAS_EXCEPTIONS=0");

			conf.Options.Add(Options.Vc.Librarian.TreatLibWarningAsErrors.Enable);

			conf.IncludePaths.Add("[project.BasePath]/Source/Public");

			// Setup the precompiled headers for the project. Just assigning a
			// value to those fields is enough for Sharpmake to understand that
			// the project has precompiled headers.
			conf.PrecompHeader = "precomp.hpp";
			conf.PrecompSource = "precomp.cpp";

			// The library wants LIBRARY_COMPILE defined when it compiles the
			// library, so that it knows whether it must use dllexport or
			// dllimport.
			conf.Defines.Add("LIBRARY_COMPILE");

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