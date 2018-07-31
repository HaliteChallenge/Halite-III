<template>
    <div class="home-container">
        <div class="row">
            <div class="col-md-12">
                <p class="t1 c-wht font-headline">HALITE III ALPHA</p>
                <p class="d1 c-org font-headline">MAY THE BEST BOT WIN</p>
           <!-- <div v-if="!me_in" class="not-me ha-button-container">
                    <div class="hidden-xs hidden-sm">
                        <a class="ha-button" href="/learn-programming-challenge"><span>Get Started</span></a>
                    </div>
                    <div class="visible-xs visible-sm">
                        <a class="ha-button" href="https://api.halite.io/v1/login/github" onclick="javascript:handleOutboundLinkClicks('account', 'click-to-sign-up','navigation');return true;"><span>SIGN UP</span></a>
                    </div>
                </div> -->
            </div>
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
        </div>
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
            }
        }
        return {
            me_in,
            loginServerUrl: `${api.LOGIN_SERVER_URL}/github`,
            modalOpen: false,
        }
    },
     mounted: function () {
       this.createRedditWidget()
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
       }
     }
   }
</script>

<style lang="scss" scoped>

</style>
