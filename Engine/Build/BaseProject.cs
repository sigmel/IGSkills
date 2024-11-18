using Sharpmake;

namespace IGSkills
{
	public class BaseProject : Project
	{
		public string BasePath = @"[project.SharpmakeCsPath]/../";

		public BaseProject()
		{
			SourceRootPath = @"[project.BasePath]/Source";
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
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
			conf.ProjectPath = "[project.BasePath]/generated/projects";

			conf.IntermediatePath = @"[conf.ProjectPath]/obj/[project.Name]/[target.Platform]_[conf.Name]_[target.DevEnv]";

			conf.Options.Add(Options.Vc.Linker.TreatLinkerWarningAsErrors.Enable);

			conf.Defines.Add("_HAS_EXCEPTIONS=0");

			conf.AddPublicDependency<IGSkills.EngineLibProject>(target);
		}
	}

	public class BaseSolution : Solution
	{
		public string BasePath = @"[solution.SharpmakeCsPath]/../";

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
			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
			conf.SolutionPath = @"[solution.BasePath]";
		}
	}
}