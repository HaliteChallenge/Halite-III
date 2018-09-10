---
layout: doc_page
title: Get Started
breadcrumb: Learn
toc: false
menu: learn
description: Welcome to the Halite III competition! Learn how to quickly get started downloading, building, and submitting an AI bot.
mount_view: Onboarding
additional_js: ["FileSaver.min"]
additional_css: []
---

<div class="doc-section" markdown="1">

#### Welcome to the Halite III Beta

We look forward to seeing you on the leaderboard!

Please send all feedback, starter kit inquiries, and new feature proposals to [**halite@halite.io**](mailto:halite@halite.io).

<br/>

#### Get to know the Game
Start with the [**game rules**](/learn-programming-challenge/game-overview).

Then, learn more about how to play the game in a [**tutorial**]((/learn-programming-challenge/tutorials/)).

<br/>

#### Build a Bot

[**Download**](/learn-programming-challenge/downloads) a starter kit and game environment to build a bot locally on your computer. Submit your bot on the [**play**](/play-programming-challenge) page.
###### OR
Build and submit your bot via the [**online editor**](/editor), which includes built-in logging and game visualization.


<br/>

#### Reference
Check out the [**API documentation**](/learn-programming-challenge/api-docs).

Get tips on improving your bot in our [**next step tutorial**](/learn-programming-challenge/tutorials/next-tutorial).

Stuck? Get help with best practices and [**debugging your bot**](/learn-programming-challenge/developing-a-bot).

[comment]: Check out the blog post feed (todo).


<br/>

[comment]: #### Join the Community
[comment]: Meet the community on the official [**forums**](https://forums.halite.io/).

[comment]: Join us on Discord (todo)!

[comment]: Check us out on Github (todo), and make a contribution to Halite III (todo).

[comment]: <br/>
</div>


<script>
  var site_downloads = {{ site.data.downloads | jsonify }};
</script>

[login]: {{ site.login_server_url }}/github
[downloads]: {{ site.baseurl }}/learn-programming-challenge/downloads-and-starter-kits
[editor]: {{ site.baseurl }}/editor
[contact]: {{ site.baseurl }}/about/contact-us
[play]: {{ site.baseurl }}/play-programming-challenge
[watch]: {{ site.baseurl }}/watch-games
[submit]: {{ site.baseurl }}/learn-programming-challenge/downloads-and-starter-kits/submit
[leaderboard]: {{ site.baseurl }}/programming-competition-leaderboard
[game rules]: {{ site.baseurl }}/learn-programming-challenge/basic-game-rules
[profile]: /user/?me
[improve bot]: /learn-programming-challenge/downloads-and-starter-kits/improve-basic-bot
[example game 1]: {{ site.baseurl }}/play/?game_id=3996012&replay_class=1&replay_name=replay-20180730-134819%2B0000-1532958122-96-96
[example game 2]: {{ site.baseurl }}/play/?game_id=3995999&replay_class=1&replay_name=replay-20180730-133054%2B0000-1532957367-48-48
[david game]: /play/?game_id=1658857&replay_class=0&replay_name=replay-20171016-183304%2B0000--998929535-240-160-1508178764
