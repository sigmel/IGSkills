using Sharpmake;

namespace IGSkills
{
	public abstract class BaseEngineProject : Project
	{
		public string BasePath = "[project.SharpmakeCsPath]/../";

		protected BaseEngineProject()
		{
			SourceRootPath = "[project.BasePath]/Source";

			// Include our build files for easy access
			AdditionalSourceRootPaths.Add("[project.BasePath]/Build");
			SourceFilesExtensions.Add("cs");

			// Include our shader files for easy access
			AdditionalSourceRootPaths.Add("[project.BasePath]/Content/Shaders");
			SourceFilesExtensions.Add("hlsl");

			AddTargets(new Target(
				Platform.win64,
				DevEnv.vs2022,
				Optimization.Debug | Optimization.Release,
				OutputType.Dll | OutputType.Lib));

			IsFileNameToLower = false;
		}

		public virtual void ConfigureAll(Project.Configuration conf, Target target)
		{
			conf.Name = "[target.Optimization]_[target.OutputType]";
			conf.ProjectPath = "[project.BasePath]/generated/projects";

			conf.IntermediatePath = "[conf.ProjectPath]/obj/[target.Platform]_[conf.Name]";

			conf.VcxprojUserFile = new Project.Configuration.VcxprojUserFileSettings();
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(OutDir)";

			conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
			conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);

			conf.Options.Add(new Sharpmake.Options.Vc.Compiler.DisableSpecificWarnings(
				"4100" // unreferenced formal parameter
				//"4251"  // needs to have dll-interface to be used by clients
				));

			conf.Options.Add(Options.Vc.Linker.TreatLinkerWarningAsErrors.Enable);

			conf.Defines.Add("_HAS_EXCEPTIONS=0");
		}
	}

	public abstract class BaseProject : BaseEngineProject
	{
		public override void ConfigureAll(Project.Configuration conf, Target target)
		{
			base.ConfigureAll(conf, target);

			conf.AddPublicDependency<IGSkills.EngineLibProject>(target);
		}
	}

	public class BaseSolution : Solution
	{
		public string BasePath = "[solution.SharpmakeCsPath]/../";

		public BaseSolution()
		{
			AddTargets(new Target(
				Platform.win64,
				DevEnv.vs2022,
				Optimization.Debug | Optimization.Release,
				OutputType.Dll | OutputType.Lib));

			IsFileNameToLower = false;
		}

		public virtual void ConfigureAll(Configuration conf, Target target)
		{
			conf.Name = "[target.Optimization]_[target.OutputType]";
			conf.SolutionFileName = "[solution.Name]";
			conf.SolutionPath = "[solution.BasePath]";
		}
	}
}