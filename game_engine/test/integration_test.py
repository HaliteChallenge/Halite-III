#!/usr/bin/env python3

import argparse
import itertools
import json
import os
import subprocess
import tempfile
import unittest


ENGINE_ENV_VAR = 'HALITE_ENGINE'


class RankingTest(unittest.TestCase):
    def setUp(self):
        """Get the engine location."""
        if ENGINE_ENV_VAR not in os.environ:
            raise RuntimeError(f'Set {ENGINE_ENV_VAR} in environment to run this test.')
        self._engine_location = os.environ[ENGINE_ENV_VAR]

    def _run_game(self, bots):
        """Run a game with the specified bots."""
        bot_commands = []
        helper_bot = os.path.join(os.path.dirname(__file__), 'integration_test_bot.py')
        for name, sequence in bots:
            bot_commands.append(f'python3 "{helper_bot}" "{name}" "{sequence}"')

        with tempfile.TemporaryDirectory() as tempdir:
            called_process = subprocess.run([
                self._engine_location,
                '--results-as-json',
                '--no-compression',
                '--no-logs',
                '--replay-directory', tempdir,
            ] + bot_commands, stdout=subprocess.PIPE)
            called_process.check_returncode()
            result = json.loads(called_process.stdout)
            stats = result['stats']

            clean_stats = {}
            for i in range(len(bots)):
                bot_key = str(i)
                clean_stats[bots[i][0]] = stats[bot_key]

            with open(result['replay']) as replay_file:
                return json.load(replay_file), clean_stats

    def test_normal_ranking(self):
        """Test end-of-game ranking with no crashes."""
        for configuration in itertools.permutations([
            # No relation to k-pop I swear
            ('BotA', 'g'),
            ('BotB', 'g,g'),
            ('BotC', 'g,g,g'),
            ('BotD', 'g,g,g,g'),
        ]):
            _, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotA', 'BotB', 'BotC', 'BotD'])

    def test_ranking_one_crashed(self):
        """Test end-of-game ranking when one bot crashes."""
        for configuration in itertools.permutations([
            ('BotA', 'g, , , ,!'),
            ('BotB', 'g,g, , , '),
            ('BotC', 'g,g,g, , '),
            ('BotD', 'g,g,g,g, '),
        ]):
            _, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotB', 'BotC', 'BotD', 'BotA'])
            self.assertEqual(raw_rankings['BotA']['score'], 0)

    def test_crashed_same_turn(self):
        """Test premature ranking when multiple bots crash together."""
        for configuration in itertools.permutations([
            ('BotA', 'g, , , ,!'),
            ('BotB', 'g,g, , ,!'),
            ('BotC', 'g,g,g, ,!'),
            ('BotD', 'g,g,g,g, '),
        ]):
            # Bot that doesn't crash has least halite, but should still win
            results, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotD', 'BotA', 'BotB', 'BotC'])
            self.assertEqual(raw_rankings['BotA']['score'], 0)
            self.assertEqual(raw_rankings['BotB']['score'], 0)
            self.assertEqual(raw_rankings['BotC']['score'], 0)
            self.assertEqual(raw_rankings['BotD']['score'], 1000)

    def test_all_crashed(self):
        """Test that ranking goes by amount of halite if all crash."""
        for configuration in itertools.permutations([
            ('BotA', 'g, , , ,!'),
            ('BotB', 'g,g, , ,!'),
            ('BotC', 'g,g,g, ,!'),
            ('BotD', 'g,g,g,g,!'),
        ]):
            results, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotA', 'BotB', 'BotC', 'BotD'])

    def test_ranking_last_turn_alive(self):
        """Test that ranking goes by the last turn alive."""
        for configuration in itertools.permutations([
            ('BotA', '!'),
            ('BotB', ' ,!'),
            ('BotC', ' , ,!'),
            ('BotD', ''),
        ]):
            results, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotD', 'BotC', 'BotB', 'BotA'])

    def test_all_crashed_different_turns(self):
        """Test that ranking goes by amount of halite per-turn if all crash."""
        for configuration in itertools.permutations([
            ('BotA', 'g, , , ,!, '),
            ('BotB', 'g,g, , ,!, '),
            ('BotC', 'g,g,g, , ,!'),
            ('BotD', 'g,g,g,g, ,!'),
        ]):
            results, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotC', 'BotD', 'BotA', 'BotB'])

    def test_no_crash_least_halite(self):
        """Test that premature ranking ranks non-crashed players first."""
        for configuration in itertools.permutations([
            ('BotA', 'g, , ,!'),
            ('BotB', 'g,g, ,!'),
            ('BotC', ' , , ,!'),
            ('BotD', 'g,g,g,'),
        ]):
            results, raw_rankings = self._run_game(configuration)
            rankings = list(sorted(raw_rankings, key=lambda bot: raw_rankings[bot]['rank']))
            self.assertEqual(rankings, ['BotD', 'BotC', 'BotA', 'BotB'])

    def test_game_ended_no_bailout(self):
        """Test that all players are terminated in end-of-turn check.

        The engine originally bailed out early, meaning some players
        would not be marked as unable to play until the next turn.

        This is a regression test.
        """
        results, raw_rankings = self._run_game([
            ('BotA', 'g,m #0 e,c #0, '),
            ('BotB', 'g,g, , '),
            ('BotC', 'g,g,g, '),
            ('BotD', 'g,m #0 e,c #0, '),
        ])
        self.assertEqual(3, results['game_statistics']['player_statistics'][0]['last_turn_alive'])
        self.assertEqual(3, results['game_statistics']['player_statistics'][3]['last_turn_alive'])


if __name__ == '__main__':
    unittest.main()
