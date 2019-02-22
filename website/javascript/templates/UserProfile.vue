<template>
    <div class="row">
        <div class="col-md-4">
            <div class="user-profile">
                <div class="bg-img"></div>
                <div class="user-profile-avatar">
                  <profile-image
                    className="img-responsive"
                    :username="user.username"
                    :profileImage="user.profile_image_key"
                  />
                </div>
                <div class="user-profile-detail">
                  <span class="user-name">{{ user.username }}</span>
                    <div v-if="user.team_id" class="user-team-detail">
                        <p class="user-team">
                            <template v-if="user.team_leader_id == user.user_id">
                                Leader of
                            </template>
                            <template v-else>
                                Member of
                            </template>
                            <a target="_blank"
                               :href="`/user/?user_id=${user.team_leader_id}`"
                            >{{ user.team_name }}</a>
                        </p>

                        <template v-if="team" v-for="member in team.members">

                            <profile-image
                              height="20px"
                              :username="member.username"
                              :profileImage="member.profile_image_key"
                            />
                            <a :href="`/user/?user_id=${member.user_id}`">
                                {{member.username}}
                            </a>
                        </template>
                        <i class="xline xline-bottom"></i>
                    </div>
                    <p>{{ user.level }} <span v-if="user.organization">at <a  :href="`/programming-competition-leaderboard?organization=${user.organization_id}`">{{ user.organization }}</a></span></p>
                    <p v-if="user.location">
                      From <a :href="`/programming-competition-leaderboard?country=${user.country_code}`">{{user.location}}</a>
                    </p>
                    <p v-if="botLang.length > 0">Bots in
                        <template v-for="(lang, index) in botLang">
                            <span v-if="lang.length > 0" class="hl"><a  :href="`/programming-competition-leaderboard?language=${lang}`">{{lang}}</a></span><span v-if="(index+1) < botLang.length">,</span>
                        </template>
                    </p>
                    <div v-if="is_my_page && bots && bots[0] && bots[0].compilation_status==='Disabled'" class="text-center" style="margin-top: 10px;">
                        <p class="warning">
                            Your bot is disabled <span title="When you join a team, any bots you personally uploaded before are disabled. They retain their rank, but can no longer play games or be updated." class="info-icon icon-info"></span>
                        </p>
                    </div>
                     <div v-if="(is_my_page || is_team_page) && bots && bots[0] && bots[0].compilation_status==='Failed'" class="text-center" style="margin-top: 10px;">
                        <p class="warning">
                          <a :href="compile_log" target="_blank"> Your bot failed to compile   <span title="Look at the compilation failure mail to debug the issue or try submitting it again." class="info-icon icon-info"></span></a>
                        </p>
                    </div>
                </div>
                <div class="user-profile-rank">
                    <div class="ranking-type">
                        <a v-if="user.rank"
                           class="individual"
                           target="_blank"
                           :title="`Show ${user.team_id ? user.team_name : user.username} on leaderboard`"
                           :href="`/programming-competition-leaderboard?show_user=${user.user_id}`">
                            <div class="lvl-icon" :class="tierClass(user.tier || 'Bronze')"></div>
                            <div>

                                <span v-if="team">
                                  <div class="type-title">Team</div>
                                </span>
                                <span v-else>
                                  <div class="type-title">Individual</div>
                                </span>
                                <div class="lvl">
                                    #{{user.rank}}
                                </div>
                            </div>
                        </a>
                        <div class="individual" v-else>
                            <div>
                                <div class="type-title">Individual</div>
                                <div class="lvl">
                                    No Rank
                                </div>
                            </div>
                        </div>
                        <div class="organization" v-if="organizationRank">
                          <div class="lvl-icon" :class="tierClass(organizationRank.tier || 'Bronze')"></div>
                          <div>
                            <div class="type-title">Organization</div>
                            <div class="lvl">
                              {{ organizationRank.organization_rank ? `#${organizationRank.organization_rank}` : "No Rank" }}
                            </div>
                          </div>
                        </div>
                        <div class="organization" v-else>
                          <div>
                            <div class="type-title">No Organization Rank</div>
                          </div>
                        </div>
                    </div>
                    <div class="user-profile-rank-stats">
                        <div class="stats-item">
                            <div class="title">Rating</div>
                            <p>{{ Math.round(user.score * 100) / 100 }}</p>
                            <p title="Rating = μ−3σ. See Analysis section on right." class="detailed-rating">μ={{user.mu.toFixed(2)}} σ={{user.sigma.toFixed(2)}}</p>
                        </div>
                        <div class="stats-item">
                            <div class="title">Bots</div>
                            <p>{{ user.num_submissions }}</p>
                        </div>
                        <div class="stats-item">
                            <div class="title">Games</div>
                            <p>{{ getFormattedNumber(user.num_games) }}</p>
                        </div>
                    </div>
                    <i class="xline xline-bottom"></i>
                    <!-- <h2 class="highest-rank" v-if="highestRank" title="This is either your current (top rank) or highest rank any of your bots had achieved when retired"> Highest Rank Achieved: {{highestRank}}</h2> -->

                </div>
                <div class="user-efficiency" v-if="efficiency">
                    <div class="title">Halite Efficiency (Last 10 Games)  <span title="Halite Efficiency is calculated as the percent of halite added to your stored halite / total halite you collect from the sea. This percent can exceed 100% when additional halite is gained in the inspired state." class="info-icon icon-info"></span></div>
                    <div class="chart-container">
                        <div class="data-distribution">
                            <div class="data-line" ref="dataLine"></div>
                            <!-- Use test data -->
                            <div class="scale tire-line" :style="getLeftLen(efficiency.min)">{{(efficiency.min * 100).toFixed(0)}}%</div>
                            <div class="scale user-line" :style="getLeftLen(efficiency.mean)">{{(efficiency.mean * 100).toFixed(1)}}%</div>
                            <div class="scale all-play-line" :style="getLeftLen(efficiency.max)">{{(efficiency.max * 100).toFixed(0)}}%</div>
                        </div>
                        <div class="data-name">
                            <div class="tire">Min</div>
                            <div class="user-name">Average</div>
                            <div class="all-play">Max</div>
                        </div>
                    </div>
                </div>
            </div>

            <div class="user-profile" v-if="season1stats">
              <div class="user-profile-detail">
                <div class="title">Halite 1 Stats</div>

                <p>{{ season1stats.level }} <span v-if="season1stats.organization">at <a :href="`https://2016.halite.io/leaderboard.php?field=organization&value=${season1stats.organization}&heading=${season1stats.organization}`">{{ season1stats.organization }}</a></span></p>
                <p v-if="season1stats.language">Bots in {{season1stats.language}}</p>
                <p v-else>No Bot Submitted</p>
              </div>

              <div class="past-year-stats" v-if="season1stats.language">
                <i class="xline xline-top"></i>
                <div class="stats-item">
                  <div class="title">Rating</div>
                  <p>{{ (season1stats.mu - 3*season1stats.sigma).toFixed(2) }}</p>
                </div>
                <div class="stats-item">
                  <div class="title">Bots</div>
                  <p>{{season1stats.num_submissions}}</p>
                </div>
                <div class="stats-item">
                  <div class="title">Games</div>
                  <p>{{season1stats.num_games}}</p>
                </div>
              </div>

              <div class="user-profile-rank" v-if="season1stats.language">
                <div class="ranking-type">
                  <div style="width: 100%; text-align: center;">
                    <div>
                      <div class="type-title">Individual</div>
                      <div class="lvl">
                        #{{season1stats.rank}}
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <p style="text-align: center; padding: 15px 0;">
                <a target="_blank"
                   :href="`https://2016.halite.io/user.php?userID=${season1stats.userID}`">
                  View Halite 1 Profile
                </a>
              </p>
            </div>

            <div class="user-profile" v-if="season2stats">
              <div class="user-profile-detail">
                <div class="title">Halite 2 Stats</div>

                <p>{{ season2stats.player_level }} <span v-if="season2stats.organization">at {{ season2stats.organization }}</span></p>
                <p v-if="season2stats.language">Bots in {{season2stats.language}}</p>
                <p v-else>No Bot Submitted</p>
              </div>

              <div class="past-year-stats" v-if="season2stats.language">
                <i class="xline xline-top"></i>
                <div class="stats-item">
                  <div class="title">Rating</div>
                  <p>{{season2stats.score === null ? '0' : season2stats.score.toFixed(2)}}</p>
                </div>
                <div class="stats-item">
                  <div class="title">Bots</div>
                  <p>{{season2stats.num_submissions}}</p>
                </div>
                <div class="stats-item">
                  <div class="title">Games</div>
                  <p>{{season2stats.num_games}}</p>
                </div>
              </div>

              <div class="user-profile-rank" v-if="season2stats.language">
                <div class="ranking-type">
                  <div style="width: 100%; text-align: center;">
                    <div>
                      <div class="type-title">Individual</div>
                      <div class="lvl">
                        #{{season2stats.rank}}
                      </div>
                    </div>
                  </div>
                </div>
              </div>

              <p style="text-align: center; padding: 15px 0;">
                <a target="_blank"
                   :href="`https://2017.halite.io/user/?user_id=${season2stats.user_id} `">
                  View Halite 2 Profile
                </a>
              </p>
            </div>
        </div>

        <div class="col-md-7">
            <div class="user-widget tab-container">
                <ul class="nav nav-tabs" role="tablist">
                    <li role="presentation" class="active">
                        <a href="#games" @click="refreshStickyTable" aria-controls="games" role="tab" data-toggle="tab">
                        <span>Games & Logs</span>
                        </a>
                    </li>
                    <li role="presentation">
                        <a href="#analysis" @click="analysisOpened" aria-controls="analysis" role="tab" data-toggle="tab">
                        <span>Analysis</span>
                        </a>
                    </li>
                </ul>
                <!-- Tab panes -->
                <div class="tab-content">
                    <div role="tabpanel" class="tab-pane active" id="games">
                        <div id="games_pane">
                            <section class="profile-section">
                                <h2>
                                    Game Videos Feed
                                </h2>
                                <div v-if="!games.length" class="section-empty">
                                    <img :src="`${baseUrl}/assets/images/no_challenges.png`" class="icon-"></img>
                                    <h2>No games played yet</h2>
                                    <p v-if="is_my_page">Complete your first game and view replays. <br/> <a href="/play-programming-challenge">Play here</a></p>
                                </div>
                                <div v-if="games.length">
                                    <table class="table table-leader">
                                        <thead>
                                            <tr>
                                                <th class="little-pd">Watch</th>
                                                <th class="large-pd">Result</th>
                                                <th class="text-center hidden-xs" >Map Size</th>
                                                <th class="text-center hidden-xs">Turns</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr v-for="game in games">
                                                <td class="little-pd watch" v-bind:class="game.versions_back ? (game.versions_back % 2 ? 'old-bot-odd' : 'old-bot-even') : ''">
                                                    <a :href="'/play?game_id=' + game.game_id">
                                                        <span class="video-icon"></span>
                                                        {{getFormattedDateForGames(game.time_played)}}
                                                    </a>
                                                </td>
                                                <td class="large-pd td-title" v-bind:class="{ 'challenge': game.challenge_id }">
                                                    <div class="info-icon-trophy" v-if="(game.players[user.user_id] || game.players[team.leader_id]).rank === 1">
                                                        <span class="tropy-icon"></span>
                                                    </div>
                                                    <a v-for="player in game.playerSorted"
                                                    :href="'/user?user_id=' + player.id"
                                                    class="game-participant"
                                                    :title="player.name_rank + (player.timed_out ? ' timed out or errored in this game. See the log for details.' : '')">
                                                      <profile-image
                                                        :username="player.username"
                                                        :profileImage="player.profile_image_key"
                                                        v-bind:className="{ 'timed-out': player.timed_out }"
                                                      />
                                                        <span class="rank">
                                                            {{ player.rank }}
                                                        </span>
                                                    </a>
                                                </td>
                                                <td class="text-center hidden-xs">{{ game.map_width }}x{{ game.map_height }}</td>
                                                <td class="text-center hidden-xs">
                                                {{ game.turns_total }}
                                                </td>
                                            </tr>
                                        </tbody>
                                    </table>
                                </div>
                                <i class="xline xline-bottom"></i>

                            </section>
                        </div>
                    </div>
                    <div role="tabpanel" class="tab-pane" id="analysis">
                        <div id="map_stats_pane">
                            <section class="profile-section">
                                <h2>
                                    Rating Analysis
                                </h2>
                                <div v-if="!user.mu" class="section-empty">
                                    <img :src="`${baseUrl}/assets/images/no_challenges.png`" class="icon-"></img>
                                    <h2>No rating analysis</h2>
                                    <p v-if="is_my_page">Submit your first bot to get your rating. <br/> <a href="/play-programming-challenge">Play here</a></p>
                                </div>
                                <div v-if="user.mu" class="user-profile-rank-stats">
                                    <div class="stats-item">
                                        <h3>Rating</h3>
                                        <p>{{ user.score.toFixed(2) }}</p>
                                    </div>
                                    <div class="stats-item">
                                        <h3>&mu;</h3>
                                        <p>{{ user.mu.toFixed(2) }}</p>
                                    </div>
                                    <div class="stats-item">
                                        <h3>&sigma;</h3>
                                        <p>{{ user.sigma.toFixed(2) }}</p>
                                    </div>
                                </div>
                                <p class="explanation">
                                    Rating is calculated as mu - 3 * sigma. Learn more about how ratings are calculated <a style="color:#ffbe00" href="https://forums.halite.io/t/how-ratings-get-calculated-for-halite-iii/95">here</a>.
                                </p>
                                <i class="xline xline-bottom"></i>
                            </section>
                            <section class="profile-section">
                                <h2>
                                    History
                                    <!-- <span title="Rank/Rating history of your bots, the rank/rating is the last rating or rank achieved before the bot was retired." class="info-icon icon-info pull-right"></span> -->
                                </h2>
                                <span>{{messages.bot_download}}</span>

                                <!-- <div v-if="!userHistory.length" class="section-empty">
                                    <img :src="`${baseUrl}/assets/images/leaderboard-zero-icon.png`" class="icon-"></img>
                                    <h2>No history</h2>
                                    <p v-if="is_my_page">Submit your first bot to see your history.<br/> <a href="/play-programming-challenge">Play here</a></p>
                                </div> -->
                                <div v-if="userHistory.length > 0">
                                    <div class="table-sticky-container">
                                        <div class="table-wrapper">
                                            <table class="table table-leader table-sticky high-index">
                                                <thead>
                                                    <tr>
                                                        <th class="little-pd">Bot Version</th>
                                                        <th class="text-center">Rating</th>
                                                        <th class="text-center">Rank</th>
                                                        <th class="text-center hidden-xs">Games</th>
                                                        <th class="hidden-xs little-pd">Retired On</th>
                                                    </tr>
                                                </thead>
                                            </table>
                                            <div class="table-scrollable-content">
                                                <table class="table table-leader">
                                                    <thead>
                                                        <tr>
                                                            <th class="little-pd">Bot Version</th>
                                                            <th class="text-center">Rating</th>
                                                            <th class="text-center">Rank</th>
                                                            <th class="text-center hidden-xs">Games</th>
                                                            <th class="hidden-xs little-pd">Retired On</th>
                                                        </tr>
                                                    </thead>
                                                    <tbody>
                                                        <tr v-for="historyItem in userHistory">
                                                            <td class="little-pd">
                                                                {{historyItem.bot_version}}
                                                            </td>
                                                            <td class="text-center">
                                                                {{ Math.round(100 * historyItem.last_score) / 100 }}
                                                            </td>
                                                            <td class="text-center">
                                                                {{historyItem.last_rank}}
                                                            </td>
                                                            <td class="text-center hidden-xs">
                                                                {{historyItem.last_games_played}}
                                                            </td>
                                                            <td class="hidden-xs little-pd">
                                                                {{getFormattedDateForGames(historyItem.when_retired, "Still Playing")}}
                                                            </td>
                                                        </tr>
                                                    </tbody>
                                                </table>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <br>
                                <button
                                    type="button"
                                    class="btn"
                                    v-if="is_my_page"
                                    v-on:click="download_bot">
                                    <span>Download Current Bot</span>
                                </button>
                            </section>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
    import * as api from '../api'
    import {Alert, tierClass} from '../utils.js'
    import Vue from 'vue'
    import * as utils from '../utils'
    import moment from 'moment'
    import numeral from 'numeral'
    import dateformat from 'dateformat'

    export default {
      name: 'UserProfile',
      props: ['baseUrl'],
      data: function () {
        return {
          tierClass: tierClass,
          organizationRank: null,
          user: {
            'level': '',
            'username': '',
            'organization': '',
            'points': '',
            'num_games': '',
            'user_id': ''
          },
          team: null,
          games: [],
          challengeGames: [],
          nemesisList: [],
          bots: [],
          error_games: [],
          userHistory: [],
          usernames: {},
          page: 0,
          limit: 10,
          offset: 0,
          nemesisLimit: 30,
          nemesisGameCount: 200,
          nemesisGameThreshold: 10,
          only_timed_out: false,
          is_my_page: false,
          is_team_page: false,
          highestRank: null,
          sharePopup: false,
          season1stats: null,
          season2stats: null,
          messages: {
            bot_download: ''
          },
          participants: {},
          isLastPage: false,
          efficiency: {
            min: 0,
            percentile25: 0,
            mean: 0,
            percentile75: 0,
            max: 0,
          },
        }
      },
      mounted: function () {
        const params = new URLSearchParams(window.location.search)
        let source

        if (params.has('me')) {
          source = api.me()
        } else {
          const user_id = params.get('user_id')
          source = api.get_user(user_id)
        }

        source.then((user) => {
          if (user === null) {
            window.location.replace(`/login`)
            return
          }
          this.user = user
          this.user.location = this.getLocation()

          if (params.has('me')) {
            window.history.replaceState(
              {}, '',
              `${window.location.origin}${window.location.pathname}?user_id=${user.user_id}`)
          }

          const titleEl = document.querySelector('title')
          titleEl.innerText = titleEl.innerText.replace('User Profile', `User Profile - ${this.user.username}`)

          if (user.team_id) {
            api.get_team(user.team_id).then((team) => {
              this.team = team
            })
          }

          if (user.organization_id) {
            api.organizationLeaderboard([`organization_id,=,${user.organization_id}`]).then((org) => {
              this.organizationRank = org[0]
            })
          }

          this.fetch().then((games) => {
            if (games.length === 0) return
            const efficiencies = []
            for (const game of games) {
              let index = -1
              for (const player of Object.values(game.players)) {
                if (parseInt(player.id, 10) === user.user_id) {
                  index = player.player_index
                  break
                }
              }

              if (index >= 0) {
                efficiencies.push(game.stats.player_statistics[index].mining_efficiency)
              }
            }
            efficiencies.sort()
            this.efficiency.min = efficiencies[0]
            this.efficiency.max = efficiencies[efficiencies.length - 1]
            this.efficiency.mean = efficiencies.reduce((a, b) => a + b, 0) / efficiencies.length
          })

          this.fetchhistory()
          this.fetchHalite1Stats()

          // switch to analysis tab if requested
          const url = new URLSearchParams(location.search)
          const view = url.get("view")
          console.log(view)
          if (view == 'analysis'){
            $('a[href="#analysis"]').trigger('click')
            this.setupStickyTable()
          }
        }, (e) => {
          window.location.replace(`${this.baseUrl}/404`);
        })

        api.me().then((me) => {
          this.is_my_page = me && me.user_id === this.user.user_id
        })

        // sticky tables
        this.setupStickyTable()
      },
      computed: {
        compile_log() {
          return `${api.API_SERVER_URL}/user/${this.user.user_id}/bot/${this.bots[0].bot_id}/error_log`
        },
        botLang: function () {
          let lang = []
          if (this.bots.length > 0) {
            for (let i = 0; i < this.bots.length; i++) {
              if (lang.indexOf(this.bots[i].language) == -1) {
                lang.push(this.bots[i].language)
              }
            }
          }
          return lang
        },
      },
      methods: {
        setupStickyTable: function () {
          $(window).on('resize', () => {
            calcCol()
          })
        },
        refreshStickyTable: function () {
            window.refreshStickyTable();
        },
        analysisOpened() {
          this.refreshStickyTable()
        },
        fetch: function (options={}) {
          return api.get_user_matches(this.user.user_id).then((data) => {
            let games = [];
            if ( data.length > 0 ){
              for (let game of data) {
                for (let player_id of Object.keys(game.players)) {
                  let player = game.players[player_id]
                  let username = player.username
                  let rating = player.mu - (player.sigma * 3)
                  player.rating = rating
                  rating = Math.round(rating * 100) / 100
                  let mu = Math.round(player.mu * 100) / 100
                  let sigma = Math.round(player.sigma * 1000) / 1000

                  player.id = player_id
                  player.name_rank = `(${player.leaderboard_rank}) ${username} [${rating}=${mu}μ${sigma}σ]`

                  this.usernames[player_id] = username

                  if (player_id == this.user.user_id) {
                    game.versions_back = this.user.num_submissions - player.version_number
                  }
                }

                const players = Object.values(game.players).sort((r1, r2) => {
                  if (r1.id.toString() === this.user.user_id.toString()) { return -1 }
                  if (r2.id.toString() === this.user.user_id.toString()) { return 1 }
                  return r1.rank - r2.rank
                })

                game.playerSorted = players
                games.push(game)
              }
            }
            this.games = games;
            return games
          });
        },
        getLocation: function () {
          const user = this.user
          let state = '', country = ''
          const countries = require('i18n-iso-countries')

          if (user.country_code) {
            const countryAlpha2 = countries.alpha3ToAlpha2(user.country_code)
            const countryData = iso3166.data[countryAlpha2]
            let stateData
            if (countryData && user.country_subdivision_code) {
              stateData = countryData.sub[user.country_subdivision_code]
            }
            state = stateData ? stateData.name : ''
            country = countryData ? countryData.name : ''
          }
          const location = `${state ? state + ', ' : ''}${country}`
          return location || ''
        },
        fetchHalite1Stats: function () {
          this.season1stats = this.user.season1
          this.season2stats = this.user.season2
        },
        fetchhistory: function () {
          const history = this.user.bot_history
          if (history && history instanceof Array) {
            history.sort(function (a, b) { return parseInt(b.bot_version) - parseInt(a.bot_version) })
            this.userHistory = history
            if (this.user.num_submissions > 0) {
              this.userHistory.unshift({bot_version: 'Current (' + this.user.num_submissions + ')', last_score: this.user.score, last_rank: this.user.rank, last_games_played: this.user.num_games, when_retired: 'Still playing' })
            }
            if (history.length <= 0) {
              return
            }
            this.highestRank = history.reduce((min, p) => p.last_rank < min ? p.last_rank : min, history[0].last_rank)
            if (this.highestRank > this.user.rank) {
              this.highestRank = this.user.rank
            }
          }
        },
        replay_download_link: function (game_id) {
          return `${api.API_SERVER_URL}/user/${this.user.user_id}/match/${game_id}/replay`
        },
        replay_link: function (game_id) {
          return `/play/?game_id=${game_id}`
        },
        download_bot() {
          api.get_bot_zip(this.user.user_id, 0).then((blob) => {
            console.log(blob)
            if (blob.type === "application/zip") {
              const url = URL.createObjectURL(blob)
              const anchor = document.createElement("a")
              anchor.href = url
              anchor.setAttribute("download", `${this.user.user_id}.zip`)
              anchor.style.display = "none"
              document.body.appendChild(anchor)
              window.setTimeout(() => {
                anchor.click()
                document.body.removeChild(anchor)
                window.setTimeout(() => URL.revokeObjectURL(url), 500)
              }, 1000)
            }
            else {
              this.messages.bot_download = "No bot uploaded, cannot download bot."
            }
          })
        },
        prev_badge: () => {
          let content = $('.user-profile-badge-content')
          let list = $('.user-profile-badge-list')
          let contentWidth = $(content).width()
          let listWidth = $(list).children('li').outerWidth(true) * $(list).children('li').length
          let marginLeft = parseInt($(list).css('marginLeft'))
          let interval = 20
          let aniVal = 0
          let cal = listWidth + marginLeft - contentWidth
          if (cal > interval) {
            aniVal = interval
          } else if (cal > 0 <= interval) {
            aniVal = cal
          } else {
            aniVal = 0
          }
          $(list).animate({marginLeft: '-=' + aniVal + 'px'})
        },
        next_badge: () => {
          let list = $('.user-profile-badge-list')
          let marginLeft = Math.abs(parseInt($(list).css('marginLeft')))
          let interval = 20
          let aniVal = 0
          if (marginLeft > interval) {
            aniVal = interval
          } else if (marginLeft > 0 <= interval) {
            aniVal = marginLeft
          } else {
            aniVal = 0
          }
          $(list).animate({marginLeft: '+=' + aniVal + 'px'})
        },
        getFormattedDateForGames: function (date, return_value_not_valid) {
          var cdate = moment(date)
          if (cdate.isValid()) {
            var dateFormat = require('dateformat')
            return dateFormat(date, 'dd/mm/yy HH:MM')
          } else {
            return return_value_not_valid
          }
        },
        gaData: function (category, action, label) {
          utils.gaEvent(category, action, label)
        },
        getFormattedNumber: function (number) {
         return numeral(number).format('0 a');
        },
        sortChallenge: function(players) {
          return _.sortBy(players, (player) => -player.points)
        },
        /**
         * @param  {e} event
         * @return {void}
         */
        copyToClipboard: function (e) {
          if (e) e.preventDefault()
          this.$refs.shareInput.select()
          document.execCommand('copy')
        },
        // Get the location of the data by percentage --- issue #397
        getLeftLen(data) {
            data = Math.max(0, Math.min(1.5, data))
            return { left: `calc(${(data / 1.5) * 100}% - 12px)` }
        }
      }
    }
</script>

<style lang="scss" scoped>
    .form-inline{
        .btn-ha{
            display: inline-block;
        }
    }
    .game-participant {
        img {
            height: 20px;
        }
    }
    .user-profile-rank-stats {
        .stats-item {
            margin-top: 0;
            height: auto !important;
        }
    }

    .stats-item {
      position: relative;
    }

    .detailed-rating {
      font-size: 0.8em !important;
      position: absolute;
      margin-top: -0.75em !important;
      left: 50%;
      width: 150%;
      transform: translateX(-50%);
      cursor: help;
    }

    .challenge-first-cell {
      max-width: 10em;
    }
</style>
<!--
     Local Variables:
     web-mode-markup-indent-offset: 4
     End:
     End: -->
