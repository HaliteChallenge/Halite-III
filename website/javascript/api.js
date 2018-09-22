/* jshint esversion: 6 */

export const API_SERVER_URL = api_server_url
export const LOGIN_SERVER_URL = login_server_url
export const LOGOUT_SERVER_URL = logout_server_url

let cached_me = null
let logged_in = null

export function me_cached () {
  if (cached_me) {
    return {
      user_id: cached_me['user_id'],
      username: cached_me['username']
    }
  } else if (window.localStorage['cache']) {
    return {
      user_id: window.localStorage['user_id'],
      username: window.localStorage['username']
    }
  } else {
    return null
  }
}

export function me () {
  if (cached_me !== null) return Promise.resolve(cached_me)
  else if (logged_in === false) return Promise.resolve(null)
  return $.get({
    url: `${LOGIN_SERVER_URL}/me`,
    xhrFields: {
      withCredentials: true
    }
  }).then((me) => {
    if (me === null) {
      logged_in = false
      return null
    }
    logged_in = true
    return get_user(me.user_id).then((user) => {
      cached_me = user
      window.localStorage['cache'] = Date.now()
      window.localStorage['user_id'] = user.user_id
      window.localStorage['username'] = user.username
      return user
    })
  })
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

export function logout () {
  return $.post({
    url: `${LOGOUT_SERVER_URL}`,
    xhrFields: {
      withCredentials: true
    }
  }).then((res) => {
    logged_in = false
    window.localStorage.removeItem('cache')
    window.localStorage.removeItem('user_id')
    window.localStorage.removeItem('username')
  })
}

export function make_profile_image_url (username) {
  return `https://github.com/${username}.png`
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

export function update_bot (user_id, bot_id, file, progress_callback) {
  const method = bot_id === null ? 'POST' : 'PUT'
  const endpoint = bot_id === null ? 'bot' : `bot/${bot_id}`
  const xhr = makeRequest()
  xhr.upload.addEventListener('progress', function (e) {
    if (e.lengthComputable) {
      progress_callback(e.loaded / e.total)
    }
  }, false)
  xhr.upload.addEventListener('load', function (e) {
    progress_callback(1)
  }, false)
  xhr.withCredentials = true
  xhr.open(method, `${API_SERVER_URL}/user/${user_id}/${endpoint}`)

  const form_data = new FormData()
  form_data.append('name', 'botFile')
  form_data.append('botFile', file)

  xhr.send(form_data)

  return new Promise((resolve, reject) => {
    xhr.onload = function (e) {
      if (this.status === 200 || this.status == 201) {
        resolve()
      } else {
        const response = JSON.parse(e.target.responseText)
        reject(response)
      }
    }
  })
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

export function get_editor_file_list (userId) {
  const url = `${API_SERVER_URL}/editor/${userId}`
  return window.fetch(url, {
    credentials: 'include'
  }).then(r => r.json())
}

export function get_editor_bot_list () {
  const url = `${API_SERVER_URL}/editor/opponents`
  return window.fetch(url, {
    credentials: 'include'
  }).then(r => r.json())
}

export function get_editor_file(userId, fileName) {
  const url = `${API_SERVER_URL}/editor/${userId}/file/${encodeURIComponent(fileName)}`;
  return window.fetch(url, {
    credentials: 'include'
  }).then(r => r.text())
}

export function create_editor_file_space (userId, language) {
  return $.get({
    url: `${API_SERVER_URL}/editor/${userId}/${language}`,
    method: 'POST',
    xhrFields: {
      withCredentials: true
    }
  })
}

export function update_source_file (user_id, file_name, file_contents, progress_callback) {
  const xhr = makeRequest()
  xhr.upload.addEventListener('progress', function (e) {
    if (e.lengthComputable) {
      progress_callback(e.loaded / e.total)
    }
  }, false)
  xhr.upload.addEventListener('load', function (e) {
    progress_callback(1)
  }, false)
  xhr.withCredentials = true
  xhr.open('POST', `${API_SERVER_URL}/editor/${user_id}/file/${encodeURIComponent(file_name)}`)

  const form_data = new FormData()
  form_data.append('name', 'sourceFile')
  form_data.append('sourceFile', new Blob([file_contents]), 'sourceFile')

  xhr.send(form_data)

  return new Promise((resolve, reject) => {
    xhr.onload = function (e) {
      if (this.status === 200 || this.status == 201) {
        resolve()
      } else {
        const response = JSON.parse(e.target.responseText)
        reject(response)
      }
    }
  })
}

export function delete_source_file (userId, file_name) {
  return $.get({
    url: `${API_SERVER_URL}/editor/${userId}/file/${encodeURIComponent(file_name)}`,
    method: 'DELETE',
    xhrFields: {
      withCredentials: true
    }
  })
}

export function delete_editor_files(userId) {
  return window.fetch(`${API_SERVER_URL}/editor/${userId}`, {
    method: 'DELETE',
    credentials: 'include'
  }).then(r => r.json())
}

export function start_ondemand_task(userId, options) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}`, {
    method: 'POST',
    headers: {
      "Content-Type": 'application/json'
    },
    body: JSON.stringify(options),
    credentials: 'include'
  }).then(r => r.json())
}

export function update_ondemand_task(userId, options) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}`, {
    method: 'PUT',
    headers: {
      "Content-Type": 'application/json'
    },
    body: JSON.stringify(options),
    credentials: 'include'
  }).then(r => r.json())
}

