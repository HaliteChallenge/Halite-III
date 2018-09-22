<template>
    <div class="home-container">
        <div class="personal-msg" v-if="user">
            <div class="left-container">
                <div class="top-bg"></div>
                <div class="title">Hi {{ user.username }}! </div>
                <div class="text-tips">
                    Let’s build a bot and get on the leaderboard!<br/>
                    Check out our documentation and interactive tutorials.
                </div>
                <a class="btn btn-primary btn-sm" href="/learn-programming-challenge/">Play now</a>
            </div>
            <div class="right-container">
                <div class="data-item">
                    <div class="item-icon"></div>
                    <div class="item-info">
                        <div class="item-title">Bot Version</div>
                        <div class="item-data">{{ user.num_submissions || "N/A" }}</div>
                    </div>
                </div>
                <div class="data-item" v-if="organization_rank">
                    <div class="item-icon"></div>
                    <div class="item-info">
                        <div class="item-title">Organization</div>
                        <div class="item-data">#{{organization_rank.organization_rank}} over past week</div>
                    </div>
                </div>
                <div class="data-item" v-else>
                    <div class="item-icon"></div>
                    <div class="item-info">
                        <div class="item-title">Organization</div>
                        <div class="item-data">Not Affiliated</div>
                    </div>
                </div>
                <div class="data-item">
                    <div class="item-icon"></div>
                    <div class="item-info">
                        <div class="item-title">Global Rank</div>
                        <div class="item-data">{{ user.rank || "N/A" }}</div>
                    </div>
                </div>
            </div>
        </div>
        <div class="row section-overview">
            <div class="col-md-6">
                <h1>Welcome to Halite III!</h1>
                <p>Halite is an open source artificial intelligence challenge, created by Two Sigma.</p>
                <p>Halite III is a resource management game. Your goal is to build a bot that efficiently navigates the seas removing halite, a luminous energy resource.</p>
                <br>
                <a class="btn btn-primary" href="/learn-programming-challenge/">Play now</a>
            </div>
            <div class="col-md-6">
                <img src="/assets/images/home-map.png" alt="Map">
            </div>
        </div>
<!--        <div class="row section-feed">
            <div class="col-md-4">
                <div class="feed-block">
                    <div class="feed-header">
                        <img src="/assets/images/icon-trophy.svg" alt="trophy" class="feed-icon">
                        <h3>Competition Updates</h3>
                    </div>
                    <div class="feed-body">
                        <article class="feed-item">
                            <h3>5 Tips for this year's Halite competition</h3>
                            <p>The Two Sigma Investments, LP Deep Learning team worked on a new write-up about </p>
                            <div class="feed-item-bottom">
                                <span class="date">
                                    <span class="icon-date"></span>
                                    11 Aug 2018
                                </span>
                                <a class="read-more" href="#">
                                    Read more
                                    <span class="icon-readmore"></span>
                                </a>
                            </div>
                        </article>
                        <article class="feed-item">
                            <h3>5 Tips for this year's Halite competition</h3>
                            <p>The Two Sigma Investments, LP Deep Learning team worked on a new write-up about </p>
                            <div class="feed-item-bottom">
                                <span class="date">
                                    <span class="icon-date"></span>
                                    11 Aug 2018
                                </span>
                                <a class="read-more" href="#">
                                    Read more
                                    <span class="icon-readmore"></span>
                                </a>
                            </div>
                        </article>
                    </div>
                </div>
            </div>
            <div class="col-md-4">

                <div class="feed-block">
                    <div class="feed-header">
                        <img src="/assets/images/icon-medal.svg" alt="medal" class="feed-icon">
                        <h3>In The Press</h3>
                    </div>
                    <div class="feed-body">
                        <article class="feed-item">
                            <h3>5 Tips for this year's Halite competition</h3>
                            <p>The Two Sigma Investments, LP Deep Learning team worked on a new write-up about </p>
                            <div class="feed-item-bottom">
                                <span class="date">
                                    <span class="icon-date"></span>
                                    11 Aug 2018
                                </span>
                                <a class="read-more" href="#">
                                    Read more
                                    <span class="icon-readmore"></span>
                                </a>
                            </div>
                        </article>
                        <article class="feed-item">
                            <h3>5 Tips for this year's Halite competition</h3>
                            <p>The Two Sigma Investments, LP Deep Learning team worked on a new write-up about </p>
                            <div class="feed-item-bottom">
                                <span class="date">
                                    <span class="icon-date"></span>
                                    11 Aug 2018
                                </span>
                                <a class="read-more" href="#">
                                    Read more
                                    <span class="icon-readmore"></span>
                                </a>
                            </div>
                        </article>
                    </div>
                </div>
            </div>
            <div class="col-md-4">
                <div class="parnter-block">
                    <h3>Partners</h3>
                    <div class="partner-main">
                        <div class="logo">
                            <img src="/assets/images/logo-two-sigma.png" alt="two sigma">
                        </div>
                        <div class="name">Two <br>Sigma</div>
                    </div>
                    <div class="partners">
                        <div class="partner">
                            <div class="logo">
                                <img src="/assets/images/logo-google-cloud.png" alt="google cloud">
                            </div>
                            <div class="name">
                                Google Cloud
                            </div>
                        </div>
                        <div class="partner">
                            <div class="logo">
                                <img src="/assets/images/logo-university.png" alt="university">
                            </div>
                            <div class="name">
                                University (TBD)
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div> -->

            <!--
            <div class="row">
                <div class="col-md-12">
                <p class="t1 c-wht font-headline">HALITE III ALPHA</p>
                <p class="d1 c-org font-headline">MAY THE BEST BOT WIN</p> -->
           <!-- <div v-if="!me_in" class="not-me ha-button-container">
                    <div class="hidden-xs hidden-sm">
                        <a class="ha-button" href="/learn-programming-challenge"><span>Get Started</span></a>
                    </div>
                    <div class="visible-xs visible-sm">
                        <a class="ha-button" href="https://api.halite.io/v1/login/github" onclick="javascript:handleOutboundLinkClicks('account', 'click-to-sign-up','navigation');return true;"><span>SIGN UP</span></a>
                    </div>
                </div> -->
            <!-- </div>
            <div class="col-md-12 des">
                <div class="col-md-offset-2 col-md-4 bot-1">
                    <img :src="`${baseUrl}/assets/images/temp/bot_2.png`"/>
                </div>
                <div class="words col-md-offset-1 col-md-4">
                    <p class="t3 c-wht font-headline">SAIL THE HIGH SEAS</p>
                    <div class="line-container"><i class="xline xline-top"></i></div>
                    <p class="t4 c-gry">Halite III is an exploratory resource management game, in which players build bots that trawl the seas for halite, a sparkling energy resource.</p>
                    <p class="t4 c-gry">In this crucial alpha stage, we are looking for feedback on game usability, mechanics, and strategic depth. Help us test Halite III in advance of our global competition launch in October!</p>
                    <div class="line-container"><i class="xline xline-top"></i></div>
                    <div class="ha-button-container no-bg-button">
                        <div>
                            <a class="ha-button" href="/learn-programming-challenge"><span>GET STARTED</span></a>
                        </div>
                    </div>
                </div>
                <div class="col-md-offset-2 col-md-4 bot-2">
                    <img :src="`${baseUrl}/assets/images/temp/bot_1.png`"/>
                </div>
           </div>
            <div class="col-md-12 ha-line">
            </div>
        </div>-->
    </div>
