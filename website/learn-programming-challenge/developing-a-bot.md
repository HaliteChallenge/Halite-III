---
layout: doc_page
title: Developing a Bot
breadcrumb: Learn
toc: false
description: Get help debugging your bot, and hints about best practices for writing a bot.
sort_key: 3
---

<div class="doc-section" markdown="1">

#### Debugging

<br/>

##### Compilation Failures
You will get an email notifying you if your bot fails to compile on the game servers. The most common causes of compilation failures are:
* **Compilation errors in your code:** Make sure that you can compile your bot locally before submitting it to our game servers. A typo can make a big difference!
* **Infrastructure issues:** You can reach out to the Halite Team to add first class support for your bot. To read more about customizing a bot, check out [our guide](/learn-programming-challenge/contributing).
* **Incorrect bot archive structure:** Your file structure should adhere to the following format:
  * You should have a `MyBot.{extension for language}` in the root folder of your zip archive.
  * If you are building on top of a starter kit provided by us, make sure to include the /hlt folder.

<br/>

##### Timeout Failures
Your bot will be killed if it exceeds the following time limits during game execution. You will receive a timeout email with a link to the log. You can also access these logs on your user profile.
* **Install Time:** Prior to game start, you have `10 minutes` to install any dependencies.
* **Initialization Time:** Prior to the first turn, you have `30 seconds` to compute any game specific data and return your bot’s name.
* **Per Turn Time:** Each player has at `max 2 seconds` per turn to respond with a command to the engine. If your bot exceeds this time limit, your bot is killed and loses the match.


To prevent your bot from timing out, you should optimize your code wherever possible. You might implement a timer to ensure that your bot returns commands in time to prevent losing the match.

When debugging timeout issues, it can be helpful to disable game engine timeouts. To do so, append the --no-timeouts flag when running a game:
`$ ./halite -vvv --width 32 --height 32 --no-timeouts "python MyBot.py" "python MyBetterBot.py"`

*Note:* In Windows, escape flags with dashes in them: `--"no-timeouts"`

<br/>

##### In Game Failures
Your bot could be crashing because of errors during game execution.
* **Incorrect commands:** The most common reason for incorrect commands is using the inbuilt functions incorrectly, or writing new functions that emit commands that the game engine cannot parse. Check your formatting, and make sure that your raw command strings are valid engine commands.
* **Illegal commands:** If your bot sends commands that are disallowed by the engine, the engine will kill your bot, resulting in a game loss. Some examples of these errors are attempting to send the same ship multiple commands, spending more halite in a turn than you have, or building a dropoff at an already-owned location.
* **Out of memory:** Bots have a memory limit of 1GB. If your code exceeds this limit, your bot is killed.


<br/>
</div>

<div class="doc-section" markdown="1">

#### Best Practices

<br/>

##### Logging
stdout and stdin are used to communicate with the game engine. Any use of functions like `System.out.println`, `print()`, or `std::cout` will cause engine errors. Instead, print debugging information to a log file.

The starter kits have some functionality to write to a log file, and should include a simple log statement for your reference.

When your bot times out or errors on the game servers, we save and display a log file with debugging information. You will be sent an email with this log, or you can visit your profile to find the logs from your recent games.

<br/>

##### Test new vs. old before upload
Before submitting a new bot, we recommend running some games against the previous version of your bot. You can play your bot against older versions using the CLI.

<br/>

##### Using the CLI
Your Halite Starter Kit comes with an executable that allows you to use a command line interface (CLI) to run games locally. You might find the CLI useful for specifying parameters and conditions for a game of Halite III.

To see all available flags, run:
`$ ./halite --help`

<br/>

##### Reproducing Maps
You can replay games with the exact same map to debug or tweak bot behavior. To do so, specify a map seed with the -s or --seed flag followed by an integer when running a game:
`$ ./halite -vvv --width 32 --height 32 --seed 4 "python MyBot.py" "python MyBetterBot.py"`

<br/>

</div>

<div class="build-a-bot text-center">
  <a href="/editor" class="btn btn-primary">Build a bot</a>
</div>
