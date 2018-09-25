<template>
  <div v-if="needs_login" class="body needs-login">
    <p>You must be logged in to use the editor.</p>
    <a :href="login_url">Log In</a>
  </div>
  <div class="body" v-else>
    <div class="container-fluid h-100 page_container">
      <div class="row flex-xl-nowrap h-100 page_row">
        <div class="col-12 col-md-8 col-lg-8 col-xl-8 py-md-8 pl-md-8 bd-content big_col">
          <div class="editor_col">
            <div class="editorArea">
              <div class="editorBody" id="embeddedEditor">
                <div class="editorTitle">
                  <span id = "progressMessageDiv">Loading language tooling plugins...</span>
                </div>
              </div>
            </div>
          </div>
        </div>
        <div class="col-12 col-md-4 col-lg-4 col-xl-4 hidden-sm hidden-xs py-md-4 pl-md-4 bd-toc replay_col">
          <div class="replay" :style="visualizerVisible ? 'display: block' : 'display: none'">
            <div class="game_replay_viewer"></div>
          </div>
          <nav class="replay-extra-controls" v-if="visualizer">
              <button
                class="pop-out-replay"
                v-on:click="pop_out_replay">Pop Out Replay</button>
              <button
                class="close-replay"
                v-on:click="toggle_replay">Toggle Replay</button>
          </nav>
          <div class="console"><span
                                 v-for="text in terminal_text"
                                 v-bind:class="text.class ? text.class : ''"
                               >{{text.text ? text.text : text}}</span></div>
        </div>
      </div>
    </div>
    <InputModal ref="new_file_modal" :baseUrl="baseUrl" :isOn="is_new_file_modal_open" :createCallback="create_new_file" :closeCallback="close_new_file_modal" title_text="New File" cancel_text="Cancel" accept_text="Create"></InputModal>
    <InputModal ref="new_folder_modal" :baseUrl="baseUrl" :isOn="is_new_folder_modal_open" :createCallback="create_new_folder" :closeCallback="close_new_folder_modal" title_text="New Folder" cancel_text="Cancel" accept_text="Create"></InputModal>
    <CheckModal ref="delete_modal" :baseUrl="baseUrl" :isOn="is_delete_modal_open" :yesCallback="delete_file" :noCallback="close_delete_modal" title_text="Delete All Files?" cancel_text="Cancel" accept_text="Delete" body_text="This will PERMANENTLY delete all editor files."></CheckModal>
    <LanguageModal :baseUrl="baseUrl" :tutorial="tutorial" :isOn="is_picking_language" @choose="pick_language" />

    <div class="toasts">
        <transition-group name="toast-pop-up" tag="div">
            <div v-for="toast in alerts" class="editor-toast" v-bind:key="toast.key">
                {{toast.message}}
            </div>
        </transition-group>
    </div>
    <div class="banner" v-if="banner_message">
      {{banner_message}}
      <button class="btn-sm btn-primary" @click="banner_message = null">Close</button>
    </div>
  </div>
</template>

<script>
  import moment from "moment";
  import JSZip from "jszip";
  import * as api from '../api'
  import editorState from '../editorState'
  import * as utils from '../utils'
  import LanguageModal from './BotEditorLanguageModal.vue'
  import InputModal from './InputModal.vue'
  import CheckModal from './CheckModal.vue'

  const botLanguagePacks = {
    'Python3': {
      mimeType: 'text/x-python',
      fileName: 'MyBot.py',
      zipName: 'my-py3-bot.zip',
      starterZipPath: '/assets/downloads/Halite2_Python3_None.zip'
    },
    'Java': {
      mimeType: 'text/x-java-source',
      fileName: 'MyBot.java',
      zipName: 'my-java-bot.zip',
      starterZipPath: '/assets/downloads/Halite2_Java_None.zip'
    },
    'C++': {
      mimeType: 'text/x-c++src',
      fileName: 'MyBot.cpp',
      zipName: 'my-c++-bot.zip',
      starterZipPath: '/assets/downloads/Halite2_C++_None.zip'
    },
    'JavaScript': {
      mimeType: 'application/javascript',
      fileName: 'MyBot.js',
      zipName: 'my-javascript-bot.zip',
    }
  }

  var logVerbose = true
  const DARK_THEME = 'Dark'
  const RESET_MSG = 'Are you sure you want to reset your bot code to the default sample code?\n(All changes will be lost!)'

  function logError(err) {
    console.error(err)
  }

  function logInfo(msg) {
    if (logVerbose) console.log(msg)
  }

  function parse_to_file_tree(files) {
    let file_tree = {}
    for (const [ file_name, file_obj ] of Object.entries(files)) {
      const components = file_name.split('/')

      // Create nested objects as necessary
      let current_obj = file_tree
      for (const component of components.slice(0, -1)) {
        if (typeof current_obj[component] === "undefined") {
          current_obj[component] = {}
        }
        current_obj = current_obj[component];
      }

      current_obj[components[components.length - 1]] = file_obj
    }
    return file_tree
  }

