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

let __users = {};
let __user_matches = null;
let __matches = null;

export function get_user (user_id) {
  const idx = user_id - (user_id % 1000);
  if (!__users[idx]) {
    __users[idx] = window.fetch(`/assets/static-data/users-${idx}.json`)
      .then(r => r.json())
      .then((users) => {
        return users;
      });
  }
  return __users[idx].then((users) => users[user_id]);
}

export function get_user_matches(user_id) {
  if (!__user_matches) {
    __user_matches = window.fetch(`/assets/static-data/user-matches.json`).then(r => r.json());
    __matches = window.fetch(`/assets/static-data/matches.json`).then(r => r.json());
  }

  return Promise.all([__user_matches, __matches, get_user(1)]).then(([user_matches, matches]) => {
    return Promise.all(user_matches[user_id].map(match_id => {
      const match = matches[match_id];
      const promises = [];
      for (const playerId of Object.keys(match.players)) {
        promises.push(get_user(playerId).then((user) => {
          match.players[playerId].username = user.username;
          match.players[playerId].profile_image_key = user.profile_image_key;
        }));
      }
      return Promise.all(promises).then(() => match);
    }));
  });
}

let __teams = null;

export function get_team(team_id) {
  if (!__teams) {
    __teams = window.fetch(`/assets/static-data/teams.json`).then(r => r.json());
  }
  return __teams.then((teams) => teams[team_id]);
}

export function makeRequest () {
  const xhr = new XMLHttpRequest()
  return xhr
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

let __leaderboard = null;

export function leaderboard (filters, offset = null, limit = null) {
  if (!__leaderboard) {
    __leaderboard = window.fetch('/assets/static-data/leaderboard.json')
      .then(r => r.json());
  }

  return __leaderboard.then((data) => {
    if (filters && filters.length > 0) {
      const conditions = [];
      for (const filter of filters) {
        let [ field, _, value ] = filter.split(/,/g);
        if (field === "organization_id") {
          value = parseInt(value, 10);
        }
        if (field === "country_code") {
          field = "country";
        }
        conditions.push([ field, value ]);
      }
      data = data.filter(player => {
        for (const [ field, value ] of conditions) {
          if (player[field] !== value) {
            return false;
          }
        }
        return true;
      });
    }

    if (offset) {
      data = data.slice(offset);
    }
    if (limit) {
      data = data.slice(0, limit);
    }
    return data;
  });
}

let __organizationLeaderboard = null;

export function organizationLeaderboard(filters, offset=null, limit=null) {
  if (__organizationLeaderboard === null) {
    __organizationLeaderboard = window.fetch('/assets/static-data/organization-leaderboard.json').then(r => r.json());
  }
  return __organizationLeaderboard;
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
