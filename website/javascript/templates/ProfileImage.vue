<template functional>
  <img
    :src="$options.methods.profileUrl(props)"
    :alt="`${props.username}'s profile image`"
    :title="props.title || props.username"
    @error="$event.target.src = $options.methods.fallbackAvatar(props.username)"
    :class="props.className || ''"
    :id="props.htmlId || ''"
    :height="props.height || ''"
    :width="props.width || ''"
    />
</template>

<script>
  import * as api from '../api';
  export default {
    methods: {
      fallbackAvatar(username) {
        return api.fallbackAvatar(username);
      },
      profileUrl(props) {
        return props.profileImage ?
               `https://github.com/${props.profileImage}.png` :
               this.fallbackAvatar(props.username);
      },
    },
  }
</script>
