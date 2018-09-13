// https://vuejs.org/v2/guide/state-management.html
import * as api from './api'

export default {
  editor: {
    expandTab: true,
    tabWidth: 4
  },
  mapSize: 32,
  opponent: 'self',
  BOT_OPTIONS: {'self': 'Self', 'live': 'Currently_Uploaded_Bot'},
  save () {
    const saved = Object.assign({}, this)
    delete saved.BOT_OPTIONS

    window.localStorage['editor'] = JSON.stringify(saved)
  },
  load () {
    return api.get_editor_bot_list().then((bots) => {
      for (const bot of bots) {
        this.BOT_OPTIONS[bot] = bot
      }

      try {
        const loaded = JSON.parse(window.localStorage.editor)
        Object.assign(this, loaded)
      } catch (e) {
      }
    })
  }
}
