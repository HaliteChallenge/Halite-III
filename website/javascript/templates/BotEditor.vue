<template>
        <div class="container-fluid h-100" role="main">
                <div class="row flex-xl-nowrap h-100">
                        <div class="col-12 col-md-1 col-xl-1 hidden-sm hidden-xs bd-sidebar">
                                <nav id="bd-docs-nav"><div class="bd-toc-item active">
                                        <ul class="nav bd-sidenav">
                                          <li v-for="f in file_list" class="bd-sidenav-active" v-bind:class="{ active: f.isActive }">
                                            <a v-on:click="file_selected(f.name)" href="">
                                              {{ f.file_name }}
                                            </a>
                                          </li>
                                        </ul>
                                </div></nav>
                        </div>
                        <div class="col-12 col-md-7 col-xl-7 py-md-7 pl-md-7 bd-content">
                                <div class="editorArea">
                                        <div class="editorBody" id="embeddedEditor">
                                                <div class="editorTitle">
                                                        <span id = "progressMessageDiv">Loading language tooling plugins...</span>
                                                </div>
                                        </div>
                                </div>
                        </div>
                        <div class="col-12 col-md-4 col-xl-4 hidden-sm hidden-xs py-md-4 pl-md-4 bd-toc">
                          REPLAY HERE
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

var logVerbose = false
const BOT_LANG_KEY = 'bot_language'
const THEME_KEY = 'editor_theme'
const DARK_THEME = 'Light'
const RESET_MSG = 'Are you sure you want to reset your bot code to the default sample code?\n(All changes will be lost!)'

function saveCode (ctx) {
  const fileName = ctx.bot_info().fileName
  logInfo('Saving bot file to web local storage: ' + fileName)
  const code = ctx.get_editor_code()
  window.localStorage.setItem(fileName, code)
  window.localStorage.setItem(BOT_LANG_KEY, ctx.bot_lang)
  window.localStorage.setItem(THEME_KEY, ctx.selected_theme)
}

function loadEditorTheme () {
  var theme = window.localStorage.getItem(THEME_KEY)
  if (!theme) {
    theme = DARK_THEME
  }
  logInfo('Setting editor theme to ' + theme)
  return theme
}

function loadCodeFromLocalStorage (ctx) {
  const fileName = ctx.bot_info().fileName
  logInfo('Loading code into editor from web local storage: ' + fileName)
  return window.localStorage.getItem(fileName)
}

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
    console.log('data')
    const lang = 'Python3'
    const theme = loadEditorTheme()
    const file_list = this.file_list == null ? [] : this.file_list
    return {
      all_bot_languages: botLanguagePacks,
      status_message: null,
      logged_in: false,
      editorViewer: null,
      // Currently defaulting to Python3 starter kit
      bot_lang: lang,
      selected_language: lang,
      selected_theme: theme,
      file_list: file_list
    }
  },
  /*
  Run on view mount. Grab our user credentials from the API and then create editor.
  */
  mounted: function () {
    api.me().then((me) => {
      console.log(me)
      if (me !== null) {
        this.logged_in = true
      }
    })
    // Restore user's bot code, or use demo code for new bot
    this.load_code().then((code) => this.create_editor(code))
  },
  methods: {
    /* Return bot language specific info */
    bot_info: function () {
      return botLanguagePacks[this.bot_lang]
    },
    /* Init the Orion Editor */
    create_editor: function (code) {
      logInfo('Creating editor...')
      const codeEdit = new orion.codeEdit()
      var opts = {parent: 'embeddedEditor', contentType: this.bot_info().mimeType, contents: code}
      codeEdit.create(opts).then((editorViewer) => {
        logInfo('Initializing editor...')
        this.editorViewer = editorViewer
        const editor = editorViewer.editor

        // make sure we're using the correct color theme
        this.reset_theme()

        logInfo('Adding editor save handlers...')

        // Set up save action (note: auto-save is enabled by default)
        const saveEventHandler = () => {
          saveCode(this)
          return true
        }

        // Auto-save event
        editor.addEventListener('InputChanged', function (evt) {
          if (evt.contentsSaved) {
            // save editor contents
            saveEventHandler()
            // clear upload status message when user edits code
            if (this.status_message) {
              this.status_message = null
            }
          }
        })

        // Manual save action (Ctrl+S)
        editor.getTextView().setAction('save', saveEventHandler)

        // Be sure to save before closing/leaving the page
        window.addEventListener('unload', saveEventHandler)

        // Other settings
        if (editorViewer.settings) {
          editorViewer.settings.showOccurrences = true
        }

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
    file_selected: function(file_name) {
      for (a in this.file_list) {
        a.isActive = file_name == a.file_name;
      }
    }
    load_code: function () {
      // Restore user's bot code, or use demo code for new bot
      const startCode = loadCodeFromLocalStorage(this)
      console.log(startCode)
      return (startCode === null) ? this.load_default_code() : Promise.resolve(startCode)
    },
    /* Return MyBot file of the starter bot in string format */
    load_default_code: function () {
      logInfo('Loading default bot code...')
      const fileName = this.bot_info().fileName
      return this.get_starter_zip().then((starterZip) => {
        logInfo('Got starter zip. Getting sample bot file: ' + fileName)
        console.log(starterZip.files)
        this.set_file_list_zip(starterZip)
        return starterZip.file(fileName).async('text')
      }, logError)
    },
    /* Return a string of the code currently in the editor */
    get_editor_code: function () {
      console.log('test')
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
      const codePromise = use_default ? this.get_default_code_promise() : this.load_code()
      return codePromise.then((contents) => {
        this.set_editor_contents(contents)
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
    /* Set the contents of our edior */
    set_editor_contents: function(contents) {
      this.editorViewer.setContents(contents, this.bot_info().mimeType)
    },
    set_file_list_zip: function(zip_file) {
      this.file_list = Object.keys(zip_file.files).map(function(key, index) {
        let f = zip_file.files[key]
        return {file_name: f.name, isActive: f.name == this.bot_info().file_name }
      }, this)
    },
    /* Submit bot to our leaderboard */
    submit_bot: function () {
      console.log('... starting upload')
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
            console.log('upload successully')
            this.status_message = 'Successfully uploaded!'
            resolve();
          }, (err) => {
            console.log('error: ' + err.message)
            this.status_message = err.message
            reject(err.message);
          })
        })
      })
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
    height: 25em;
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
  .row, .col-12, .editorArea, .container-fluid, .editorBody {
    min-height: 80vh;
    height: 80vh;
  }
</style>
