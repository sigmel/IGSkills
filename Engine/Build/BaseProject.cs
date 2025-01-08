using Sharpmake;

namespace IGSkills
{
	public class EngineTargets
	{
		public static Target[] Targets
		{
			get
			{
				return new Target[]{ new Target(
					Platform.win64,
					DevEnv.vs2022,
					Optimization.Debug | Optimization.Release,
					OutputType.Dll | OutputType.Lib) };
			}
		}
	}

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

			AddTargets(EngineTargets.Targets);

			IsFileNameToLower = false;
		}

		[Configure]
		public virtual void ConfigureAll(Project.Configuration conf, Target target)
		{
			conf.Name = "[target.Optimization]_[target.OutputType]";
			conf.ProjectPath = "[project.BasePath]/generated/projects";

			conf.IntermediatePath = "[conf.ProjectPath]/obj/[target.Platform]_[conf.Name]";

			conf.VcxprojUserFile = new Project.Configuration.VcxprojUserFileSettings();
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = "$(ProjectDir)/output/";

			conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
			conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);

			conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);

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

			conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);

			// Include all content for the engine and game
			// need to do this manually because Sharpmake doesn't include an option to do subdirectories and I didn't want to make a C# function that will iterate and set up a bunch of BuildStepCopy commands for each directory
			string EngineInputPath = "[project.BasePath]/../../Engine/Content";
			string ProjectInputPath = "[project.BasePath]/Content";
			string OutputPath = "output/Content";
			conf.EventPostBuild.Add($"robocopy.exe /xo /ns /nc /np /njh /njs /ndl /nfl /e \"{EngineInputPath}\" \"{OutputPath}\" \"*\" > nul & if %ERRORLEVEL% GEQ 8 (echo Copy failed & exit 1) else (type nul>nul)");
			conf.EventPostBuild.Add($"robocopy.exe /xo /ns /nc /np /njh /njs /ndl /nfl /e \"{ProjectInputPath}\" \"{OutputPath}\" \"*\" > nul & if %ERRORLEVEL% GEQ 8 (echo Copy failed & exit 1) else (type nul>nul)");
		}
	}

	public class ExportProject : Project
	{
		public string BasePath = "[project.SharpmakeCsPath]/../";

		public ExportProject()
		{
			AddTargets(EngineTargets.Targets);
		}

		[Configure(), ConfigurePriority(1)]
		public virtual void ConfigureAll(Configuration conf, Target target)
		{
		}

		[Configure(Optimization.Debug), ConfigurePriority(2)]
		public virtual void ConfigureDebug(Configuration conf, Target target)
		{
		}

		[Configure(Optimization.Release), ConfigurePriority(3)]
		public virtual void ConfigureRelease(Configuration conf, Target target)
		{
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

		[Configure]
		public virtual void ConfigureAll(Configuration conf, Target target)
		{
			conf.Name = "[target.Optimization]_[target.OutputType]";
			conf.SolutionFileName = "[solution.Name]";
			conf.SolutionPath = "[solution.BasePath]";
		}
	}
}