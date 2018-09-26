---
layout: static_page
title: Sign up/Log In
content: website
---

<div class="doc-section text-center login-section">

<h4 class="mt3">Choose your account type:</h4>

<div class="login-choices">
<a href="https://api.2018.halite.io/v1/login/github" class="login-choice">
<!-- <img src="/assets/images/octocat.jpg" /> -->
<i class="fa fa-github">&nbsp;</i>Connect with GitHub
</a>
</div>
<div class="login-choices">
<a href="https://api.2018.halite.io/v1/login/google" class="login-choice">
<i class="fa fa-google">&nbsp;</i>Connect with Google
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