class EditorFile {
  constructor(name, contents) {
    this.name = name;
    this.contents = contents;
  }

  toJSON() {
    return {
      name: this.name,
      contents: this.contents,
    };
  }
}

export default {
  name: 'bot_editor',
  components: { InputModal, CheckModal, LanguageModal },
  data: function () {
    const lang = 'Python3'
    const theme = DARK_THEME
    return {
      state: editorState,
      active_file: new EditorFile('', ''),
      status_message: null,
      logged_in: false,
      needs_login: false,
      editorViewer: null,
      bot_lang: lang,
      selected_language: lang,
      selected_theme: theme,
      editor_files: [],
      file_tree: [],
      terminal_text: [],
      is_new_file_modal_open: false,
      is_new_folder_modal_open: false,
      is_delete_modal_open: false,
      alerts: [],
      alertKey: 0,
      visualizer: null,
      visualizerVisible: true,
      // List of times games were started, to prevent spamming
      gameTimes: [],
      // Monotonic counter, so running multiple games doesn't give you multiple results
      gameCounter: 0,
      allSaved: false,
      saveCallback: null,
      is_picking_language: false,
      banner_message: '',
    }
  },
  props: {
    baseUrl: {
      type: String,
    },
    localStorage: {
      default: false,
      type: Boolean,
    },
    tutorial: {
      default: false,
      type: Boolean,
    },
  },
  created: function() {
    // Create promise so callers can use our methods without having to
    // check whether we're ready
    this.ready = new Promise((resolve) => {
      this._readyResolve = resolve;
    });
  },
  mounted: function () {
    // Get rid of spacing to header
    for (const el of document.querySelectorAll('header.navbar')) {
      el.style.marginBottom = "0";
    }
    document.querySelector('.body').style.marginTop = "0";

    api.me().then((me) => {
      this.logged_in = me !== null
      if (me !== null) {
        this.user_id = me.user_id
      }

      if (me !== null || this.localStorage) {
        this.load_user_code().then((editor_files) => {
          this.editor_files = parse_to_file_tree(editor_files)
          for (const fileName of Object.keys(editor_files)) {
            if (fileName.startsWith("MyBot")) {
              this.active_file = editor_files[fileName]
              for (const [lang, langInfo] of Object.entries(botLanguagePacks)) {
                if (langInfo.fileName === fileName) {
                  this.bot_lang = lang
                  break
                }
              }
              break
            }
          }
          if (!this.active_file) {
            this.active_file = editor_files[Object.keys(editor_files)[0]]
          }
          this.create_editor(this.get_active_file_code())
        }).catch((e) => {
          console.warn(e)
          // No files, ask user to pick
          this.is_picking_language = true
        })
      } else {
        this.needs_login = true
      }
    })

    window.addEventListener('resize', (event) => {
      if (this.visualizer) {
        const width = jQuery('.replay').width()
        this.visualizer.resize(width, width)
      }
    }, true)
  },
  computed: {
    login_url() {
      return `${api.LOGIN_SERVER_URL}/github`
    },
  },
  methods: {
    /* Return bot language specific info */
    bot_info: function () {
      return botLanguagePacks[this.bot_lang]
    },
    pick_language: async function(language) {
      this.is_picking_language = false
      this.bot_lang = language
      if (this.localStorage) {
        // TODO: download something for real
        const fetch = await window.fetch(`/assets/downloads/${language}-Tutorial.zip`)
        const zipBlob = await fetch.blob()
        const zip = await JSZip.loadAsync(zipBlob)
        const files = await Promise.all(
          Object.values(zip.files)
                .map(key => key.async("text").then(contents => [ key.name, contents ])))
        for (const [ key, contents ] of files) {
          window.localStorage.setItem(key, JSON.stringify(new EditorFile(key, contents).toJSON()))
        }
        window.localStorage.setItem("@@editor-filelist",
                                    JSON.stringify(files.map(([key]) => key)))
      }
      else if (this.tutorial) {
        await api.create_editor_file_space(this.user_id, language + '-Tutorial')
      }
      else {
        await api.create_editor_file_space(this.user_id, language)
      }
      this.setup_editor()
    },
    setup_editor: async function() {
      const editor_files = await this.load_user_code()
      this.active_file = _.find(editor_files, {name: this.bot_info().fileName})
      this.editor_files = parse_to_file_tree(editor_files)
      this.create_editor(this.get_active_file_code())
    },
    /* Init the Orion Editor */
    create_editor: function (code) {
      logInfo('Creating editor...')
      const codeEdit = new orion.codeEdit({editorConfig: {wordWrap: true, overviewRuler: false, lineNumberRuler: true, annotationRuler: false}})
      var opts = {parent: 'embeddedEditor', contentType: this.bot_info().mimeType, contents: code}
      codeEdit.create(opts).then((editorViewer) => {
        logInfo('Initializing editor...')
        this.editorViewer = editorViewer
        const editor = editorViewer.editor

        console.log(editorViewer)
        editor.setAnnotationRulerVisible(false);

        if (this.tutorial) {
          // https://wiki.eclipse.org/Orion/How_Tos/Code_Edit#How_to_highlight_a_line_in_the_editor_and_set_cursor_on_that_line
          // Set up annotations so we can highlight lines
          const annoStyler = editor.getAnnotationStyler()
          annoStyler.addAnnotationType("tutorial.highlight")
          editor.getAnnotationRuler().addAnnotationType("tutorial.highlight")
        }

        // make sure we're using the correct color theme

        logInfo('Adding editor save handlers...')

        // Set up save action (note: auto-save is enabled by default)
        const saveEventHandler = () => {
          this.save_current_file()
          return true
        }

        // Manual save action (Ctrl+S)
        editor.getTextView().setAction('save', saveEventHandler)

        editor.addEventListener('InputChanged', (function (evt) {
          if (evt.contentsSaved) {
            logInfo('input changed')
            this.allSaved = false;
            if (this.saveCallback !== null) {
              window.clearTimeout(this.saveCallback)
            }
            window.setTimeout(() => {
              this.save_current_file()
            }, 2500)
          }
        }).bind(this))

        this.add_console_text("Initialized editor.");

        // Be sure to save before closing/leaving the page
        window.addEventListener('beforeunload', (function(e) {
          logInfo(this.allSaved)
          if (this.allSaved) {
            return undefined;
          }

          var confirmationMessage = 'It looks like you have been editing something. '
          + 'If you leave before saving, your changes will be lost.';

          (e || window.event).returnValue = confirmationMessage; //Gecko + IE
          return confirmationMessage; //Gecko + Webkit, Safari, Chrome etc.
        }).bind(this))

        // Other settings
        editorViewer.updateSettings({
          showOccurrences: true,
          expandTab: true,
        })

        jQuery('.textview').addClass('editorTheme')

        this.editorViewer.editor.setLineNumberRulerVisible(true);

        // Load saved settings
        this.state.load().then(() => {
          logInfo('Editor ready!')
          this.allSaved = true
          this.$emit('save', true)
          this._readyResolve();
        })
      })
    },
    // Schedule a function to be called when we're ready
    doReady: function(f) {
      this.ready.then(() => f(this));
    },
    /*Export bots to a zip file for local storage by user*/
    download_bot: function () {
      this.get_user_zip_promise().then((blob) => {
        const zipName = this.bot_info().zipName
        logInfo('Saving zip file of editor code: ' + zipName)
        saveAs(blob, zipName)
      })
      return true
    },
    extract_files_from_zip: function(zip_file) {
      let promises = []
      let names = Object.keys(zip_file.files)
      for(let a = 0; a < names.length; a++) {
        promises.push(zip_file.file(names[a]).async('text'))
      }
      return Promise.all(promises).then(function(values) {
        let editor_files = {}
        for(let a = 0; a < names.length; a++) {
          let name = names[a]
          editor_files[name] = new EditorFile(name, values.shift())
        }
        return parse_to_file_tree(editor_files)
      })
    },
    file_selected: function(sel_file) {
      console.log(sel_file)
      if(this.active_file !== null) {
        this.save_current_file()
        this.active_file.contents = this.get_editor_code()
      }
      this.set_editor_contents(sel_file.contents)
      this.editorViewer.editor.setLineNumberRulerVisible(true);
      this.active_file = sel_file
    },
    load_user_code: function() {
      function api_get_files_with_list(ctx, file_list) {
        return Promise.all(
          file_list.map((file_name) => {
            return api.get_editor_file(ctx.user_id, file_name)
               .then((contents) => new EditorFile(file_name, contents))
          }))
      }

      let filePromise;
      if (this.localStorage) {
        const fileList = window.localStorage.getItem("@@editor-filelist")
        if (!fileList) {
          return Promise.reject()
        }
        else {
          filePromise = Promise.resolve(
            JSON.parse(fileList)
                .map(key => JSON.parse(window.localStorage.getItem(key))))
        }
      }
      else {
        filePromise = api.get_editor_file_list(this.user_id)
                         .then((file_list) => {
                           if (file_list.length > 0) {
                             return api_get_files_with_list(this, file_list)
                           }
                           else { // if no workspace ask user to choose a language
                             return Promise.reject()
                           }
                         })
      }

      return filePromise.then((file_contents) => {
        let editor_files =  file_contents.reduce(function(prev, cur) {
          prev[cur.name] = new EditorFile(cur.name, cur.contents)
          return prev
        }, {})
        return parse_to_file_tree(editor_files)
      })
    },
    clearHighlights: function() {
      const editor = this.editorViewer.editor
      const annotationModel = editor.getAnnotationModel()
      // Actual getAnnotations doesn't seem to return all of them
      // (iterator invalidation?)
      while (annotationModel._annotations.length > 0) {
        const annotation = annotationModel._annotations[0];
        annotationModel.removeAnnotation(annotation);
      }
    },
    highlightContaining: function(text, klass="tutorial-highlight") {
      const editor = this.editorViewer.editor
      const view = editor.getTextView()
      const viewModel = view.getModel()
      const annotationModel = editor.getAnnotationModel()
      for (let lineNumber = 0; lineNumber < viewModel.getLineCount(); lineNumber++) {
        const line = viewModel.getLine(lineNumber)
        if (text instanceof RegExp ? line.match(text) : line.indexOf(text) > -1) {
          const lineStart = editor.mapOffset(viewModel.getLineStart(lineNumber))
          const lineEnd = editor.mapOffset(viewModel.getLineEnd(lineNumber))
          annotationModel.replaceAnnotations([], [
            {
              start: lineStart,
              end: lineEnd,
              title: "",
              type: "tutorial.highlight",
              html: "",
              lineStyle: { styleClass: klass },
            },
          ])
        }
      }
    },
    get_active_file_code: function() {
      return this.active_file == null ? "" : this.active_file.contents
    },
    /* Return a string of the code currently in the editor */
    get_editor_code: function () {
      return this.editorViewer.editor.getModel().getText()
    },
    /* Return a zip file object with our starter pack files + our mybot file */
    get_user_zip_promise: function () {
      let zip = new JSZip();

      const files = [];
      // Queue of [ parent path, Object { pathname: file | Object } ]
      const queue = [ [ '', this.editor_files ] ];

      while (queue.length > 0) {
        const [ parentPath, pathEntries ] = queue.shift();

        for (const [ pathName, entry ] of Object.entries(pathEntries)) {
          if (entry instanceof EditorFile) {
            files.push([ parentPath + pathName, entry.contents ]);
          }
          else {
            queue.push([ parentPath + pathName + '/', entry ]);
          }
        }
      }
      for (const [path, fileContents] of files) {
        zip.file(path, fileContents)
      }
      return zip.generateAsync({type: 'blob'})
    },
    create_new_file: function(name) {
      this.$set(this.editor_files, name, new EditorFile(name, ''))
      this.file_selected(this.editor_files[name])
      this.close_new_file_modal()
    },
    open_new_file_modal: function() { this.is_new_file_modal_open = true; },
    close_new_file_modal: function() { this.is_new_file_modal_open = false; },

    create_new_folder: function(name) {
      this.$set(this.editor_files, name, {})
      this.close_new_folder_modal()
    },
    open_new_folder_modal: function() { this.is_new_folder_modal_open = true; },
    close_new_folder_modal: function() { this.is_new_folder_modal_open = false; },


    delete_file: function() {
      api.delete_editor_files(this.user_id).then(() => {
        window.location.reload()
      })
    },
    delete_from_tree: function(files, elem) {
      for(let name in files) {
        if(files[name] === elem) {
          this.$delete(files, name)
          return
        }

        if(typeof files[name].contents !== 'string') {
          this.delete_from_tree(files[name], elem)
        }
      }
    },
    delete_all_files: function() {
      this.is_delete_modal_open = true;
    },
    close_delete_modal: function() { this.is_delete_modal_open = false; },

    run_ondemand_game: async function(params) {
      await this.save_current_file()
      this.clear_terminal_text()
      const user_id = this.user_id

      const cutoff = moment().subtract(1, 'minutes')
      this.gameTimes = this.gameTimes.filter(time => time.isSameOrAfter(cutoff))
      if (this.gameTimes.length > 3) {
        this.banner_message = "No more than 3 games per minute allowed, please wait and try again."
        return;
      }

      this.gameTimes.push(moment())

      const taskResult = await api.start_ondemand_task(user_id, Object.assign({}, {
        opponents: [
          {
            name: this.state.BOT_OPTIONS[this.state.opponent],
            bot_id: this.state.opponent,
          },
        ],
        width: this.state.mapSize,
        height: this.state.mapSize,
      }, params))
      console.log(taskResult)

      if (taskResult.status !== "success") {
        if (taskResult.message) {
          this.banner_message = taskResult.message
        }
        return
      }
      this.banner_message = ''

      const startResult = await api.update_ondemand_task(user_id, {
        "turn-limit": 500,
      })
      console.log(startResult)
      this.add_console_text("Starting game...\n")

      this.gameCounter += 1
      return this.check_ondemand_game(this.gameCounter)
    },
    check_ondemand_game: async function(counter) {
      // If another game has been started in the meantime, abandon this one
      if (this.gameCounter !== counter) {
        console.log(`Ondemand game ${counter} canceled.`)
        return;
      }

      const status = await api.get_ondemand_status(this.user_id)

      if (this.visualizer) {
        this.visualizer.destroy()
        this.visualizer = null
      }

      if (status.status === "completed") {
        if (status.error_log) {
          if (status.crashed) {
            this.add_console_text("Your bot crashed :(\n")
          }
          else {
            this.add_console_text("Your bot generated a log.\n")
          }
        }

        if (status.compile_error) {
          this.add_console_text("Your bot failed to compile :(\n")
          this.add_console_text(status.compile_error)
          return
        }

        this.add_console_text("Fetching replay...\n")

        const replayBlob = await api.get_ondemand_replay(this.user_id)
        const libhaliteviz = await import(/* webpackChunkName: "libhaliteviz" */ "libhaliteviz")
        await libhaliteviz.setAssetRoot('')
        const replay = await libhaliteviz.parseReplay(replayBlob)
        const width = jQuery('.replay').width()

        this.visualizer = new libhaliteviz.EmbeddedVisualizer(replay, width, width)
        this.visualizer.attach('.game_replay_viewer', '.game_replay_viewer')
        this.visualizer.play()
        this.visualizerVisible = true
        this.add_console_text("Starting replay.\n")

        const energy = replay.full_frames[replay.full_frames.length - 1].energy || {};
        for (let i = 0; i < status.opponents.length; i++) {
          this.add_console_text({
            text: `Player ${i}`,
            class: `color-${i + 1}`,
          })
          const name = status.opponents[i].name
          let displayName = name
          if (i === 0) {
            displayName = "(your bot)"
          }
          else if (name === "Self") {
            displayName = "(a copy of your bot)"
          }
          else {
            displayName = `"${displayName}"`
          }
          const last_energy = typeof energy[i] === "undefined" ? replay.GAME_CONSTANTS.INITIAL_ENERGY : energy[i];
          this.add_console_text(` ${displayName} was rank ${status.game_output.stats[i].rank} with ${last_energy} halite.\n`)
        }

        if (status.error_log) {
          this.add_console_text("Fetching error log...\n")
          const log = await api.get_ondemand_error_log(this.user_id)
          this.add_console_text(log)
        }

        if (status.bot_log) {
          this.add_console_text("Fetching bot log...\n")
          const log = await api.get_ondemand_bot_log(this.user_id)
          this.add_console_text(log)
        }

        return
      }
      else if (status.status === "failed") {
        this.add_console_text("Game failed to run. Please try again in a few minutes.\n")
        return
      }

      window.setTimeout(() => this.check_ondemand_game(counter), 1000)
    },
    pop_out_replay: function() {
      window.open("/play?ondemand")
    },
    toggle_replay: function() {
      this.visualizerVisible = !this.visualizerVisible
    },
    save_current_file: function() {
      logInfo('Saving bot file to gcloud storage')
      if(this.active_file !== null) {
        this.update_editor_files()
        return this.save_file(this.active_file).then(() => {
          this.alert("Saved")
        })
      }
    },
    save_file: function(file) {
      if (this.localStorage) {
        window.localStorage.setItem(file.name, JSON.stringify(file.toJSON()))
        window.localStorage.setItem(
          "@@editor-filelist",
          JSON.stringify(JSON.parse(window.localStorage.getItem("@@editor-filelist"))
                             .filter(name => name !== file.name)
                             .concat([ file.name ])))

        this.allSaved = true
        return Promise.resolve()
      }

      return api.update_source_file(this.user_id, file.name, file.contents, function(){}).then((function() {
        logInfo('success')
        this.allSaved = true;
      }).bind(this), function(response) {
        logInfo(response)
      })
    },

    /* Set the contents of our edior */
    set_editor_contents: function(contents) {
      this.editorViewer.setContents(contents, this.bot_info().mimeType)
    },
    clear_terminal_text: function() {
      this.terminal_text = []
    },
    add_console_text: function(new_text) {
      this.terminal_text.push(new_text)
    },
    alert: function(text) {
      this.alerts.push({ message: text, key: this.alertKey++ })
      setTimeout(() => {
        this.alerts.shift()
      }, 3000)
    },
    /* Submit bot to our leaderboard */
    submit_bot: function () {
      logInfo('... starting upload')
      return new Promise ((resolve, reject) => {
        this.get_user_zip_promise().then((blob) => {
          const botFile = new File([blob], this.bot_info().zipName)
          let user_id
          this.status_message = null

          const has_bot_promise = api.me().then((user) => {
            user_id = user.user_id
            return api.list_bots(user.user_id)
          }).then((bots) => {
            if (bots.length > 0) {
              return bots[0].bot_id
            }
            return null
          })

          has_bot_promise.then((bot_id) => {
            logInfo('Uploading zip file: ' + botFile)
            return api.update_bot(user_id, bot_id, botFile, (progress) => {
              const p = Math.floor(progress * 100)
              this.status_message = `Uploading... (${p}%)`
            })
          }).then(() => {
            logInfo('upload successully')
            this.status_message = 'Successfully uploaded!'
            this.alert("Successfully uploaded!")
            resolve();
          }, (err) => {
            logInfo('error: ' + err.message)
            this.status_message = err.message
            this.alert(`Error uploading bot: ${err.message}`)
            reject(err.message);
          })
        })
      })
    },
    update_editor_files: function() {
      this.active_file.contents = this.editorViewer.editor.getModel().getText()
    }
  },
  watch: {
    allSaved(value) {
      this.$emit('save', value)
    },
    state: {
      handler(newState) {
        this.editorViewer.updateSettings(newState.editor)
        this.editorViewer.editor.setLineNumberRulerVisible(true);
        this.state.save()
        this.alert("Updated settings")
      },
      deep: true
    }
  },
}
</script>

