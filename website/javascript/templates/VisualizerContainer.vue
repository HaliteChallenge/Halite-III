<template>
  <div>
    <div v-if="is_upload">
      <halite-upload-zone
        title="Replay a File"
        description="Drop a replay file here to upload"
        buttonText = "Select a replay file"
        :icon="`${baseUrl}/assets/images/icon-replay.svg`"
        v-on:change="play_replay"
        :progressBar="is_downloading"
        :progress="progress"
        :message="message">
      </halite-upload-zone>
    </div>
    <div v-if="message" class="row">
        <div class="col-md-12 status-message">
            <img class="loading-img" :src="`${baseUrl}/assets/images/loading-icon.gif`"/>
            <p class="visuallizer-loading-message message-top">{{message}}</p>
        </div>
    </div>
  </div>

</template>

<script>
  import Vue from 'vue'
  import * as api from '../api'
  import UploadZone from './UploadZone.vue'
  import Visualizer from './Visualizer.vue'
  import * as utils from '../utils'

  let visualizer = null

  const showGame = (game) => {
    if (visualizer && visualizer.getVisualizer) {
      visualizer.getVisualizer().destroy()
    }

    const promisify = (jq) => new Promise((resolve) => {
      jq.then((...args) => resolve(...args))
    })
    const users = Promise.all(
      Object.keys(game.game.players).map(id => promisify(api.get_user(id))));

    const buffer = game.replay
    return import(/* webpackChunkName: "libhaliteviz" */ "libhaliteviz")
      .then((libhaliteviz) =>Promise.all([
          (
            // just for electron
            (window && window.process &&
             window.process.type) ?
            libhaliteviz.setAssetRoot('assets/js/') :
            libhaliteviz.setAssetRoot('')
          ).then(() => libhaliteviz),
          users,
      ]))
      .then(([ libhaliteviz, users ]) => {
        return libhaliteviz.parseReplay(buffer).then((replay) => {
          let outerContainer = document.getElementById('halitetv-visualizer')
          outerContainer.innerHTML = ''

          let container = document.createElement('div')
          document.getElementById('halitetv-visualizer').appendChild(container)

          for (const user of users) {
            game.game.players[user.user_id] =
              Object.assign({}, game.game.players[user.user_id], user)
          }

          new Vue({
            el: container,
            render: (h) => h(Visualizer, {
              props: {
                replay: Object.freeze(replay),
                game: game.game,
              }
            }),
            mounted: function () {
              visualizer = this.$children[0]
            }
          })
        })
      });
  }

  export default {
    name: 'visualizer',
    props: [],
    components: {
      'halite-upload-zone': UploadZone
    },
    data: function () {
      return {
        is_downloading: false,
        progress: 0,
        message: null,
        is_upload: true,
        baseUrl: _global.baseUrl
      }
    },
    mounted: function () {
      const params = new URLSearchParams(window.location.search)
      const setupGame = (game) => {
        // this.message = 'Parsing replay, please wait…'
        this.message = 'Almost done...'
        this.$parent.currentView = 'replay'
        showGame(game).then(() => {
          this.is_downloading = false
          this.message = null
        }).catch((e) => {
          console.error(e);
          this.is_downloading = false
          this.message = 'There was an error parsing the replay. Try force-refreshing (Control-Shift-R or Command-Shift-R), or if that does not work, please let us know at halite@halite.io.'
        })
      }

      if (params.has('ondemand')) {
        this.message = "Downloading latest ondemand game.`"
        this.is_upload = false;

        (async function() {
          const me = await api.me()
          const replayBlob = await api.get_ondemand_replay(me["user_id"])
          setupGame({
            game: null,
            replay: replayBlob,
          })
        })()
      }
      else if (params.has('game_id')) {
        const game_id = params.get('game_id')
        this.message = `Downloading game ${game_id}.`
        this.is_upload = false
        api.get_replay(game_id, (loaded, total) => {
          if (total !== 0) {
            const progress = loaded / total
            this.is_downloading = true
            this.progress = Math.floor(100 * progress)
          }
        }).then((game) => {
          window.history.replaceState(null, '', `?game_id=${game_id}&replay_class=${game.game.replay_class}&replay_name=${encodeURIComponent(game.game.replay)}`)
          setupGame(game)
        }, () => {
          if (params.has('replay_class') && params.has('replay_name')) {
            const replay_class = params.get('replay_class')
            const replay_name = params.get('replay_name')
            this.$parent.replayFile = replay_name
            api.get_expired_replay(replay_class, replay_name).then((replay) => {
              this.$parent.currentView = 'replay'
              setupGame({
                game: null,
                replay: replay
              })
            }, () => {
              this.message = `Could not find old replay.`
              this.is_downloading = false
            })
          } else {
            this.message = `Could not download replay.`
            this.is_downloading = false
          }
        })
      }
      window.onhashchange = function () {
        if (!window.location.hash) {
          window.location.reload()
        }
      }

      // handle whole page drag and drop
      const ins = this
      $('body').on('drop dragdrop', function (e) {
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')

        // verify if the dropzone is not the bot uploader zone
        const files = e.originalEvent.dataTransfer.files
        if ((!container || !container.contains(e.target)) && files.length > 0) {
          e.preventDefault()
          // clear the current game
          let outerContainer = document.getElementById('halitetv-visualizer')
          outerContainer.innerHTML = ''
          // play the upload replay
          ins.play_replay(files)
        }
      })
      $('body').on('dragenter', function (e) {
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')
        if (!container || !container.contains(e.target)) {
          event.preventDefault()
        }
      })
      $('body').on('dragover', function (e) {
        // get the bot uploader container
        const container = document.getElementById('bot-upload-container')
        if (!container || !container.contains(e.target)) {
          event.preventDefault()
        }
      })
  },
    methods: {
      play_replay: function (files) {
        if (files.length > 0) {
          this.gaData('play', 'select-replay-file', 'replay-flow')
          const reader = new FileReader()
          const inst = this
          reader.onload = (e) => {
            inst.is_upload = false
            this.$parent.currentView = 'replay'
            window.location.hash = '/replay-bot'
            this.$parent.replayFile = files[0].name
            // this.message = 'Parsing replay, please wait…'
            this.message = 'Almost done...'
            showGame({
              game: null,
              replay: e.target.result
            }).then(() => {
              this.message = null
            }).catch(() => {
              this.message = 'There was an error parsing the replay. Try force-refreshing (Control-Shift-R or Command-Shift-R), or if that does not work, please let us know at halite@halite.io.'
            })
          }
          reader.readAsArrayBuffer(files[0])
        }
      },
      gaData: function (category, action, label) {
        utils.gaEvent(category, action, label)
      }
    }
  }
</script>

<style>

</style>
