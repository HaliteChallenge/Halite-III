<template functional>
  <img
    :src="`https://github.com/${props.username}.png`"
    :alt="`User ${props.username}`"
    :title="props.username"
    @error="$event.target.src = fallbackAvatar(props.username)"
    :class="props.className || ''"
    :id="props.htmlId || ''"
    :height="props.height || ''"
    :width="props.width || ''"
    />
</template>
