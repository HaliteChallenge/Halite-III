<template>
  <div class="leaderboard-container">
    <ul class="nav nav-tabs" role="tablist">
      <li role="presentation" class="active">
        <a href="#players" @click="refreshStickyTable" aria-controls="players" role="tab" data-toggle="tab">
          <span>PLAYERS ({{ metric.players }})</span>
        </a>
      </li>
      <li role="presentation">
        <a href="#organizations" @click="refreshStickyTable" aria-controls="organizations" role="tab" data-toggle="tab">
          <span>ORGANIZATIONS ({{ metric.organizations }})</span>
        </a>
      </li>
    </ul>
    <div class="tab-content">
      <div role="tabpanel" class="tab-pane active" id="players">
        <div class="panel panel-stats">
          <div class="panel-heading" role="tab" id="heading_player_details">
            <a data-toggle="collapse" id="toggle_filter" aria-controls="panel_filter">
              <h4 class="title-h4">Filters</h4>
            </a>
            <div class="filter-handler" v-if="filter_handle_view==='normal'">
              <a href="#" class="handler-item" @click="clearFilter(1)">
                <span class="handler-item-img icon-remove"></span>
                <span class="handler-item-text">Clear all</span>
              </a>
            </div>
          </div>
          <div class="panel-collapse collapse in" role="tabpanel" aria-labelledby="panel_filter">
            <form class="leaderboard-filter-form" v-on:submit="on_update_filter">
              <div class="form-header">
              </div>
              <div class="filter-group">
                <div class="input-group">
                  <v-select multiple placeholder="Player" v-model="username_filter" :options="filter_options.usernames_options">
                  </v-select>
                  <v-select multiple placeholder="Tier" v-model="tier_filter" :options="tiers">
                  </v-select>
                  <v-select multiple placeholder="Level" v-model="level_filter" :options="levels">
                  </v-select>
                  <v-select multiple placeholder="Country" v-model="country_filter" :options="filter_options.country_options">
                  </v-select>
                  <v-select multiple placeholder="Organization" v-model="organization_filter" :options="filter_options.org_options">
                  </v-select>
                  <v-select multiple placeholder="Language" v-model="language_filter" :options="filter_options.language_options">
                  </v-select>
                </div>
              </div>
            </form>
          </div>
        </div>
        <div v-if="leaderboard.length > 0">
          <div class="table-container">
            <table class="table">
              <thead>
                <tr>
                  <th class="text-center">Rank</th>
                  <th>Player</th>
                  <th>Rating</th>
                  <th class="text-center">Tier</th>
                  <th>Level</th>
                  <th class="text-center">Country</th>
                  <th>Organization</th>
                  <th>Language</th>
                  <th>Last Submission</th>
                </tr>
              </thead>
              <tbody>
                <tr v-for="player in leaderboard"
                    :id="`user-row-${player.user_id}`"
                    :key="player.user_id"
                    :class="{ 'hl': show_user && show_user.includes(player.user_id.toString()) }">
                  <td class="text-center">{{ player.rank || player.local_rank }}</td>
                  <td class="nowrap">
                    <a :href="'/user?user_id=' + player.user_id" class="leaderboard-name" v-bind:title="player_link_title(player)">
                      <template v-if="player.user_id === player.team_leader_id && player.team_members.length > 0">
                        <profile-image
                          v-for="team_member in player.team_members"
                          :key="team_member.username"
                          width="30"
                          height="30"
                          :username="team_member.username"
                          :profileImage="team_member.profile_image_key"
                        />
                        {{ player.team_name }}
                      </template>
                      <template v-else-if="player.team_members.length > 0">
                        <profile-image
                          width="30"
                          height="30"
                          :username="player.username"
                          :profileImage="player.profile_image_key"
                        />
                        {{ player.username }} ({{player.team_name}})
                      </template>
                      <template v-else>
                        <profile-image
                          width="30"
                          height="30"
                          :username="player.username"
                          :profileImage="player.profile_image_key"
                        />
                        {{ player.username }}
                      </template>
                    </a>
                  </td>
                  <td :title="`${player.rating_components}`">{{ Math.round(100 * player.score) / 100 }}</td>
                  <td class="text-center tier-td">
                    <TierPopover :tier="tierClass(player.tier || player.local_tier)" />
                  </td>
                  <td>{{ player.level }}</td>
                  <td class="text-center">
                    <div v-if="player.user_id === player.team_leader_id && player.team_members.length > 0">
                      <img
                            v-for="team_member in player.team_members"
                            v-if="getCountry(team_member.country)"
                            :title="`${getCountryName(team_member.country)}`"
                            :src="`${getCountry(team_member.country)}`"
                            class="country-img">
                        </div>
                      <div v-else>
                        <img v-if="getCountry(player.country)" :title="`${getCountryName(player.country)}`" :src="`${getCountry(player.country)}`" class="country-img">
                        </div>
                  </td>
                  <td style="width:220px">
                    <template v-if="player.user_id === player.team_leader_id && player.team_members.length > 0">
                      {{ player.team_members.filter(tm => tm.organization).map(tm => tm.organization).join(", ") }}
                    </template>
                    <template v-else>
                      {{ player.organization }}
                    </template>
                  </td>
                  <td>{{ player.language }}</td>
                  <td :title="`${player.num_games} games`">
                    {{ getFormattedDate(player.update_time) }}
                    {{ player.compile_status == "Successful" ? "v"+player.version_number : player.compile_status }}
                  </td>
                </tr>
              </tbody>
            </table>
          </div>
          <div class="leaderboard-page" v-if="isDefaultLimit">
            <HalitePagination :page="this.page" :lastPage="this.lastPage" :baseUrl="this.baseUrl" :changePage="this.changePage" />
          </div>
        </div>
        <div v-else class="zero-state-pane">
          <img src="/assets/images/leaderboard-zero-icon.png" alt="" />
          <div class="zero-state-title">NOTHING TO SHOW</div>
          <div>Please try adjusting or removing your selection criteria.</div>
        </div>
      </div>
      <div role="tabpanel" class="tab-pane organizations" id="organizations">
        <div v-if="filter_org_list.length">
          <div class="table-container">
            <table class="table">
              <thead>
                <tr>
                  <th class="text-center">RANK</th>
                  <th>ORGANIZATION</th>
                  <th>Games Won (during finals)</th>
                  <th>LEVEL</th>
                </tr>
              </thead>
              <tbody>
                <tr :id="`org-row-${org.organization_id}`" :key="org.organization_id" v-for="org in filter_org_list">
                  <td class="text-center">{{ org.organization_rank }}</td>
                  <td class="nowrap">
                    <a :href="`/programming-competition-leaderboard?organization=${org.organization_id}`">
                      {{ org.organization_name }}
                    </a>
                  </td>
                  <td>{{ org.weekly_games_won }}</td>
                  <td>{{ org.organization_kind }}</td>
                </tr>
              </tbody>
            </table>
          </div>
        </div>
        <div v-else class="zero-state-pane">
          <img src="/assets/images/leaderboard-zero-icon.png" alt="" />
          <div class="zero-state-title">NOTHING TO SHOW</div>
          <div>Please try adjusting or removing your selection criteria.</div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import Vue from "vue";
