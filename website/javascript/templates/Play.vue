<template>
  <div class="play-container">
    <div class="container-fluid" v-if="currentView == 'upload'">
      <h1 class="page-heading">Play Halite III</h1>
      <div class="doc-section doc-section-play text-center">
        <p><img src="/assets/images/icon-flag.svg" width="30" alt="flag"></p>
        <h4 class="mt3">COMPETITION INFORMATION</h4>
        <p class="sub-title">October 16, 2018 - January 22, 2019</p>
        <p>Halite III bots can be developed locally or in a web-based editor. Submit your bot via one of the below options to participate in Halite III.</p>
      </div>
      <div class="col-sm-6">
        <div class="doc-section doc-section-play text-center">
          <h4 class="mt3">WEB-BASED DEVELOPMENT</h4>
          <p>Create, replay, and submit a Python 3.6, Java, or C++ bot via a web-based editor.</p>
          <br>
          <a class="btn btn-primary" href="/editor">Go to Editor</a>
        </div>
      </div>
      <div class="col-sm-6">
        <div class="doc-section doc-section-play text-center">
          <h4 class="mt3">LOCAL DEVELOPMENT</h4>
    <!--  <p class="sub-title">Submit your bot here</p> -->
          <p>Or, <a href="/learn-programming-challenge/downloads">download</a> the game environment and starter kit bundle for your platform and language of choice.</p>
          <p>To submit your local Halite bot in the competition, upload a .zip file here. The root of your zip should contain a MyBot.{extension} file and the /hlt folder from the starter kit.</p>
          <halite-upload-zone
            description="Select File or Drop File to <br> Upload your Bot"
            buttonText = "Select File"
            :icon="`/assets/images/icon-upload.svg`"
            v-on:change="upload_bot"
            :progressBar="is_downloading"
            :progress="uploadProgress"
            :message="uploadMessage">
            </halite-upload-zone>
        </div>
      </div>
    </div>
    <div id="halite-uploaded-bot" v-if="currentView=='botUpload'">
      <bot-upload ref="botUploadComponent" :user="user" :bot-file="botFile" :bots-list="botsList"  v-if="currentView='botUpload'"
      :showMessage="showMessage"></bot-upload>
    </div>

  </div>
