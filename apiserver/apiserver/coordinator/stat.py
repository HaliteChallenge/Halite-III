"""
Provide named variables for coordinator api to update bots stats after every match.
"""


class GameStat(object):
    def __init__(self, num_players=0):
        self.turns_total = 0
        self.players = {}
        for player_tag in range(num_players):
            self.players[player_tag] = PlayerStat()


class PlayerStat(object):
    def __init__(self):
        pass