import * as api from "../api";
import HalitePagination from "./Pagination.vue";
import TierPopover from "./TierPopover.vue";
import { tierClass, countries_data } from "../utils";
import vSelect from "vue-select";
import _ from "lodash";
import moment from "moment";
import dateformat from "dateformat";

const DEFAULT_LIMIT = 100;

export default {
  name: "leaderboard",
  props: ["baseUrl", "lbFromContainer", "metric"],
  components: {
    HalitePagination,
    vSelect,
    TierPopover,
  },
  data: function() {
    const countries = countries_data;
    let country_options = [];
    countries.forEach(item => {
      country_options.push({ value: item["alpha-3"], label: item.name });
    });
    return {
      show_user: null,
      contry_data: countries_data,
      countries: country_options,
      leaderboard: [],
      username_filter: "",
      tier_filter: "",
      organization_filter: "",
      country_filter: "",
      language_filter: "",
      level_filter: "",
      org_name_filter: "",
      org_level_filter: "",
      page: 1,
      limit: DEFAULT_LIMIT,
      org_limit: DEFAULT_LIMIT,
      lastPage: 0,
      org_page: 1,
      org_last_page: 0,
      organizations: [],
      filter_org_list: [],
      classes: {
        professional: 0,
        university: 0,
        high_school: 0
      },
      filter_handle_view: "normal",
      users: [],
      all_leaderboards: [],
      params: [],
      summary: [
        {
          icon: "medal",
          number: 23,
          name: "Professional"
        },
        {
          icon: "hat",
          number: 23,
          name: "University"
        },
        {
          icon: "bag",
          number: 23,
          name: "High School"
        }
      ],
      levels: [
        {
          value: "Professional",
          label: "Professional"
        },
        {
          value: "University",
          label: "University"
        },
        {
          value: "High School",
          label: "High School"
        }
      ],
      tiers: [
        {
          label: "Diamond",
          value: 1
        },
        {
          label: "Platinum",
          value: 2
        },
        {
          label: "Gold",
          value: 3
        },
        {
          label: "Silver",
          value: 4
        },
        {
          label: "Bronze",
          value: 5
        }
      ],
      filter_name: "",
      selected_filter: null,
      build_params_count: 0,
    };
  },
  mounted: function() {
    // start setup filter list
    this.calculate_filters();

    // Get all organizational data
    this.update_org_filter();

    // determine if the filter should be collapsed or not
    // this.setupCollapseFilter()  // issue #409
    $(window).on("popstate", () => {
      this.build_params_count = -1;
      this.calculate_filters();
    });
  },
  updated() {
    // scroll to user
    if (this.show_user) {
      console.log(this.show_user)
      const id = $(`#user-row-${this.show_user}`);
      if (id.length === 0) {
        return;
      }
      const offset = 60; // the header height
      $("body, html").scrollTop(id.offset().top - offset);

      // highlight
      id.addClass("hl");
    }
  },
  computed: {
    saved_filters: function() {
      let saved_filters = JSON.parse(localStorage.saved_filters);

      return saved_filters || [];
    },

    // getting the filter options
    filter_options: function() {
      let filters = {
        language_options: [],
        country_options: [],
        org_options: [],
        usernames_options: []
      };

      let language_options = [];
      let country_codes = [];
      let organization_ids = [];
      let org_options = [];
      let username_options = [];

      this.all_leaderboards.forEach(user => {
        if (user.language && language_options.indexOf(user.language) == -1) {
          language_options.push(user.language);
        }
        if (user.country && country_codes.indexOf(user.country) == -1) {
          country_codes.push(user.country);
        }
        if (
          user.organization_id &&
          organization_ids.indexOf(user.organization_id) == -1
        ) {
          organization_ids.push(user.organization_id);
          org_options.push({
            label: user.organization,
            value: user.organization_id
          });
        }
        username_options.push(user.username);
      });

      // filter countries
      // const country_options = [];
      const country_options = countries_data
        .filter(country => {
          if (country_codes.indexOf(country["alpha-3"]) !== -1) {
          }
          return country_codes.indexOf(country["alpha-3"]) != -1;
        })
        .map(country => {
          return {
            label: country.name,
            value: country["alpha-3"]
          };
        });

      filters.language_options = language_options;
      filters.country_codes = country_codes;
      filters.country_options = country_options;
      filters.org_options = org_options.sort(function(a, b) {
        return a.label.localeCompare(b.label);
      });
      filters.usernames_options = username_options;

      return filters;
    },

    // return if limit is set default value
    isDefaultLimit: function() {
      return this.limit == DEFAULT_LIMIT;
    },

    // return if organization limit is set default value
    isDefaultOrgLimit: function() {
      return this.org_limit == DEFAULT_LIMIT;
    },

    // getting the organizations filter options
    filter_org_options: function() {
      let filters = {
          name_options: [],
          level_options: []
        },
        name_options = [],
        level_options = [];
      this.organizations.forEach(org => {
        const { organization_name, organization_kind} = org;
        name_options.push(organization_name);
        if(!level_options.includes(organization_kind)) {
          level_options.push(organization_kind);
        }
      });
      filters.name_options = name_options;
      filters.level_options = level_options;
      return filters;
    }
  },
  watch: {
    username_filter: function() {
      this.on_update_filter();
    },
    tier_filter: function() {
      this.on_update_filter();
    },
    level_filter: function() {
      this.on_update_filter();
    },
    organization_filter: function() {
      this.on_update_filter();
    },
    country_filter: function() {
      this.on_update_filter();
    },
    language_filter: function() {
      this.on_update_filter();
    },
    org_name_filter: function() {
      this.on_update_filter2();
    },
    org_level_filter: function() {
      this.on_update_filter2();
    }
  },
  methods: {
    tierClass: tierClass,
    build_filters_from_url: function() {
      let params = {};
      if (!window.location.search.slice(1)) {
        this.page = 1;
        this.org_page = 1;
        return;
      }

      // extract filter term to objects
      window.location.search
        .slice(1)
        .split("&")
        .forEach(item => {
          let param = item.split("=");
          params[param[0]] = param[1].split(",");
        });

      // get jump user ID value
      if (params.show_user && params.show_user.length > 0) {
        this.show_user = params.show_user;
      }

      // get organization name value
      if (params.organization_name && params.organization_name.length > 0) {
        let organization_name = params.organization_name.map(item => {
          return decodeURIComponent(item);
        });
        this.org_name_filter = organization_name;
      }
      // get organization level value
      if (params.org_kind && params.org_kind.length > 0) {
        this.org_level_filter = params.org_kind;
      }

      // get username value
      if (params.username && params.username.length > 0) {
        this.username_filter = params.username;
      }

      // get level value
      if (params.level && params.level.length > 0) {
        let selected = this.levels.filter(item => {
          return params.level.indexOf(item.value + "") != -1;
        });
        this.level_filter = selected;
      }

      // get tier value
      if (params.tier && params.tier.length > 0) {
        let selected = this.tiers.filter(item => {
          return params.tier.indexOf(item.label + "") != -1;
        });
        this.tier_filter = selected;
      }

      // get organization
      if (params.organization && params.organization.length > 0) {
        let selected = this.filter_options.org_options.filter(item => {
          // return item.value == 16872;
          return params.organization.indexOf(item.value + "") != -1; // convert to string
        });
        this.organization_filter = selected;
      }

      // get country
      if (params.country && params.country.length > 0) {
        let selected = this.filter_options.country_options.filter(item => {
          return params.country.indexOf(item.value + "") != -1; // convert to string
        });
        this.country_filter = selected;
      }

      // get country
      if (params.language && params.language.length > 0) {
        let selected = this.filter_options.language_options.filter(item => {
          return params.language.indexOf(item + "") != -1; // convert to string
        });
        this.language_filter = selected;
      }

      // get page limit
      if (params.limit && params.limit.length > 0) {
        this.limit = params.limit;
      }

      // get organization page limit

      if (params.org_limit && params.org_limit.length > 0) {
        this.org_limit = params.org_limit;
      }

      // page
      if (params.page) {
        this.page = parseInt(params.page[0]);
      }

      // org_page
      if (params.org_page) {
        this.org_page = parseInt(params.org_page[0]);
      }
    },

    // type = 1 player filter , type = 2 organization filter
    build_filter: function(type) {
      let filters = [];
      let org_filters = [];
      let params = {};
      // adding the organization name filter
      if (this.org_name_filter.length > 0) {
        params["organization_name"] = [];
        this.org_name_filter.forEach(function(item) {
          org_filters.push("organization_name,=," + item);
          params["organization_name"].push(item);
        });
      }

      // adding the organization level filter
      if (this.org_level_filter.length > 0) {
        params["org_kind"] = [];
        this.org_level_filter.forEach(function(item) {
          org_filters.push("organization_kind,=," + item);
          params["org_kind"].push(item);
        });
      }

      // adding the username filter
      if (this.username_filter.length > 0) {
        params["username"] = [];
        this.username_filter.forEach(function(item) {
          filters.push("username,=," + item);
          params["username"].push(item);
        });
      }

      // adding the level filter
      if (this.level_filter.length > 0) {
        params["level"] = [];
        this.level_filter.forEach(function(item) {
          filters.push("level,=," + item.value);
          params["level"].push(item.value);
        });
      }

      // adding the tier filter
      if (this.tier_filter.length > 0) {
        let key = "tier";
        params["tier"] = [];
        this.tier_filter.forEach(function(item) {
          filters.push(key + ",=," + item.label);
          params["tier"].push(item.label);
        });
      }

      // adding the organization filter
      if (this.organization_filter && this.organization_filter.length > 0) {
        params["organization"] = [];
        this.organization_filter.forEach(function(item) {
          filters.push("organization_id,=," + item.value);
          params["organization"].push(item.value);
        });
      }

      // adding the country filter
      if (this.country_filter.length > 0) {
        params["country"] = [];
        this.country_filter.forEach(function(item) {
          filters.push("country_code,=," + item.value);
          params["country"].push(item.value);
        });
      }

      if (this.language_filter.length > 0) {
        params["language"] = [];
        this.language_filter.forEach(item => {
          filters.push("language,=," + item);
          params["language"].push(item);
        });
      }

      if (this.page > 1) {
        params["page"] = [this.page];
      }

      if (this.org_page > 1) {
        params["org_page"] = [this.org_page];
      }

      if (this.limit != DEFAULT_LIMIT) {
        params["limit"] = [this.limit];
      }

      if (this.org_limit != DEFAULT_LIMIT) {
        params["org_limit"] = [this.org_limit];
      }

      this.params = params;

      // build params in url
      this.build_params();
      const filter_list = type === 1 ? filters : org_filters;
      return filter_list.length ? filter_list : null;
    },

    // issue #409
    // toggleFilter: function () {
    //   setTimeout(() => {
    //     const collapsed = !$('#panel_filter').hasClass('in')
    //     this.$cookie.set('leaderboard_filter_collapsed', collapsed ? 1 : 0)
    //   }, 500)
    // },

    build_params: function() {
      const params = this.params;
      let path_name = "";
      // build params
      if (Object.entries(params).length > 0) {
        let query_string = [];
        _.forEach(params, function(items, key) {
          query_string.push(key + "=" + items.join());
        });
        path_name = window.location.pathname + "?" + query_string.join("&");
      } else {
        path_name = window.location.pathname;
      }
      if (this.build_params_count === 0) {
        window.history.replaceState(null, null, path_name);
      } else if (this.build_params_count !== -1) {
        window.history.pushState(null, null, path_name);
      }
      this.build_params_count = 1;
    },

    // issue #409
    // setupCollapseFilter: function () {
    //   const collapse = this.$cookie.get('leaderboard_filter_collapsed')
    //   if (collapse == 1 || window.mobileAndTabletcheck()) {
    //     $('#panel_filter').removeClass('in')
    //     $('#toggle_filter').attr('aria-expanded', 'false')
    //   }
    // },

    on_update_filter: function(e) {
      if (e && e.preventDefault) e.preventDefault();

      this.page = 1; // reset page number when reset filter
      this.update_filter(true); // apply filter
    },

    on_update_filter2: function(e) {
      if (e && e.preventDefault) e.preventDefault();

      this.org_page = 1; // reset page number when reset filter
      this.update_org_filter(true); // apply filter
    },

    // calculate filter list rendered in the filter bar
    calculate_filters: function() {
      // calculating filter list items doesn't need to apply any filter in the request
      // limit should be set to a big number in order to get all the items to calculate

      const handleLeaderboard = leaderboard => {
        const instance = this;
        leaderboard.forEach(function(user, index) {
          instance.users.push(user.username);
        });

        instance.users.sort();

        // save the users
        this.all_leaderboards = leaderboard;

        // apply filter. Firstly, setup filter items matched with the url,
        // Then apply searching items matched with the filter
        this.build_filters_from_url();

        // Find pagination of the goto user
        if (this.show_user) {
          let gotoIndex = -1;
          for (let i = 0; i < leaderboard.length; i++) {
            const user = leaderboard[i];
            if (user.user_id.toString() === this.show_user.toString()) {
              gotoIndex = i;
              break;
            }
          }
          if (gotoIndex > -1) {
            this.page = 1 + Math.floor(gotoIndex / this.limit);
          }
        }
        this.update_filter(true, true);
      };

      let leaderboard;
      if (this.lbFromContainer) {
        leaderboard = this.lbFromContainer;
        handleLeaderboard(leaderboard);
      } else {
        api.leaderboard([], 0, 99999).then(leaderboard => {
          handleLeaderboard(leaderboard);
        });
      }
    },
    update_org_filter: function(
      updatePageNumber = false,
      defaultFilter = false
    ) {
      api.organizationLeaderboard().then(org => {
        if (org && org instanceof Array) {
          this.organizations = org;
          this.filter_org_list = org;
        }
      });
    },
    update_filter: function(updatePageNumber = false, defaultFilter = false) {
      const filters = this.build_filter(1);
      if (updatePageNumber) {
        if (this.all_leaderboards && defaultFilter) {
          this.lastPage = Math.ceil(this.all_leaderboards.length / this.limit);
        } else {
          api
            .leaderboard(filters, 0, 999999)
            .then(leaderboard => {
              if (leaderboard && leaderboard instanceof Array) {
                this.lastPage = Math.ceil(leaderboard.length / this.limit);
              }
            });
        }
      }

      const handleLeaderboard = leaderboard => {
        for (const user of leaderboard) {
          let mu_round = Math.round(user.mu * 100) / 100;
          let sigma_round = Math.round(user.sigma * 1000) / 1000;
          user.rating_components = `${mu_round}μ ${sigma_round}σ`;
        }
        this.leaderboard = leaderboard;
      };

      if (this.all_leaderboards && defaultFilter) {
        handleLeaderboard(
          this.all_leaderboards.slice(
            (this.page - 1) * this.limit,
            this.page * this.limit
          )
        );
      } else {
        api
          .leaderboard(
            filters,
            (this.page - 1) * this.limit,
            this.limit
          )
          .then(leaderboard => {
            handleLeaderboard(leaderboard);
          });
      }
    },

    player_link_title(player) {
      if (player.team_members.length > 0) {
        return `${player.team_name}: ${player.team_members
          .map(tm => tm.username)
          .join(", ")}`;
      } else {
        return player.username;
      }
    },

    changePage: function(page) {
      this.page = page;
      this.update_filter();
    },
    getCountryName: function(name) {
      var countries = require("i18n-iso-countries");
      return countries.getName(name, "en");
    },
    // type = 1 player filter , type = 2 organization filter
    clearFilter: function(type) {
      if (type === 1) {
        this.username_filter = [];
        this.country_filter = [];
        this.organization_filter = [];
        this.tier_filter = [];
        this.update_filter();
      }
    },
    getCountry: function(name) {
      if (name) {
        var countries = require("i18n-iso-countries");
        return (
          "/assets/images/countries/" +
          countries
            .getAlpha2Code(countries.getName(name, "en"), "en")
            .toLowerCase() +
          ".png"
        );
      }
      return null;
    },
    getFormattedDate: function(date) {
      return moment(date).fromNow();
    },
    refreshStickyTable: function() {
      window.refreshStickyTable();
    },
  }
};
</script>

<style lang="scss" scoped>
.xline {
  &::before,
  &::after {
    display: none;
  }
}
.table-container {
  border-radius: 4px;
  overflow: hidden;
  background: linear-gradient(180deg, #edf6ff 0%, #ffffff 100%);
  table {
    margin: 0;
    thead {
      tr {
        background: rgba(2, 30, 82, 0.05);
        th {
          border-right: 1.2px solid rgba(8, 27, 83, 0.1);
          border-bottom: none;
          padding: 20px 15px;
          font-size: 14px;
        }
      }
    }
    tbody {
      tr td {
        border: 1.2px solid rgba(8, 27, 83, 0.1);
        padding: 20px 15px;
        color: #0c0c0c;
        font-size: 16px;
        .leaderboard-name > img {
          margin-right: 10px;
          margin-top: -3px;
          vertical-align: top;
        }
      }
      tr:first-child td {
        border-top: none;
      }
    }
  }
}
</style>
