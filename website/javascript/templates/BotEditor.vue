<template>
  <div style='margin-top: 61px' class="body">
    <div class="container-fluid h-100 page_container">
      <div class="row flex-xl-nowrap h-100 page_row">
        <div class="col-12 col-md-8 col-lg-8 col-xl-8 py-md-8 pl-md-8 bd-content big_col">
          <div class="file_tree_col">
            <div class="file_command_center_cont">
              <div class="file_command_center">
                <div class="command_icon"><a><span class="glyphicon glyphicon-file"></span></a></div>
                <div class="command_icon"><a><span class="glyphicon glyphicon-folder-open"></span></a></div>
                <div class="command_icon"><a><span class="glyphicon glyphicon-pencil"></span></a></div>
                <div class="command_center_right">
                  <div class="command_icon"><a><span class="glyphicon glyphicon-trash"></span></a></div>
                </div>
              </div>
            </div>
            <nav class="tree_nav"><div class="bd-toc-item active">
              <ul class="nav bd-sidenav">
                <li v-for="(f, name) in editor_files" class="bd-sidenav-active tree_files" v-bind:class="{ active: name === active_file_name }">
                  <a v-on:click="file_selected(name)">
                    {{ name }}
                  </a>
                </li>
              </ul>
            </div></nav>
          </div>
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
          <div class="replay">
            <div class="game_replay_viewer"></div>
          </div>
          <div class="console">
            {{ terminal_text }}
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  import * as api from '../api'
  import * as utils from '../utils'
  import * as libhaliteviz from '../../../libhaliteviz'


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
    }
  }

  var logVerbose = true
  const BOT_LANG_KEY = 'bot_language'
  const FILE_NAMES_KEY = 'file_names'
  const DARK_THEME = 'Dark'
  const RESET_MSG = 'Are you sure you want to reset your bot code to the default sample code?\n(All changes will be lost!)'
  const EX_GAME_STRING = '{"ENGINE_VERSION":"1.5.521.g6df5","GAME_CONSTANTS":{"BASE_TURN_ENERGY_LOSS":5,"BLUR_FACTOR":0.75,"DEFAULT_MAP_HEIGHT":128,"DEFAULT_MAP_WIDTH":128,"INITIAL_ENERGY":1000,"MAX_CELL_PRODUCTION":255,"MAX_ENERGY":255,"MAX_PLAYERS":16,"MAX_TURNS":300,"MIN_CELL_PRODUCTION":85,"NEW_ENTITY_ENERGY":255,"NEW_ENTITY_ENERGY_COST":1000},"REPLAY_FILE_VERSION":1,"full_frames":[{"events":[],"moves":{"0":[{"direction":"w","entity_x":0,"entity_y":1,"type":"move"}],"1":[{"direction":"e","entity_x":1,"entity_y":1,"type":"move"}]}}],"game_statistics":{"number_turns":49,"player_statistics":[{"last_turn_alive":49,"player_id":1,"rank":1,"total_production":770},{"last_turn_alive":49,"player_id":0,"rank":2,"total_production":518}]},"map_generator_seed":1531318637,"number_of_players":2,"players":[{"energy":0,"entities":[{"energy":0,"x":1,"y":1}],"factory_location":[1,1],"name":"JavaSP","player_id":1},{"energy":0,"entities":[{"energy":0,"x":0,"y":1}],"factory_location":[0,1],"name":"JavaSP","player_id":0}],"production_map":{"grid":[[{"production":14,"type":"n"},{"production":14,"type":"n"}],[{"type":"f"},{"type":"f"}]],"height":2,"map_generator":"Fractal Value Noise Tile","width":2}}'

  const HaliteVisualizer = libhaliteviz.HaliteVisualizer
  libhaliteviz.setAssetRoot('/assets/js/')

  function logError (err) {
    console.error(err)
  }

  function logInfo (msg) {
    if (logVerbose) console.log(msg)
  }


