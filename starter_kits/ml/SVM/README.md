
# SVM Starter Kit

## Starter kit components
* Two pre-trained example bots, comprised of a python file that references SVMBot.py, and a .svc model weight file:
** Passive bot: SVMBotPassive.py, passive.svc
** Aggressive bot: SVMBotAggressive.py, aggressive.svc
* Resources for training a new SVM bot
** Empty replays directory, into which you put game replay files for training
** train.py, which you run to train a new bot
** parse.py, which parses replay files during training
** model.py, which will define your model
* install.sh, which will install dependencies
* /hlt directory, which contains modifiable helper functions for your bot
* A Halite executable that enables local playtesting of your bot
* The scripts run_game.bat (Windows) and run_game.sh (MacOS, Linux)


## Training a new bot

### Download Training Data

Create a training dataset by downloading historical game replay files via the Halite client.

* Download the client directly at http://halite.io/assets/downloads/Halite3Tools.zip.

* Run the client. 

`python -m hlt_client` 

* Download replays for your target training user(s). The download destination_folder should be your replays directory.

`python -m hlt_client replay user -i [user_id] -l [maximum_number_of_files] -d [destination_folder]`


### Train your bot

* Update train.py with the name of your training user(s). The name is case-sensitive. For example, to train on a user named "aggressive", for whom you have downloaded replays, you would insert the following into the file:

```
m = model.HaliteModel()
m.train_on_files('training', 'aggressive')
m.save(file_name='aggressive.svc')
```

* Run train.py.

## Testing your bot locally
* Run run_game.bat (Windows) and run_game.sh (MacOS, Linux) to run a game of Halite III. By default, these scripts run a game of your MyBot.py bot vs. itself.  You can modify the board size, map seed, and the opponents of test games using the CLI.

## CLI
The Halite executable comes with a command line interface (CLI). Run `$ ./halite --help` to see a full listing of available flags.


## Submitting your bot
* Duplicate the example SVMBotPassive.py or SVMBotPassive.py, and rename the file "MyBot.py". If you have trained a new bot, update the file with the correct references to your new bot file and model weight.
* Zip MyBot.py, install.sh, model.py, parse.py, and the /hlt directory together.
* Submit your zipped file here: https://halite.io/play-programming-challenge

