#!/usr/bin/env python3

import SVMBot


class SVMBotPassive(SVMBot.SVMBot):
    def __init__(self):
        super().__init__("passive.svc")


if __name__ == '__main__':
    bot = SVMBotPassive()
    bot.run()