export function get_ondemand_status(userId) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}`, {
    method: 'GET',
    credentials: 'include'
  }).then(r => r.json())
}

export function get_ondemand_replay(userId) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}/replay?_=${new Date().getTime()}`, {
    method: 'GET',
    credentials: 'include'
  }).then(r => r.arrayBuffer())
}

export function get_ondemand_error_log(userId) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}/error_log?_=${new Date().getTime()}`, {
    method: 'GET',
    credentials: 'include'
  }).then(r => r.text())
}

export function get_ondemand_bot_log(userId) {
  return window.fetch(`${API_SERVER_URL}/ondemand/${userId}/bot_log?_=${new Date().getTime()}`, {
    method: 'GET',
    credentials: 'include'
  }).then(r => r.text())
}

export function get_bot_zip(userId, botId) {
  return window.fetch(`${API_SERVER_URL}/user/${userId}/bot/${botId}`, {
    method: 'GET',
    credentials: 'include',
    headers: {
      'Accept': 'application/zip'
    }
  }).then(r => r.blob())
}

export function register_me (data) {
  return $.post({
    url: `${API_SERVER_URL}/user`,
    data: JSON.stringify(data),
    contentType: 'application/json',
    xhrFields: {
      withCredentials: true
    }
  })
}

export function update_me (user_id, data) {
  return $.ajax({
    url: `${API_SERVER_URL}/user/${user_id}`,
    method: 'PUT',
    data: JSON.stringify(data),
    contentType: 'application/json',
    xhrFields: {
      withCredentials: true
    }
  })
}

export function resend_verification_email(user_id) {
    return $.ajax({
        url: `${API_SERVER_URL}/user/${user_id}/verify/resend`,
        method: "POST",
        xhrFields: {
            withCredentials: true,
        },
    });
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

export function leaderboard (filters, hackathon = null, offset = null, limit = null) {
  let url = `${API_SERVER_URL}/leaderboard`
  let fields = {}
  if (hackathon) {
    url = `${API_SERVER_URL}/hackathon/${hackathon}/leaderboard`
    fields.withCredentials = true
  }

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

export function organizationLeaderboard(filters, offset=null, limit=null) {
  let url = `${API_SERVER_URL}/leaderboard/organization`
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
  return window.fetch(url).then(r => r.json())
}

export function reset_api_key () {
  return $.post({
    url: `${API_SERVER_URL}/api_key`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function register_hackathon (code) {
  const me = me_cached()
  if (!me) {
    return Promise.reject({
      message: 'You must be logged in to register for a hackathon.'
    })
  }
  return $.post({
    url: `${API_SERVER_URL}/user/${me.user_id}/hackathon`,
    data: {
      verification_code: code
    },
    xhrFields: {
      withCredentials: true
    }
  })
}

export function get_hackathon (id) {
  return $.get({
    url: `${API_SERVER_URL}/hackathon/${id}`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function get_user_hackathons (userId) {
  return $.get({
    url: `${API_SERVER_URL}/user/${userId}/hackathon`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function get_hackathons () {
  return $.get({
    url: `${API_SERVER_URL}/hackathon`,
    xhrFields: {
    }
  })
}

export function get_user_history (userId) {
  return $.get({
    url: `${API_SERVER_URL}/user/${userId}/history`,
    xhrFields: {
      withCredentials: true
    }
  })
}

export function invite_friend (email) {
  return $.post({
    url: `${API_SERVER_URL}/invitation/user/` + email
  })
}

export function subscribe (email) {
  return $.post({
    url: `${API_SERVER_URL}/user/addsubscriber/` + email
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

/** leagues **/
export function getLeaguesList(){
  return $.get({
    url: `${API_SERVER_URL}/leagues` ,
    xhrFields: {
    }
  });
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

/**
 * Create a new team.
 *
 * @param name - The team name. NOTE THAT THE SERVER WILL ADD 'Team'
 * TO THE TEAM NAME FOR YOU. So if you want to be 'Team Rocket', the
 * user should just provide 'Rocket'.
 *
 * @returns {Promise} A Promise that will resolve to an object with
 * two fields: the `team_id` and a `verification_code` that other
 * players will use to join this team. The user must have both the
 * team ID and verification code - to make it easier on the user, you
 * can join the two together with an @ and parse it on the client. (So
 * for example, tell the user their code is 5@aioseu38952, and when
 * they join, split the code on the '@' and call join_team below.)
 *
 * If the team is invalid, then the object will instead contain a
 * status and message field.
 */
export function create_team(name) {
  return window.fetch(`${API_SERVER_URL}/team`, {
    method: 'POST',
    headers: {
      "Content-Type": 'application/json'
    },
    body: JSON.stringify({ name }),
    credentials: 'include'
  }).then(r => r.json())
}

/**
 * Join a team.
 *
 * @param team_id
 * @param verification_code
 *
 * @returns {Promise}
 */
export function join_team(team_id, verification_code) {
  const formData = new FormData()
  formData.append('verification_code', verification_code)
  return window.fetch(`${API_SERVER_URL}/team/${team_id}/user`, {
    method: 'POST',
    body: formData,
    credentials: 'include'
  }).then(r => r.json())
}
