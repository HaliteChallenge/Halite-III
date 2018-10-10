<template>
    <div class="associate-container edit-user-container">
        <div>
            <a href="/user?me" class="back-arrow"><img class="arrow" :src="`${baseUrl}/assets/images/temp/back_arrow.png`"/><span>Back to your profile</span></a>
        </div>
        <div class="row">
            <div class="col-md-6 col-xm-10 col-md-offset-3 col-xm-offset-1">
                <div class="page-header">
                    <a id="section_personal_info"></a>
                    <h1>Edit your profile</h1>
                </div>

                <h2 v-if="user && !user.is_email_good" class="form-heading">Resend Verification Mail</h2>
                <p v-if="user && !user.is_email_good">If you cant find our account verification mail, resend a verification mail to your mail now.</p>
                <br/>
                <button v-if="user && !user.is_email_good" class="btn btn-primary" v-on:click="resend_verification_email">Resend Verfication</button>

                <h2 class="form-heading">personal info</h2>
                <form v-on:submit.prevent="submit" class="create-account-form">
                    <p v-if="user && user.organization_id">You are currently affiliated with {{ user.organization }}.</p>
                    <p v-else>You are not currently affiliated with an organization.</p>
                    <br/>
                    <button
                        type="button"
                        class="btn btn-primary"
                        v-if="!edit_email"
                        v-on:click="edit_email = true">Edit Affiliation</button>
                    <template v-if="edit_email">
                        <div class="form-group">
                            <label for="level">Which of the following describes you best?<span class="text-danger">*</span></label>
                            <select class="form-control" id="level" v-model="level">
                                <option value="Professional">Professional</option>
                                <option value="University">University</option>
                                <option value="High School">High school</option>
                            </select>
                        </div>

                        <div class="form-group" v-if="level != 'High School'">
                            <label for="personal-email">Work or University Email</label>
                            <input class="form-control" type="email" id="personal-email" v-model="email" />
                            <p style="margin-top: 10px">We’ll use your email domain to affiliate you with your school or company, but we won’t share your email publicly.</p>
                        </div>
                        <div class="form-group" v-else>
                          <v-select
                            placeholder="Select School"
                            v-model="selected_highSchool"
                            :options="highSchools">
                          </v-select>
                          <p style="margin-top: 10px">If your school is not listed, please email us at <a href="mailto:halite@halite.io">halite@halite.io</a>.</p>
                        </div>
                    </template>

                    <div class="line-container"><i class="xline xline-top"></i></div>

                    <h2 id="section_account_info" class="form-heading">Account info</h2>

                    <div class="form-group">
                        <label for="username">Your username</label>
                        <div class="relative-container">
                            <input id="username" type="text" class="form-control" placeholder="Enter your username" v-model="username">
                        </div>
                        <div
                          :class="{'has-error': !username_error.valid}"
                          class="form-group"
                          v-if="username_error.reason">
                          <span id="error-help" class="help-block">{{ username_error.reason }}</span>
                        </div>
                    </div>

                    <div class="form-group">
                        <label for="country">Which country will you be playing from?</label>
                        <v-select
                            placeholder="(would prefer not to disclose)"
                            label="label"
                            :on-change="change_country"
                            v-model="selected_country"
                            :options="country_options">
                        </v-select>
                    </div>

                    <div class="form-group" v-if="selected_country">
                        <label for="region">State, Province, or Region</label>
                        <v-select
                            placeholder="(would prefer not to disclose)"
                            v-model="selected_region"
                            label="label"
                            :options="regions">
                        </v-select>
                    </div>
                    <div class="form-group has-error" v-if="error">
                        <span id="error-help" class="help-block">{{ error }}</span>
                    </div>
                    <a class="cancel-href base" href="/user/?me" target="_self">Cancel</a>
                    <button type="submit" class="btn btn-primary btn-sm">Update Profile</button>
                </form>

                <!-- issue #408 Add ability for player to create or join a team -->
                <div class="team-panel">
                  <i class="xline xline-top"></i>
                  <div class="no-team" v-if="!team">
                    <h2 class="form-heading">TEAM AFFILIATION</h2>
                    <p>You are not affiliated with a team.</p>
                    <p>Creating or joining a team is a permanent account conversion! <a href="/play-programming-challenge#rules-teams">Learn more about teams.</a></p>
                    <h3>Create Team</h3>
                    <div class="form-group">
                      <label for="team-name">New Team Name</label>
                      <input id="team-name" type="text" class="form-control" placeholder="Team name" v-model="new_team_name">
                      <button class="btn btn-primary btn-sm" @click="createTeam">CREATE TEAM</button>
                      <p class="error" v-if="teamCreateError">{{teamCreateError}}</p>
                    </div>
                    <h3>Join a Team</h3>
                    <div class="form-group">
                      <label for="join-team-code">Enter join code here</label>
                      <input id="join-team-code" type="text" class="form-control" placeholder="Join code" v-model="join_team_name">
                      <button class="btn btn-primary btn-sm" @click="joinTeam">JOIN TEAM</button>
                      <p class="error" v-if="teamJoinError">{{teamJoinError}}</p>
                    </div>
                  </div>
                  <div class="in-team" v-else>
                    <h2 class="form-heading">Team affiliation</h2>
                    <div class="tips">You are on <strong>{{team.name}}</strong>.</div>
                    <template v-if="team.verification_code">
                      <h3>Invite Friends to Your Team</h3>
                      <div class="input-tips">Share code</div>
                      <input type="text" class="form-control" :value="`${team.team_id}@${team.verification_code}`">
                    </template>
                  </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
    import * as api from '../api'
    import vSelect from 'vue-select'
    import {Alert, countries_data} from '../utils'
    import * as utils from '../utils'

