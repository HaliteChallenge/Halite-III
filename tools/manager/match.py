
import os
import copy
import json
import shutil
import skills
from skills import trueskill
from subprocess import Popen, PIPE, call

record_dir = "replays"


def update_skills(players, ranks):
    """ Update player skills based on ranks from a match """
    teams = [skills.Team({player.name: skills.GaussianRating(player.mu, player.sigma)}) for player in players]
    match = skills.Match(teams, ranks)
    calc = trueskill.FactorGraphTrueSkillCalculator()
    game_info = trueskill.TrueSkillGameInfo()
    updated = calc.new_ratings(match, game_info)
    for team in updated:
        player_name, skill_data = next(iter(team.items()))    #in Halite, teams will always be a team of one player
        player = next(player for player in players if player.name == str(player_name))   #this suggests that players should be a dictionary instead of a list
        player.mu = skill_data.mean
        player.sigma = skill_data.stdev
        player.update_skill()
        print("skill = %4f  mu = %3f  sigma = %3f  name = %s" % (player.skill, player.mu, player.sigma, str(player_name)))


class Match:
    def __init__(self, players, width, height, seed, time_limit, keep_replays, keep_logs):
        print("Seed = " + str(seed))
        self.map_seed = seed
        self.map_height = height
        self.map_width = width
        self.players = players
        self.paths = [player.path for player in players]
        self.finished = False
        self.results = [0 for _ in players]
        self.return_code = None
        self.results_string = ""
        self.replay_file = ""
        self.total_time_limit = time_limit
        self.timeouts = []
        self.num_players = len(players)
        self.keep_replay = keep_replays
        self.keep_logs = keep_logs
        self.logs = None
        self.map_generator = None

    def __repr__(self):
        title1 = "Match between " + ", ".join([p.name for p in self.players]) + "\n"
        title2 = "Binaries are " + ", ".join(self.paths) + "\n"
        dims = "dimensions = " + str(self.map_width) + ", " + str(self.map_height) + "\n"
        results = "\n".join([str(i) + " " + j for i, j in zip(self.results, [p.name for p in self.players])]) + "\n"
        replay = self.replay_file + "\n" #\n"
        return title1 + title2 + dims + results + replay

    def get_command(self, halite_binary):
        if self.keep_replay is True:
            replays = "--replay-directory replays/"
        else:
            replays = "--no-replay"
        width = "--width 32"
        height = "--height 32"
        seed = "-s " + str(self.map_seed)
        results = "--results-as-json"
        result = [halite_binary, replays, width, height, seed, results]
        if self.keep_logs is False:
            result.append("--no-logs")

        return result + self.paths

    def run_match(self, halite_binary):
        command = self.get_command(halite_binary)
        print("Command = " + str(command))
        p = Popen(command, stdin=None, stdout=PIPE, stderr=None)
        results, _ = p.communicate(None, self.total_time_limit)

        self.results_string = results.decode('ascii')
        self.return_code = p.returncode
        self.parse_results_string()
        update_skills(self.players, copy.deepcopy(self.results))

    def parse_results_string(self):
        data = json.loads(self.results_string)
        self.logs = data['error_logs']
        self.map_height = data['map_height']
        self.map_width = data['map_width']
        self.map_seed = data['map_seed']
        self.map_generator = data['map_generator']
        self.replay_file = data['replay']
        stats = data['stats']
        for player_index_string in stats:
            player_index = int(player_index_string)
            rank_string = stats[player_index_string]['rank']
            self.results[player_index] = int(rank_string)

