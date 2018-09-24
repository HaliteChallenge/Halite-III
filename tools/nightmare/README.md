Nightmare
=========

A tool for debugging bots. Nightmare turns a replay back into the
messages that the game engine sends to a bot, allowing you to force
your bot to re-experience the same game over and over again. Note that
your bot should be deterministic, i.e. it shouldn't rely on random
numbers or iteration order over unordered collections.

Nightmare is also planned to 'expand' a replay into an uncompressed
JSON format that stores all information on every frame, making it easy
to work with (but also use much more storage space).
