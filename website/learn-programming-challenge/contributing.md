---
layout: doc_page
title: Contributing
breadcrumb: Learn
toc: false
description: Learn about how to contribute to Halite III
sort_key: 4
---

<div class="doc-section" markdown="1">
#### Open Source
Halite is an open source project. We encourage you to check out our source code on [Github](https://github.com/HaliteChallenge/Halite-III). Here are some of the ways you can contribute:

<br/>

* [OPEN ISSUES ON GITHUB](https://github.com/HaliteChallenge/Halite-III/issues)
* [COMMUNITY TOOLS](#community-tools)
* [NEW STARTER KIT GUIDE](#create-a-new-starter-kit)


<br/>

---------
<br/>
## Community Tools
Our community creates and shares a wide variety of tools for developing Halite bots. Our [Community Halite Projects](https://forums.halite.io/c/community-halite-projects) forum is a great place to talk about a tool you're working on, or one you'd like to see!

* [Match Manager](https://gitlab.com/smiley1983/halite2-match-manager)
* [Reloader Script](https://forums.halite.io/t/halite-3-reloader-script/120) displays the difference a bot's output and a given replay file.
* [Alternative Local Replay Viewer](https://forums.halite.io/t/fluorine-a-halite-3-replay-viewer/99)


<br/>

------------

<br/>
## Create a New Starter Kit
<br/>
Thank you for your interest in creating a new Halite III starter kit! Starter kits help players develop Halite bots in their preferred language. We have put together this guide to help you create a new starter kit that will work with our game engine and be easy to use for other players. If you have any questions, feel free to reach out via email at [halite@halite.io](mailto:halite@halite.io).


<br/>
### API Design
Please adhere as best you can to the API design used in the Python3 starter kit. Our API documentation is [here](/learn-programming-challenge/api-docs).


<br/>
### Game Communication Overview
Bots communicate with the environment via JSON, stdin, and stdout using a stream of space-separated integers. There are two stages of communication: initialization and turn formats.


<br/>
### Initialization
**Constants**

At the beginning of the game, the bot must parse a blob of JSON constants from the game environment.
* If your language doesn’t support JSON parsing easily, you can treat all of the special JSON characters as separators and parse the resulting flat key-value stream (the Java and C++ starter kits handle the constants this way, for reference).
* Your starter kit should silently ignore unknown constants.

**Game Data**

The game environment also sends a stream of space-separated integers representing:
1. The number of players and their player IDs.
2. The location of each players’ shipyards.
3. A full list of each map cell’s location and halite amount.


<br/>
### Turn
Each turn, every bot is sent:
1. The turn number.
2. Each player in the game.
  * A list of this player’s ships.
  * A list of this player’s dropoffs and shipyards.
3. The changed cells on the game map, represented by an update count (how many cells have changed) and the details of changed cells (x, y, halite amount).

Some objects are recreated from scratch each turn, such as ships. Other objects are reused and updated each turn, such as the the map and player. It’s helpful to be clear when writing your starter kit which objects are recreated and which are reused from turn to turn.


<br/>
### Submitting your new starter kit
To submit your starter kit, fork our repo, place your starter package in the `starter-kits/` folder, and send us a pull request! If we accept your PR, your starter package will be added to the site.


**Note:** please include the `run_game.sh` and `run_game.bat` scripts, following the Python3 starter kit as a template.


<br/>
### Tips
* Adhere to the Python 3 starter kit design, function/object/variable names, and class structure as much as possible. The Python 3 kit is the most fully documented, and the online API documentation refers to the Python 3 starter kit.
* Support seeding the internal bot RNG from the command line, as it’s helpful for debugging.
* The game environment allows time to analyze the initial game map before your bot returns its name. Make sure that your starter kit bot can take advantage of this time by keeping analysis functions separate, as in existing kits.

<br/>

</div>
