<template>
  <a
    :title="`${badge} Tier (${percentage}% of players)`"
    @click="open = !open"
  >
    <span class="tier-clickable" :class="tier"></span>
    <transition name="fadeIn">
      <div v-if="open" class="tier-popup">
        <header>
          <h4><span :class="tier"></span> {{badge}} Tier</h4>
          <span class="close-btn icon-remove" />
        </header>
        <p>{{percentage}}% of players achieve this badge.</p>
      </div>
    </transition>
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
        percentage: '',
        open: false,
      }
    },
  }
</script>

<style lang="scss" scoped>
    a {
      position: relative;
    }

    a:hover {
      text-decoration: none;
    }
</style>
