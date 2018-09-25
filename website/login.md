---
layout: static_page
title: Log In to Halite III
content: website
---

<div class="doc-section text-center login-section">

<h4 class="mt3">Choose your sign-in method:</h4>

<div class="login-choices">
<a href="https://api.2018.halite.io/v1/login/github" class="login-choice">
<img src="/assets/images/octocat.jpg" />
Sign-in/register with GitHub
</a>
<a href="https://api.2018.halite.io/v1/login/google" class="login-choice">
<i class="fa fa-google"></i> Sign-in/register with Google
</a>
</div>

</div>

<script>
  window.addEventListener("DOMContentLoaded", function() {
    window.api.me().then(function(me) {
      if (me) {
        window.location = '/user?me';
      }
    })
  })
</script>
