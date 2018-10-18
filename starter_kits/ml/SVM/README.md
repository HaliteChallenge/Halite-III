A few tips for using the kit:

* Be sure to include a .svc model weights file in your zipped bot. There are now two examples, aggressive.svc and passive.svc, available in the kit download; if you downloaded the kit on 10/16 you can download these from the repo: https://github.com/HaliteChallenge/Halite-III/tree/master/starter_kits/ml/SVM
* To submit either the pre-trained Aggressive or Passive bot, rename SVMBotPassive.py or SVMBotAggressive.py to MyBot.py
* Include an install.sh file in your zip with the following to install dependencies.:
```bash
#!/bin/sh

python3.6 -m pip install --target . tqdm
python3.6 -m pip install --target . zstd
```
