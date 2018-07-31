---
layout: doc_page
title: Get Started with Halite
breadcrumb: Learn
toc: false
menu: learn
description: Welcome to the Halite II competition! Learn how to quickly get started downloading, building, and submitting an AI bot.
mount_view: Onboarding
additional_js: ["FileSaver.min", "jszip.min", "jszip-utils.min", "editorBuild/code_edit/built-codeEdit.min"]
additional_css: ["built-codeEdit", "dark-codeEdit"]
---
<script>
  var site_downloads = {{ site.data.downloads | jsonify }};
</script>

<div id="onboarding-container"></div>

Welcome to the Halite III alpha! Let's get you set up to play.

### 1) SIGN UP

First, [sign up via Github][login] and create a Halite user profile.


### 2) GET A FEEL FOR THE GAME

Start by watching a few game replays, such as those [here][example game 1] or [here][example game 2].

Be sure to read through [the game rules][game rules], too.


### 3) BUILD A BOT

* Via the web-based [editor][editor].
* Locally. [Download the game environment and starter kit bundle][downloads] for your platform and language of choice.


### 4) SUBMIT A BOT

When you're satisfied with your bot's performance, submit it to the competition via the editor or [here][play].

Check out your [user profile][profile] and the [leaderboard][leaderboard] to view your games and ranking.


### May the best bot win!


[login]: {{ site.login_server_url }}/github
[downloads]: {{ site.baseurl }}/learn-programming-challenge/downloads-and-starter-kits
[editor]: {{ site.baseurl }}/editor
[contact]: {{ site.baseurl }}/about/contact-us
[play]: {{ site.baseurl }}/play-programming-challenge
[submit]: {{ site.baseurl }}/learn-programming-challenge/downloads-and-starter-kits/submit
[leaderboard]: {{ site.baseurl }}/programming-competition-leaderboard
[game rules]: {{ site.baseurl }}/learn-programming-challenge/basic-game-rules
[profile]: /user/?me
[improve bot]: /learn-programming-challenge/downloads-and-starter-kits/improve-basic-bot
[example game 1]: {{ site.baseurl }}/play/?game_id=3996012&replay_class=1&replay_name=replay-20180730-134819%2B0000-1532958122-96-96
[example game 2]: {{ site.baseurl }}/play/?game_id=3995999&replay_class=1&replay_name=replay-20180730-133054%2B0000-1532957367-48-48
[david game]: /play/?game_id=1658857&replay_class=0&replay_name=replay-20171016-183304%2B0000--998929535-240-160-1508178764