</template>

<script>
  import Vue from 'vue'
  import * as api from '../api'
  import * as utils from '../utils'
  import ChallengeModal from './ChallengeModal.vue'

  export default {
     name: 'home',
     props: ['baseUrl'],
     components: {ChallengeModal},
     data: function () {
        const me = api.me_cached()
        let me_in = false
        if (me) {
            return {
                me_in: true,
                modalOpen: false,
                user: null,
                organization_rank: null
            }
        }
        return {
            me_in,
            loginServerUrl: `${api.LOGIN_SERVER_URL}/github`,
            modalOpen: false,
            organization_rank: null
        }
    },
     mounted: function () {
        this.createRedditWidget()
        // Get user information
        this.fetchUserInfo();
     },
     methods: {
       invite: function () {
         this.gaData('invite', 'click-to-invite', 'home')
         if (!document.getElementById('intmpid').checkValidity()) {
           document.getElementById('invitestatus').style.color = 'red'
           document.getElementById('invitestatus').textContent = 'Invalid email..'
           this.gaData('invite-error', 'click-to-invite', 'home')
         } else {
           let content = document.getElementById('intmpid').value
           api.invite_friend(content).then(status => {
             document.getElementById('invitestatus').style.color = 'green'
             document.getElementById('invitestatus').textContent = 'Invitation sent...'
             this.gaData('invite-success', 'click-to-invite', 'home')
           })
         }
       },
       createRedditWidget: function () {
         let s = document.createElement('script'),
           content = ''

         s.src = 'https://www.reddit.com/domain/halite.io/hot/.embed?limit=5&t=all'

         document.write = function (s) {
           content += s
         }

         s.onload = function () {
           document.getElementById('redditWidget').innerHTML = content
         }

         document.getElementsByTagName('head')[0].appendChild(s)
       },
       gaData: function (category, action, label) {
         utils.gaEvent(category, action, label)
       },
       openChallengeModal: function(){
        this.modalOpen = true
       },
       closeChallengeModal: function(){
        this.modalOpen = false;
       },
       // Get user information
       fetchUserInfo: function() {
         api.me().then((user) => {
           if (user) {
             this.user = user

             if (user.organization_id === null) {
               this.organization_rank = null
               return
             }
             const filter = [`organization_id,=,${user.organization_id}`]
             api.organizationLeaderboard(filter).then((org) => {
               if (org && org.length > 0)
                 this.organization_rank = org[0]
             })
           }
         })
         // Test Code --- Because my own account has no data.
         // api.get_user(1).then(res => {
         //     this.user = res;
         // });
       },
     }
   }
</script>

<style lang="scss" scoped>
    .btn.btn-primary {
        &:after {
            content: "";
            display: inline-block;
            margin-left: 10px;
            width: 12px;
            height: 18px;
            background: url("/assets/images/icons/pixelate-arrow.png") center center no-repeat;
            background-size: cover;
        }
    }
</style>
