#!/usr/bin/env bash

# This script is essentially the second half of the coordinator's startup
# script.

virtualenv --python=python3.6 venv

source venv/bin/activate
pip install -r requirements.txt

wget -O cloud_sql_proxy https://dl.google.com/cloudsql/cloud_sql_proxy.linux.amd64
chmod +x ./cloud_sql_proxy

# Get the spec for the DB instances to pass to Cloud SQL Proxy
DB_INSTANCE="$(python -m apiserver.scripts.print_db_proxy_instance)"

while read -r instance_config; do
    instance_config=($instance_config)

    echo "Starting sqlproxy"
    echo "./cloud_sql_proxy -instances=${instance_config[1]}=tcp:${instance_config[0]}"

    screen -S "sqlproxy-${instance_config[0]}" -d -m /bin/bash -c \
           "./cloud_sql_proxy -instances=${instance_config[1]}=tcp:${instance_config[0]}"

done <<< "$DB_INSTANCE"


# Try and start the server in a loop because NewRelic sometimes fails
# to start up initially
while ! screen -list | grep -q api; do
screen -S api -d -m /bin/bash -c \
    "NEW_RELIC_CONFIG_FILE=newrelic.ini PYTHONPATH=$(pwd) newrelic-admin run-program gunicorn apiserver.server:app -w 12 -b 0.0.0.0:5000"
sleep 5
done

while ! screen -list | grep -q coordinator_internal; do
screen -S coordinator_internal -d -m /bin/bash -c \
    "NEW_RELIC_CONFIG_FILE=newrelic.ini PYTHONPATH=$(pwd) newrelic-admin run-program gunicorn apiserver.coordinator_server:app -w 8 -b 0.0.0.0:5001"
sleep 5
done

# Run game deletion job at 8 AM UTC = midnight PST (DOES NOT account
# for DST)
# Disabled after finals ended
# { crontab -l -u worker; echo "0 8 * * * $(pwd)/delete_old_games.sh"; } | crontab -u worker -
