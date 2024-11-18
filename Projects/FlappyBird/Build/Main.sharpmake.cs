using Sharpmake;

[module: Sharpmake.Include("../../../Engine/Build/BaseProject.cs")]
[module: Sharpmake.Include("../../../Engine/Build/Engine.sharpmake.cs")]

[Generate]
public class FlappyBirdProject : IGSkills.BaseProject
{
	public FlappyBirdProject()
	{
		Name = "FlappyBird";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, Target target)
	{
		base.ConfigureAll(conf, target);
	}
}

[Generate]
public class FlappyBirdSolution : IGSkills.BaseSolution
{
	public FlappyBirdSolution()
	{
		Name = "FlappyBird";
	}

	[Configure]
	public override void ConfigureAll(Configuration conf, Target target)
	{
		base.ConfigureAll(conf, target);

		conf.AddProject<FlappyBirdProject>(target);
	}
}

public static class Main
{
	[Sharpmake.Main]
	public static void SharpmakeMain(Sharpmake.Arguments arguments)
	{
		arguments.Generate<FlappyBirdSolution>();
	}
}