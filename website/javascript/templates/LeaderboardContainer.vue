<template>
  <Leaderboard v-if="leaderboard" :lbFromContainer="leaderboard" :baseUrl="baseUrl" :metric="metric"></Leaderboard>
</template>
<script>
import Vue from 'vue'
import * as api from '../api'
import _ from 'lodash'
import Leaderboard from './Leaderboard.vue'

export default {
    name: 'LeaderBoardContainer',
    props: ['baseUrl'],
    components: {Leaderboard},
    mounted: function () {
      this.fetchData()

      // determine whether the collapseable summary panel should be collapsed
      this.setupCollapseStats()
    },
    data: function () {
      return {
        isGlobalActive: true,
        metric: {
          organizations: 20,
          countries: 0,
          players: 0
        },
        classes: {
          professional: 0,
          university: 0,
          high_school: 0
        },
        leaderboard: null,
        isMobile: window.mobileAndTabletcheck()
      }
    },
    methods: {
      toggleStats: function () {
        setTimeout(() => {
          const collapsed = !$('#panel_metric').hasClass('in')
          this.$cookie.set('leaderboard_stats_collapsed', collapsed ? 1 : 0)
        }, 500)
      },

      setupCollapseStats: function () {
        const collapse = this.$cookie.get('leaderboard_stats_collapsed')
        if (collapse == 1 || window.mobileAndTabletcheck()) {
          $('#panel_metric').removeClass('in')
          $('#toggle_metric').attr('aria-expanded', 'false')
        }
      },

      fetchGlobal: function () {
        this.isGlobalActive = true
        this.fetchData()
      },

      fetchData: function () {
        api.leaderboard([], 0, 999999).then(leaderboard => {
          this.leaderboard = leaderboard
          let classes = {
            professional: 0,
            university: 0,
            high_school: 0
          }
          let countries = []
          let country_count = 0
          let org_count = 0
          let org_ids = []

          leaderboard.forEach(function (item) {
            if (item.level == 'Professional') {
              classes.professional += 1
            } else if (item.level == 'University') {
              classes.university += 1
            } else if (item.level == 'High School') {
              classes.high_school += 1
            }
            if (item.country && countries.indexOf(item.country) === -1) {
              countries.push(item.country)
              country_count++
            }
            if (item.organization_id && org_ids.indexOf(item.organization_id) === -1) {
              org_ids.push(item.organization_id)
              org_count++
            }
          })

          this.metric.organizations = org_count
          this.metric.countries = country_count
          this.metric.players = leaderboard.length
          this.classes = classes
        })
      }
    }
  }
</script>
