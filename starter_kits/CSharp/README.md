# CSharp Starter Kit

## Halite III starter kit components
* MyBot.{extension}, a starter bot outline
* /hlt directory, which contains modifiable helper functions for your bot
* A Halite executable that enables local playtesting of your bot
* The scripts run_game.bat (Windows) and run_game.sh (MacOS, Linux)

## Testing your bot locally
* Run run_game.bat (Windows) and run_game.sh (MacOS, Linux) to run a game of Halite III. By default, these scripts run a game of your bot vs. itself.  You can modify the board size, map seed, and the opponents of test games.

Some interesting modifications you can make to the run_game files:
- Set the `--height 32` and `--width 32` flags
- Use a `--seed 1234` flag to play on a specific seed
- Set the `--no-timeout` flag, very useful for debugging.

## Submitting your bot
* Zip your MyBot.csproj file, your Sharpie.cs file and the /hlt directory together.
* It is *IMPERATIVE* that your project be called **MyBot.csproj** 
* Submit your zipped file here: https://halite.io/play-programming-challenge

## Using the Visual Studio Debugger
If you would like to use the VS Debugger:

1. Add a reference `using System.Diagnostics;` in MyBot.cs
2. At the beginning of the main method in MyBot.cs, add the following line: `while(!Debugger.IsAttached);`
3. Build your bot using `dotnet publish -c Debug -r win10-x64`. This creates a special version of your bot that will wait until the debugger attaches. Look in `bin\Debug\netcoreapp2.0\win10-x64`. 
4. Add the `--no-timeout` flag to run_game.bat in order to prevent the bot from losing immediately.
5. Ensure that you have set a break point somewhere in your code.
6. Modify your `run_game` script so that one of the players is the "MyBot.exe" that was built with the debugging loop.
6. Run the bot using one of the `run_game` scripts. You should notice that the game hangs right before the first move.
7. In Visual Studio, in the toolbar click on `Debug>Attach to Process` and select MyBot.exe from the list
8. Debug to your heart's desire.

## Some useful tips about NuGet packages
NuGet packages do indeed work on the Halite server. When we call `dotnet build` one of the steps that takes place is `dotnet restore`. In `dotnet restore`, the packages you use are downloaded and linked.

Just to be sure though, there's two things you might want to check for before uploading the bot:
1. In the Solution Explorer, under Dependencies, your package is showing up.
2. In your MyBot.csproj file, there is an ItemGroup that contains all your dependencies. For example:

```
<ItemGroup>
    <PackageReference Include="Newtonsoft.Json" Version="11.0.2" />
</ItemGroup>
```
