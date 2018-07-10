<template>
        <div class="container-fluid h-100 page_container" role="main">
                <div class="row flex-xl-nowrap h-100 page_row">
                        <div class="col-12 col-md-2 col-xl-1 hidden-sm hidden-xs bd-sidebar file_tree_col">
                                <nav id="bd-docs-nav"><div class="bd-toc-item active">
                                        <ul class="nav bd-sidenav">
                                          <li v-for="(f, name) in editor_files" class="bd-sidenav-active" v-bind:class="{ active: name === active_file_name }">
                                            <a v-on:click="file_selected(name)">
                                              {{ name }}
                                            </a>
                                          </li>
                                        </ul>
                                </div></nav>
                        </div>
                        <div class="col-12 col-md-6 col-xl-7 py-md-6 pl-md-6 bd-content editor_col">
                                <div class="editorArea">
                                        <div class="editorBody" id="embeddedEditor">
                                                <div class="editorTitle">
                                                        <span id = "progressMessageDiv">Loading language tooling plugins...</span>
                                                </div>
                                        </div>
                                </div>
                        </div>
                        <div class="col-12 col-md-4 col-xl-4 hidden-sm hidden-xs py-md-4 pl-md-4 bd-toc replay_col">
                          <div class="replay">
                          </div>
                          <div class="console">
                          </div>
                        </div>
                </div>
        </div>
</template>

<script>
import * as api from '../api'

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

function logError (err) {
  console.error(err)
}

function logInfo (msg) {
  if (logVerbose) console.log(msg)
}

function copyZip (zipPromise) {
  return zipPromise.then((zip) => zip.generateAsync({type: 'blob'}))
    .then(JSZip.loadAsync)
}

export default {
  name: 'bot_editor',
  data: function () {
    const lang = 'Python3'
    const theme = DARK_THEME
    const editor_files = this.editor_files === null ? [] : this.editor_files
    return {
      all_bot_languages: botLanguagePacks,
      status_message: null,
      logged_in: false,
      editorViewer: null,
      bot_lang: lang,
      selected_language: lang,
      selected_theme: theme,
      editor_files: editor_files
    }
  },
  /*
  Run on view mount. Grab our user credentials from the API and then create editor.
  */
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
      }
    })
    // Restore user's bot code, or use demo code for new bot
  },
  methods: {
    /* Return bot language specific info */
    bot_info: function () {
      return botLanguagePacks[this.bot_lang]
    },
    /* Init the Orion Editor */
    create_editor: function (code) {
      logInfo('Creating editor...')
      const codeEdit = new orion.codeEdit({editorConfig: {wordWrap: true, annotationRuler: false}})
      var opts = {parent: 'embeddedEditor', contentType: this.bot_info().mimeType, contents: code}
      codeEdit.create(opts).then((editorViewer) => {
        logInfo('Initializing editor...')
        this.editorViewer = editorViewer
        const editor = editorViewer.editor
        
        editor.setAnnotationRulerVisible(false);

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
      })
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
      return copyZip(this.get_starter_zip()).then((zip) => {
        return zip.file(this.bot_info().fileName, this.get_editor_code())
          .generateAsync({type: 'blob'})
      })
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

  .container-fluid {
    margin-right: 0px;
    margin-left: 0px;
    padding-left: 0px;
    padding-right: 0px; 
  }
  .file_tree_col {
    padding: 0px;	
    border-right: 1px solid #424C53;
  }
  .editor_col {
    padding: 0px;	
    margin-top: 0px;
    border-right: 1px solid #424C53;
  }
  .replay_col {
    padding: 0px;
  }
  .page_container, .page_row, .editorArea, .replay_col, .editor_col, .file_tree_col, .editorBody {
     height: 100%;
  }

  .replay {
    border-bottom: 1px solid #424C53;
  }

  .replay, .console {
    width: 100%;
    height: 50%;
  }

</style>