</template>
<script>
  import * as api from '../api'
  import Vue from 'vue'
  import HaliteBreadcrumb from './Breadcrumb.vue'
  import VisualizerContainer from './VisualizerContainer.vue'
  import Upload from './Upload.vue'
  import BotUpload from './BotUpload.vue'
  import Message from './Message.vue'
  import {Alert} from '../utils.js'
  import UploadZone from './UploadZone.vue'
  import Visualizer from './Visualizer.vue'
  import * as utils from '../utils'

  // showing game
  let visualizer = null
  const showGame = (game) => {
  if (visualizer && visualizer.getVisualizer) {
    visualizer.getVisualizer().destroy()
  }

  const buffer = game.replay
  return import(/* webpackChunkName: "libhaliteviz" */ "libhaliteviz")
    .then((libhaliteviz) => {
      // just for electron
      if (window && window.process && window.process.type) {
        return libhaliteviz.setAssetRoot('assets/js/').then(() => libhaliteviz)
      }
      return libhaliteviz.setAssetRoot('').then(() => libhaliteviz)
    }).then((libhaliteviz) => {
      return libhaliteviz.parseReplay(buffer).then((replay) => {
        let outerContainer = document.querySelector('.play-container > .container-fluid')
        outerContainer.innerHTML = ''

        let container = document.createElement('div')
        outerContainer.appendChild(container)

        new Vue({
          el: container,
          render: (h) => h(Visualizer, {
            props: {
              replay: Object.freeze(replay),
              game: game.game,
              makeUserLink: function (user_id) {
                return `/user?user_id=${user_id}`
              },
              getUserProfileImage: function (user_id) {
                return api.get_user(user_id).then((user) => {
                  return api.make_profile_image_url(user.username)
                })
              }
            }
          }),
          mounted: function () {
            window.scrollTo(0, 0);
            visualizer = this.$children[0]
          }
        })
      })
    });
}

  export default {
    name: 'uploader',
    props: ['baseUrl'],
    components: {
      'Upload': Upload,
      'bot-upload': BotUpload,
      'visualizer-container': VisualizerContainer,
      'halite-upload-zone': UploadZone,
      HaliteBreadcrumb
    },
    data: function () {
      return {
        currentView: 'upload',
        replayFile: '',
        botFile: {name: ''},
        loggedIn: false,
        user: null,
        botsList: [],
        displayMessage: false,
        message: '',
        is_downloading: false,
        uploadProgress: null,
        uploadMessage: null,
        path: [
          {
            name: 'Back',
            link: '/play-programming-challenge'
          }
        ]
      }
    },
    mounted: function () {
      // logged in
      api.me().then((me) => {
        if (me !== null) {
          this.loggedIn = true
          this.user = me
          api.list_bots(me.user_id).then((bots) => {
            this.botsList = bots
          })
        }
      })

      // handle whole page drag and drop
      const ins = this
      $('body').on('drop dragdrop', (e) => {
        // verify if the dropzone is not the bot uploader zone
        const files = e.originalEvent.dataTransfer.files
        if (files.length > 0) {
          e.preventDefault()

          const [f] = files;
          if (f) { // f.type === "application/zip"
            Vue.set(ins, 'botFile', f)
            Vue.set(ins, 'currentView', 'botUpload')
            Vue.nextTick(() => {
              if (ins.$refs.botUploadComponent) {
                ins.$refs.botUploadComponent.validateBot()
              }
            })
          }
        }
      })
      $('body').on('dragenter', (e) => {
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')
        if (!container || !container.contains(e.target)) {
          event.preventDefault()
        }
      })
      $('body').on('dragover', (e) => {
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')
        if (!container || !container.contains(e.target)) {
          event.preventDefault()
        }
      });
  },
    methods: {
      showMessage: function (type = 'success', content) {
        if (content === null) {
          Alert.hide()
        }
        else {
          Alert.show(content, type)
        }
      },
      play_replay: function (files) {
        this.gaData('play', 'select-replay-file-another', 'replay-flow')
        if (files.length > 0) {
          const reader = new FileReader()
          const inst = this
          reader.onload = (e) => {
            inst.is_upload = false
            inst.currentView = 'replay'
            inst.replayFile = files[0].name
            window.location.hash = '/replay-bot'
            // inst.message = 'Parsing replay, please wait…'
            inst.message = 'Almost done...'
            showGame({
              game: null,
              replay: e.target.result
            }).then(() => {
              this.message = null
            }).catch(() => {
              this.message = 'There was an error parsing the replay. Please let us know at halite@halite.io.'
            })
          }
          reader.readAsArrayBuffer(files[0])
        }
      },
      gaData: function (category, action, label) {
        utils.gaEvent(category, action, label)
      },
      upload_bot(files) {
        window.scrollTo(0, 0)
        if (files.length && files[0].type === "application/zip") {
          this.botFile = files[0]
          this.currentView = 'botUpload'
        } else {
          Alert.show('You may only upload a zip file.', 'error')
        }
      },
    }
  }
</script>

<style lang="scss" scoped>
  $primary-blue-text: #033C89;
  $primary-black-text: #414141;
  $text-content-color: #52678D;
  .mt3 {
    color: $primary-blue-text;
    font-size: 24px;
    line-height: 29px;
  }
  .sub-title {
    color: $primary-black-text;
    margin-top: 10px;
    margin-bottom: 25px;
    font-size: 20px;
  }
  .text-content {
    color: $text-content-color;
    font-size: 18px;
    line-height: 27px;
    letter-spacing: 0.12px;
  }
</style>
