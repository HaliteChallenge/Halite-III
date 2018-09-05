<template>
  <div class="play-container">

    <!--
    <div class="play-navbar only-desktop">
      <ul>
        <li class="active">
          <a href="#">Competition Overview</a>
        </li>
        <li>
          <a href="#">Current Ranking</a>
        </li>
        <li>
          <a href="#">Watch Games</a>
        </li>
      </ul>
    </div>

    <div class="responsive-toc-nav only-mobile">
      <span class="current-nav-item" data-toggle="collapse" data-target="#res_nav_selector">
        Competition Overview
      </span>
      <ul id="res_nav_selector" class="select-nav collapse" aria-expanded="false">
        <li><a href="#">Competition Overview</a></li>
        <li><a href="#">Current Ranking</a></li>
        <li><a href="#">Watch Games</a></li>
      </ul>
    </div>
    -->


    <div class="container-fluid">
      <h1 class="page-heading">Competition summary</h1>
      <div class="doc-section doc-section-play text-center">
        <p><img src="/assets/images/icon-flag.svg" width="30" alt="flag"></p>
        <h4 class="mt3">SEASON DURATION</h4>
        <p class="sub-title">1st sep 2018 - 2nd feb 2019</p>
        <p class="text-content">Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque sem ante, tincidunt in augue eu, sodales pellentesque massa. Cras congue lectus quis risus pellentesque, quis aliquam purus sollicitudin. Maecenas feugiat lorem erat, eget ornare elit volutpat eget. </p>
      </div>
      <div class="doc-section doc-section-play text-center">
        <h4 class="mt3">DROP-OFF AREA</h4>
        <p class="sub-title">Submit your bot here</p>
        <halite-upload-zone
          description="Drop a replay file here to <br>upload your bot"
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


        <!-- <halite-upload-zone
          title="Replay a File"
          description="Drop a replay file here to upload"
          buttonText = "Select File"
          :icon="`${baseUrl}/assets/images/icon-replay.svg`"
          v-on:change="play_replay"
          :progressBar="is_downloading"
          :progress="uploadProgress"
          :message="uploadMessage">
        </halite-upload-zone> -->
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
        libhaliteviz.setAssetRoot('assets/js/')
      } else {
        libhaliteviz.setAssetRoot('')
      }
      return libhaliteviz;
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
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')

        // verify if the dropzone is not the bot uploader zone
        const files = e.originalEvent.dataTransfer.files
        if ((!container || !container.contains(e.target)) && files.length > 0) {
          e.preventDefault()

          const [f] = files;
          if (f.type === "application/zip") {
            // TODO: upload
          }
          else {
            /* let outerContainer = document.getElementById('halitetv-visualizer') */
            /* outerContainer.innerHTML = '' */
            ins.play_replay(files)
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
        Alert.show(content, type)
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
        api.update_bot(this.user.user_id, this.botsList.length > 0 ? 0 : null, files[0], (progress) => {})
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
