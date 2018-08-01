#!/usr/bin/env bash

# First, create the other 12 bot accounts needed
export NUM_BOTS=16
for i in $(seq 4 $((NUM_BOTS-1))); do
    USERNAME="bot_${i}"
    sudo groupadd bots_$i
    sudo usermod -aG bots_$i worker
    sudo useradd -m -g bots_${i} ${USERNAME}
    sudo iptables -A OUTPUT -m owner --uid-owner ${USERNAME} -j DROP
    sudo sh -c "echo \"worker ALL=(${USERNAME}) NOPASSWD: ALL\" > /etc/sudoers.d/worker_${USERNAME}"
    sudo chmod 0400 /etc/sudoers.d/worker_${USERNAME}
    echo 'export PATH="$PATH:/usr/local/go/bin"' | sudo -iu ${USERNAME} tee -a /home/${USERNAME}/.profile
done

# Now set up the reduced-resource cgroups
sudo rm -f /etc/cgconfig.conf
sudo touch /etc/cgconfig.conf
for i in $(seq 0 $((NUM_BOTS-1))); do
    CGROUP="bot_${i}"
    echo "Creating cgroup ${CGROUP}"
    cat <<EOF | sudo tee -a /etc/cgconfig.conf
group ${CGROUP} {
        # Grant control over the cgroup to the worker user
        perm {
                admin {
                        uid = root;
                        gid = root;
                }
                task {
                        uid = worker;
                        gid = worker;
                }
        }
        cpu {
                cpu.shares="256";
        }
        memory {
                memory.limit_in_bytes="256M";
        }
        devices {
                devices.allow="a *:* rwm";
        }
        cpuset {
               cpuset.cpu_exclusive=0;
               cpuset.cpus=$(($i % 4));
               cpuset.mems=0;
        }
}
EOF
done

sudo /usr/sbin/cgconfigparser -l /etc/cgconfig.conf

# We are running as root - run as our worker user instead

sudo -iu worker bash <<"EOF"
cd
# Fetch the worker script
while [ ! -f ./Halite.tgz ]; do
    sleep 5
    curl -v http://10.142.0.5:5001/v1/coordinator/download/worker --output Halite.tgz
done

tar xvzf Halite.tgz

cd Halite/apiserver/worker
bash setup.sh --task-type=ondemand

screen -S worker-2 -d -m /bin/bash -c "python3 worker.py --task-type=ondemand --user-offset=4"
screen -S worker-3 -d -m /bin/bash -c "python3 worker.py --task-type=ondemand --user-offset=8"
screen -S worker-4 -d -m /bin/bash -c "python3 worker.py --task-type=ondemand --user-offset=12"

EOF
