---
layout: doc_page
title: Get Started
breadcrumb: Learn
toc: false
menu: learn
description: Welcome to the Halite II competition! Learn how to quickly get started downloading, building, and submitting an AI bot.
mount_view: Onboarding
additional_js: ["FileSaver.min"]
additional_css: []
---

<div class="doc-section" markdown="1">

#### Get to know the Game
Read the [**game rules**](/learn-programming-challenge/game-overview).

<br/>
#### Build a Bot
Build a bot locally on your computer by [**downloading**](/learn-programming-challenge/downloads) a starter kit and game environment
###### OR
Use our new [**online editor**](/editor), which includes a game visualizer and logging built right in.

<br/>
#### Submit a Bot
Submit your bot on our [**play**](/play-programming-challenge) page.

Check out the [**leaderboard**](/programming-competition-leaderboard), and learn how we rank bots (todo).

<br/>
#### Next Steps
Get tips on improving your bot in our [**next step tutorial**](/learn-programming-challenge/tutorials/next-tutorial).

Stuck? Get help with best practices and [**debugging your bot**](/learn-programming-challenge/developing-a-bot).

Check out the blog post feed (todo).

Get in-depth with our [**API docs**](/learn-programming-challenge/api-docs).

<br/>
#### Join the Community
Meet the community on the official [**forums**](https://forums.halite.io/).

Join us on Discord (todo)!

Check us out on Github (todo), and make a contribution to Halite III (todo).

<br/>
</div>

<div class="build-a-bot text-center">
  <a href="#" class="btn btn-primary">Build a bot</a>
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
