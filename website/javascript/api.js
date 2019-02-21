/* jshint esversion: 6 */
import jdenticon from 'jdenticon'
import md5 from 'md5'

export const API_SERVER_URL = api_server_url
export const LOGIN_SERVER_URL = login_server_url

export function me_cached () {
  return null
}

export function me () {
  return Promise.resolve(null)
}

export function get_user (user_id) {
  return $.get({
    url: `${API_SERVER_URL}/user/${user_id}`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function get_team(team_id) {
  return window.fetch(`${API_SERVER_URL}/team/${team_id}`, {
    credentials: 'include',
  }).then(r => r.json())
}

export function list_bots (user_id) {
  return $.get({
    url: `${API_SERVER_URL}/user/${user_id}/bot`
  })
}

export function makeRequest () {
  const xhr = new XMLHttpRequest()
  return xhr
}

export function list_organizations (user_id) {
  return $.get({
    url: `${API_SERVER_URL}/organization`
  })
}

export function get_season1_stats (userId) {
  return $.get({
    url: `${API_SERVER_URL}/user/${userId}/season1`,
    xhrFields: {
      withCredentials: true
    }
  })
}
export function get_season2_stats (userId) {
  return $.get({
    url: `${API_SERVER_URL}/user/${userId}/season2`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function get_replay (game_id, progress_callback) {
  let game_data_promise = Promise.resolve($.get(`${API_SERVER_URL}/user/0/match/${game_id}`))
  let replay_promise = new Promise((resolve, reject) => {
    const xhr = makeRequest()
    xhr.withCredentials = true
    xhr.open('GET', `${API_SERVER_URL}/user/0/match/${game_id}/replay`, true)
    xhr.responseType = 'arraybuffer'

    if (progress_callback) {
      xhr.onprogress = function (e) {
        progress_callback(e.loaded, e.total)
      }
    }

    xhr.onload = function (e) {
      if (this.status === 200) {
        const blob = this.response
        resolve(blob)
      } else {
        reject()
      }
    }

    xhr.onerror = function () {
      reject()
    }

    xhr.onreadystatechange = function (e) {
      if (xhr.readyState === 4) {
        if (xhr.status !== 200) {
          reject()
        }
      }
    }

    xhr.send()
  })
  return Promise.all([game_data_promise, replay_promise]).then(([game, replay]) => {
    return {
      game: game,
      replay: replay
    }
  })
}

export function get_expired_replay (replay_class, replay_name) {
  return new Promise((resolve, reject) => {
    const xhr = makeRequest()
    xhr.withCredentials = true
    xhr.open('GET', `${API_SERVER_URL}/replay/class/${replay_class}/name/${replay_name}`, true)
    xhr.responseType = 'arraybuffer'

    xhr.onload = function (e) {
      if (this.status === 200) {
        const blob = this.response
        resolve(blob)
      } else {
        reject()
      }
    }

    xhr.onerror = function () {
      reject()
    }

    xhr.onreadystatechange = function (e) {
      if (xhr.readyState === 4) {
        if (xhr.status !== 200) {
          reject()
        }
      }
    }

    xhr.send()
  })
}

export function leaderboard (filters, offset = null, limit = null) {
  let url = `${API_SERVER_URL}/leaderboard`
  let fields = {}

  const querystring = []
  if (offset !== null && limit !== null) {
    querystring.push(`offset=${offset}&limit=${limit}`)
  }
  if (filters && filters.length > 0) {
    filters = filters.map(window.encodeURIComponent);
    querystring.push(`filter=${filters.join('&filter=')}`)
  }
  if (querystring.length > 0) {
    url += `?${querystring.join('&')}`
  }
  return $.get({
    url: url,
    xhrFields: fields
  })
}

let __organizationLeaderboard = null;

export function organizationLeaderboard(filters, offset=null, limit=null) {
  if (__organizationLeaderboard === null) {
    __organizationLeaderboard = window.fetch('/assets/static-data/organization-leaderboard.json').then(r => r.json());
  }
  return __organizationLeaderboard;
}

export function get_user_history (userId) {
  return $.get({
    url: `${API_SERVER_URL}/user/${userId}/history`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function challenge(user_id, opponents) {
  return $.post({
    url: `${API_SERVER_URL}/user/` + user_id + `/challenge`,
    method: 'POST',
    data: JSON.stringify({opponents: opponents}),
    contentType: 'application/json',
    xhrFields: {
      withCredentials: true
    }
  })
}

/**
 * @returns {Promise} A Promise that resolves to a list of team
 * objects, each of which has an 'id', 'created', and 'name' field.
 */
export function list_teams() {
  return window.fetch(`${API_SERVER_URL}/team`, {
    method: 'GET',
  }).then(r => r.json())
}

export function fallbackAvatar(username) {
  // Custom identicon style
  // https://jdenticon.com/icon-designer.html?config=ffffffff01416400154b194b
  jdenticon.config = {
    lightness: {
      color: [0.21, 0.75],
      grayscale: [0.26, 0.75],
    },
    saturation: {
      color: 1.00,
      grayscale: 0.00,
    },
    backColor: "#ffffffff",
  }

  const identicon = window.encodeURIComponent(jdenticon.toSvg(md5(username), 100))
  return `data:image/svg+xml;utf8,${identicon}`
}
