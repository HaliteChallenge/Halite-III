<template>
  <a href="#" :title="`${badge} Tier (${percentage}% of players)`">
    <span class="tier-clickable" :class="tier"></span>
  </a>
</template>

<script>
  import Vue from 'vue'
  const badges = ['Diamond', 'Platinum', 'Gold', 'Silver', 'Bronze']
  const percentages = ['1', '5', '10', '25', '59']

  export default {
    name: 'TierPopover',
    props: ['tier'],
    mounted: function () {
      const index = parseInt(this.tier.split('-')[2])
      const badge = badges[index - 1]
      this.index = index
      this.badge = badge
      this.percentage = percentages[index - 1]
  },
    data: function () {
      return {
        index: '',
        badge: '',
        percentage: ''
      }
    }
  }
</script>

<style lang="scss">
  .tier-clickable {
    cursor: pointer;
  }
</style>