export default {
  name: 'bot_editor',
  data: function () {
    const lang = 'Python3'
    const theme = DARK_THEME
    const editor_files = this.editor_files === null ? [] : this.editor_files
    const terminal_text = this.terminal_text === null ? "" : this.terminal_text
    return {
      all_bot_languages: botLanguagePacks,
      status_message: null,
      logged_in: false,
      editorViewer: null,
      bot_lang: lang,
      selected_language: lang,
      selected_theme: theme,
      editor_files: editor_files,
      terminal_text: terminal_text
    }
  },
  props: {
    tutorial: {
      default: false,
      type: Boolean,
    }
  },
  created: function() {
    // Create promise so callers can use our methods without having to
    // check whether we're ready
    this.ready = new Promise((resolve) => {
      this._readyResolve = resolve;
    });
  },
  mounted: function () {
    api.me().then((me) => {
      if (me !== null) {
        this.user_id = me.user_id
        this.logged_in = true
        this.load_code().then((function(editor_files) {
          this.editor_files = editor_files
          this.active_file_name = this.bot_info().fileName
          this.create_editor(this.get_active_file_code())
        }).bind(this))
      } else {
        window.location.href = '/';
      }
    })
    console.log(jQuery('.replay'))
    let width = jQuery('.replay').width()
    this.visualizer = new HaliteVisualizer(JSON.parse(EX_GAME_STRING), width, width)
    this.visualizer.attach('.game_replay_viewer')
    window.addEventListener('resize', (function(event) {
      width = jQuery('.replay').width()
      console.log(width)
      this.visualizer.resize(width, width)
    }).bind(this), true)
  },
  methods: {
    /* Return bot language specific info */
    bot_info: function () {
      return botLanguagePacks[this.bot_lang]
    },
    /* Init the Orion Editor */
    create_editor: function (code) {
      this.add_console_text("[INFO] Initializing player 0 with command ./MyBot");
      logInfo('Creating editor...')
      const codeEdit = new orion.codeEdit({editorConfig: {wordWrap: true, overviewRuler: false, annotationRuler: false}})
      var opts = {parent: 'embeddedEditor', contentType: this.bot_info().mimeType, contents: code}
      codeEdit.create(opts).then((editorViewer) => {
        logInfo('Initializing editor...')
        this.editorViewer = editorViewer
        const editor = editorViewer.editor

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
          }
        }).bind(this))

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
        if (editorViewer.settings) {
          editorViewer.settings.showOccurrences = true
        }

        jQuery('.textview').addClass('editorTheme')
        logInfo('Editor ready!')
        this._readyResolve();
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
          editor_files[name] = {contents: values.shift()}
        }
        return editor_files
      })
    },
    file_selected: function(sel_file_name) {
      this.save_current_file()
      this.editor_files[this.active_file_name].contents = this.get_editor_code()
      this.set_editor_contents(this.editor_files[sel_file_name].contents)
      this.active_file_name = sel_file_name
    },
    load_code: function () {
      // Restore user's bot code, or use demo code for new bot
      const startCode = this.load_user_code()
      return (startCode === null) ? this.load_default_code() : Promise.resolve(startCode)
    },
    load_user_code: function() {
      function get_files_with_list(ctx, file_list) {
        return Promise.all(file_list.map(
          (file_name) => (api.get_editor_file(ctx.user_id, file_name).then(
            (contents) => ({contents: contents, name: file_name})))
          ))
      }
      return api.get_editor_file_list(this.user_id).then((function(file_list) {
        if(file_list !== []) {
          return get_files_with_list(this, file_list)
        } else { // if no workspace create one and then pull our files down
          api.create_editor_file_space(this.user_id, 'Python').then((function(file_list) {
            return get_files_with_list(this, file_list)
          }).bind(this))
        }
      }).bind(this)).then(function(file_promise) {
        return file_promise.then(function(file_contents) {
          let editor_files =  file_contents.reduce(function(prev, cur) {
            prev[cur.name] = {contents: cur.contents}
            return prev
          }, {})
          return editor_files
        })
      })
    },
    clearHighlights: function() {
      const editor = this.editorViewer.editor
      const annotationModel = editor.getAnnotationModel()
      const annotations = annotationModel.getAnnotations();
      while (annotations.hasNext()) {
        const annotation = annotations.next();
        annotationModel.removeAnnotation(annotation);
      }
    },
    highlightContaining: function(text) {
      const editor = this.editorViewer.editor
      const view = editor.getTextView()
      const viewModel = view.getModel()
      const annotationModel = editor.getAnnotationModel()
      const lineNumber = view.getLineAtOffset(view.getText().indexOf(text))
      const lineStart = editor.mapOffset(viewModel.getLineStart(lineNumber))
      const lineEnd = editor.mapOffset(viewModel.getLineEnd(lineNumber))
      annotationModel.replaceAnnotations([], [
        {
          start: lineStart,
          end: lineEnd,
          title: "",
          type: "tutorial.highlight",
          html: "",
          style: { styleClass: "tutorial-highlight" },
          lineStyle: { styleClass: "tutorial-highlight" },
        },
      ])
    },
    /* Return MyBot file of the starter bot in string format */
    load_default_code: function () {
      logInfo('Loading default bot code...')
      const fileName = this.bot_info().fileName
      return this.get_starter_zip().then((starterZip) => {
        logInfo('Got starter zip. Getting sample bot file: ' + fileName)
        return this.extract_files_from_zip(starterZip)
      }, logError)
    },
    get_active_file_code: function() {
      return this.editor_files[this.active_file_name].contents
    },
    /* Return a string of the code currently in the editor */
    get_editor_code: function () {
      return this.editorViewer.editor.getModel().getText()
    },
    get_starter_zip: function fn (forceClean = false) {
      if (fn.cached === undefined) { fn.cached = {} }
        const lang = this.bot_lang
      if (fn.cached[lang] === undefined || forceClean) {
        fn.cached[lang] = new Promise((resolve, reject) => {
          const starterZipPath = this.bot_info().starterZipPath
          JSZipUtils.getBinaryContent(starterZipPath, (err, data) => {
            logInfo('Getting starter zip: ' + starterZipPath) 
            if (err) {
              reject(err)
              return
            }
            try {
              JSZip.loadAsync(data).then(resolve, reject)
            } catch (e) {
              reject(e)
            }
          })
        })
      }
      return fn.cached[lang]
    },
    /* Return a zip file object with our starter pack files + our mybot file */
    get_user_zip_promise: function () {
      let zip = new JSZip();
      for(let name in this.editor_files) {
        zip.file(name, this.editor_files[name].contents)
      }
      return zip.generateAsync({type: 'blob'})
    },
    /* Load code into editor; either saved code or default */
    reload_code: function (use_default = false) {
      const codePromise = use_default ? this.load_default_code() : this.load_code()
      return codePromise.then((editor_files) => {
        this.editor_files = editor_files
        this.set_editor_contents(this.get_active_file_code())
      })
    },
    /* Revert to starter bot code */
    reset_code: function () {
      if (window.confirm(RESET_MSG)) {
        // reset to starter pack sample code for current language
        this.reload_code(true)
      }
      return true
    },
    run_ondemand_game: function() {
      let user_id = this.user_id
      api.start_ondemand_task(user_id).then(function(_) {
        console.log(_)
        return api.update_ondemand_task(user_id, 5000).then((a) => console.log(a))
      })
    },
    save_current_file: function() {
      logInfo('Saving bot file to gcloud storage')
      this.update_editor_files()
      api.update_source_file(this.user_id, this.active_file_name, this.editor_files[this.active_file_name].contents, function(){}).then((function() {
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
      this.terminal_text = "";
    },
    add_console_text: function(new_text) {
      if(this.terminal_text == undefined) this.terminal_text = ""
        this.terminal_text += new_text
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
            resolve();
          }, (err) => {
            logInfo('error: ' + err.message)
            this.status_message = err.message
            reject(err.message);
          })
        })
      })
    },
    update_editor_files: function() {
      this.editor_files[this.active_file_name].contents = this.editorViewer.editor.getModel().getText()
    }
  }
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
  padding-left: 10px;
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

.tree_files {
  font-size: 12pt;
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

.console {
  flex: 1 1 auto;
  padding: 15px;
  background-color: black;
  color: silver;
  font-family: Consolas,Monaco,Lucida Console,Liberation Mono,DejaVu Sans Mono,Bitstream Vera Sans Mono,Courier New, monospace;
  font-size: 10pt;
}

.replay, .console {
  width: 100%;
}
</style>

<style lang="scss">
.tutorial-highlight {
  background: red !important;
}
</style>