<style lang="scss" scoped>
.editorTitle{
  border: none;
  vertical-align: middle;
  overflow: hidden;
  text-align: left;
  margin-left: 15%;
  margin-right: 15%;
  padding-bottom: 5px;
  position: relative;
}
.editorBody{
  border: 0;
  vertical-align: middle;
  overflow: hidden;
  text-align: left;
  margin: 15px 0px;
  position: relative;
  margin: 0px;
}

.editorArea button{
  color: black
}

.page_container {
  margin-right: 0px;
  margin-left: 0px;
  padding-left: 0px;
  padding-right: 0px;
}

.big_col {
  margin: 0px;
  padding: 0px;
}

.editor_col {
  padding: 0px;
  display: flex;
  flex-grow: 1;
  margin-top: 0px;
  border-right: 1px solid #424C53;
  flex-grow: 100;
}

.editorArea  {
  width: 100%;
}

.file_tree_col {
  padding: 0px;
  width: 150px;
  border-right: 1px solid #424C53;
  float: left;
  display: flex;
  flex-flow: column;
}

.file_command_center_cont {
  width: 100%;
  padding-top: 8px;
  padding-bottom: 2px;
  border-bottom: 1px solid #424C53;
  background-color: #202325;
}

.file_command_center {
  width: 100%;
  padding-left: 3px;
  padding-right: 3px;
  display: flex;
  flex-flow: row;
}

