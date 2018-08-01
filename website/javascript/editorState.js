// https://vuejs.org/v2/guide/state-management.html

export default {
  editor: {
    expandTab: true,
    tabWidth: 4
  },
  mapSize: 32,
  opponent: 'self',
  BOT_OPTIONS: {'self': 'Self', '0': 'GoodBot'},
  save () {
    const saved = Object.assign({}, this)
    delete saved.BOT_OPTIONS

    window.localStorage['editor'] = JSON.stringify(saved)
  },
  load () {
    try {
      const loaded = JSON.parse(window.localStorage.editor)
      Object.assign(this, loaded)
    } catch (e) {

    }
  }
}
