# halite3csharp
PopAye - A C# starter kit for Halite 3 written for .NET Core and VS Code. Works in Visual Studio as well with a few adjustments

## Usage
Copy halite.exe into root directory and run the batch/sh file to start.

## Submitting your bot
Copy MyBot.cs along with the Core directory and any other files/directories that you create on top of these into a zip file and submit.

## Using the Visual Studio Code Debugger
If you would like to use the built-in VS Code debugger to debug your code then do this:

1. in MyBot.cs, uncomment the below line:
    ```
        // while(!Debugger.IsAttached);
    ```
1. In the bat file, uncomment the last line which has the `--no-timeout` flag set and comment out the line which is for normal operations.
1. Run the simulation. At this point the simulation should freeze on the initialisation of the bot.
1. Ensure that you have set a break point somewhere in your code.
1. In Visual Studio Code, go to the Debug window and select `.NET Core Attach` from the Debug drop down.
1. Select dotnet.exe running Halite3.dll and your break points should start working.