.command_center_right{
  width: 100%;
  float: right;
  flex: 1;
  text-align: right;
}

.command_center_right > div {
  text-align: right;
  float: right;
}

.command_icon {
  display: inline;
  margin-left: 3px;
  margin-right: 3px;
}

.command_icon a {
  font-size: 16px;
  display: inline;
  position: relative;
  color: orange;
  height: auto;
}

.tree_nav {
  flex: 1;
}

.replay_col {
  padding: 0px;
  display: flex;
  flex-flow: column;
}
.main, .page_container, .page_row, .editorArea, .replay_col, .big_col, .editor_col, .file_tree_col, .editorBody {
 height: 100%;
}

.replay {
  border-bottom: 1px solid #424C53;
  height: auto;
  min-height: auto;
}

.game_replay_viewer {
  height: auto;
  width: 100%;
}

.replay-extra-controls {
  flex: 0 0 2.5rem;
  display: flex;

    button {
      flex: 1;
      background: #21212B;
    }
}

.console {
  white-space: pre-line;
  flex: 1 1 auto;
  padding: 15px;
  background-color: black;
  color: silver;
  font-family: Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;
  font-size: 10pt;
  overflow-y: auto;
}

.replay, .console {
  width: 100%;
}

.toasts {
  position: fixed;
  bottom: 1em;
  left: 0;
  right: 0;
  z-index: 99999999;

  font-size: 1.2em;

  .editor-toast {
    margin: 1em auto 0;
    padding: 0.25em 0.5em;
    width: fit-content;

    background: #FFF;
    box-shadow: 0 1px 5px #FFF;
    color: #333;
  }
}

.toast-pop-up-enter-active {
  transition: all .5s ease-out;
}

.toast-pop-up-leave-active {
  transition: all .3s ease-in;
}

.toast-pop-up-enter {
  transform: translateY(5em);
}

.toast-pop-up-leave-to {
  transform: translateY(-2em);
  opacity: 0;
}

.toast-pop-up-move {
  transition: all .5s ease-in-out;
}

.needs-login {
  font-size: 1.25em;
  color: #FFF;
  text-align: center;
}

.banner {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  padding: 1em;
  background: #FF2D55;
  color: #FFF;
  text-align: center;
  z-index: 20000;

  button {
    position: absolute;
    right: 1em;
    top: 0.25em;
    bottom: 0.25em;
  }
}
</style>

<style lang="scss">
.tutorial-highlight {
  background: rgba(255, 255, 255, 0.3) !important;
}
.tutorial-highlight-alt {
  background: rgba(255, 0, 255, 0.3) !important;
}
canvas {
  /* Fixes extra space under visualizer */
  display: block;
}
</style>
<!--
     Local Variables:
     web-mode-style-padding: 0
     web-mode-script-padding: 0
     End:
     End: -->
