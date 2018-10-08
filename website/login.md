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
<img src="/assets/images/GitHub-Mark.png" alt="Github" style="width:64px;height:64px;padding:10px;"/>Connect with GitHub
</a>
</div>
<div class="login-choices">
<a href="https://api.2018.halite.io/v1/login/google" class="login-choice">
<img src="/assets/images/btn_google_light.png" alt="Google" style="width:64px;height:64px;padding:10px;"/>Connect with Google
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
