#!/usr/bin/env bash

# We are running as root, switch to a less privileged account
sudo -iu worker bash <<"EOF"
cd

# Fetch the coordinator
# TODO: This is in GCloud right now, but we should use Git when we go public
while [ ! -f ./Halite.tgz ]; do
    sleep 5
    gsutil cp gs://ts2018-halite-3-deployed-artifacts/Halite.tgz .
done

tar -xzf Halite.tgz
cd Halite/
echo $(datetime) > ~/_SUCCESS
EOF
sudo cp -rf /home/worker/Halite/website/_site/* /var/www/html/