export default {
      name: 'EditUserProfile',
      components: {vSelect},
      props: ['baseUrl'],
      data: function () {
        return {
          countries: [],
          country_options: [],
          data: iso3166.data,
          username: '',
          originalUsername: '',
          selected_country: null,
          selected_region: null,
          selected_highSchool: null,
          level: 'Professional',
          organization: null,
          highSchools: [],
          email: '',
          error: null,
          primary: true,
          user: null,
          edit_email: false,
          team: null,  // My team information
          teamJoinError: null,
          teamCreateError: null,
          new_team_name: "", // Team name required to create the team
          join_team_name: "", // Team name required to join the team
          username_error: {
            valid: false,
            reason: '',
          },
          checkUsername: _.debounce(() => {
            api.check_username(this.username).then((resp) => {
              this.username_error.valid = resp.valid
              this.username_error.reason = resp.reason
            })
          }, 750),
        }
  },
    mounted: function () {
      const countries = Object.entries(iso3166.data)
      countries.sort(function (country1, country2) {
        const country1name = country1[1].name
        const country2name = country2[1].name
        const country1code = country1[0]
        const country2code = country2[0]
        if (country1code == 'US' && country2code != 'US') return -1
        if (country2code == 'US' && country1code != 'US') return 1

        if (country1name < country2name) {
          return -1
        } else if (country1name === country2name) {
          return 0
        } else {
          return 1
        }
      });

      // Alert.show("test", 'success', false);

      const codes = {}
      Object.entries(iso3166.codes).forEach((item) => {
        codes[item[1]] = item[0]
      })

      const new_countries = countries.map((item) => {
            return {
              label: item[1].name,
              value: codes[item[0]],
              code: item[0]
            }
      })

      new_countries.unshift({
        value: 'NONE',
        code: 'NONE',
        label: '(would prefer not to disclose)'
      })
      this.countries = countries
      this.country_options = new_countries

      // get current user
      api.me().then((me) => {
        // initialize the data
        console.log(me);
        this.user = me
        this.level = me.level
        this.username = me.username
        this.originalUsername = me.username
        // country
        this.selected_country = this.country_options.find((item) => {
          return item.value == me.country_code
        })

        this.selected_region = this.regions.find((item) => {
          return item.value == me.country_subdivision_code
        })

        // Get the current account team list
        if (me.team_id !== null) {
          this.fetchTeamsList(me.team_id);
        }
      })

      api.list_organizations().then((orgs)=>
      {
          let schools = []
          if(orgs && orgs instanceof Array)
          {
              schools = orgs.filter((item) => {
              return item.type === 'High School'
            })
          }

          for (var i = 0; i < schools.length; i++) {
              this.highSchools.push({label: schools[i].name, id: schools[i].organization_id})
            }

          this.selected_highSchool = this.highSchools.find((item) => {
            return item.id == this.user.organization_id
          })
      })
    },
    computed: {
      regions: function () {
        if (!this.selected_country || this.selected_country.value === 'NONE') return []

        const regions = Object.entries(iso3166.data[this.selected_country.code].sub)

        const codes = []

        regions.sort(function (region1, region2) {
          const name1 = region1[1].name
          const name2 = region2[1].name
          if (name1 < name2) {
            return -1
          } else if (name1 === name2) {
            return 0
          } else {
            return 1
          }
        })

        const new_regions = regions.map((item) => {
          return {
            label: item[1].name,
            value: item[0],
            type: item[1].type
          }
        })

        new_regions.unshift({
          value: 'NONE',
          code: 'NONE',
          label: '(would prefer not to disclose)'
        })

        return new_regions
      },
      country_code: function () {
        return this.selected_country ? this.selected_country.value : null
      },
      country_region_code: function () {
        return (this.selected_country && this.selected_region) ? this.selected_region.value : null
      }
    },
    methods: {
      // reset selected region when change the country
      change_country: function (value) {
        if (this.selected_country !== value) {
          this.selected_country = value
          this.selected_region = null
        }
      },
      get_country: function () {
        const selected_country = this.country_options.find((item) => {
          return item.value == this.country_code.value
        })
        return selected_country
      },
      submit: function (e) {
        let request = {
          'level': this.level,
          'organization_id': this === 'NONE' ? null : this.organization
        }

        if(this.level === 'High School'){
          request['organization_id'] = this.selected_highSchool.id
        }

        if (this.username !== this.originalUsername) {
          if (!this.username_error.valid) {
            Alert.show('Invalid username.', 'error')
            return
          }
          request['username'] = this.username
        }

        if (this.country_code !== '') {
          const codes = {}

          // Build conversion table of 2-char country code to 3-char
          for (let code3 of Object.keys(iso3166.codes)) {
            codes[iso3166.codes[code3]] = code3
          }

          if (this.country_code !== '' && this.country_code !== 'NONE') {
            request['country_code'] = this.country_code
            if (this.country_region_code !== '' && this.country_region_code !== 'NONE') {
              request['country_subdivision_code'] = this.country_region_code
            }
            else if (this.country_region_code === 'NONE') {
              request['country_subdivision_code'] = null
            }
          }
          else if (this.country_code === 'NONE') {
            request['country_code'] = null
            request['country_subdivision_code'] = null
          }
        }

        if (this.level !== 'High School' && this.email) {
          request['email'] = this.email
        }
        api.update_me(this.user.user_id, request).then((response) => {
          let message = 'You have updated your profile successfully.';
          if (response.message)
            message += ' ' + response.message;
          Alert.show(message, 'success', true)
          this.gaData('account', 'edit-profile-success', 'edit-profile-flow')
        }, (error) => {
          const errorMessage = error.responseJSON
            ? error.responseJSON.message
            : "Sorry, we couldn't update your profile. Please try again later."
          Alert.show(errorMessage, 'error')
          this.gaData('account', 'edit-profile-error', 'edit-profile-flow')
        })
      },
      gaData: function (category, action, label) {
        utils.gaEvent(category, action, label)
      },
      resend_verification_email: function() {
        api.resend_verification_email(this.user.user_id).then((response) => {
          let message = "Verification code has been resent."
          if (response && response.message) {
            message = response.message;
          }
          else if (response && response.responseJSON && response.responseJSON.messasge) {
            message = response.responseJSON.message;
          }
          Alert.show(message, "success", true);
        }, (response) => {
          let message = "Sorry, we couldn't resend the verification email. Please try again later.";
          if (response && response.message) {
            message = response.message;
          }
          else if (response && response.responseJSON && response.responseJSON.message) {
            message = response.responseJSON.message;
          }

          Alert.show(message, "error");
        });
      },
      // Get the current account team list
      fetchTeamsList: function(team_id) {
        api.get_team(team_id).then((team)=>{
          this.team = team;
        })
      },
      // Create Team
      createTeam: function() {
        const team_name = this.new_team_name;
        if (team_name) {
          this.teamCreateError = null
          api.create_team(team_name).then((res)=>{
            if (res.status === 'success') {
              this.fetchTeamsList(res.team_id)
            }
            else {
              this.teamCreateError = res.message
            }
          })
        }
      },
      // Join a Team
      joinTeam: function () {
        const [ team_id, verification_code ] = this.join_team_name.split('@');
        this.teamJoinError = null
        if (team_id && verification_code) {
          api.join_team(team_id, verification_code).then((res)=>{
            if (res.status === 'success') {
              window.location.reload()
            }
            else {
              this.teamJoinError = res.message
            }
          }, (res) => {
            console.warn(res)
            this.teamJoinError = "Could not join team."
          })
        }
        else {
          this.teamJoinError = "Invalid verification code."
        }
      },
    },
    watch: {
      username(newUsername) {
        this.username_error.reason = ''
        this.checkUsername()
      }
    }
  }
</script>

<style lang="scss" scoped>
</style>
