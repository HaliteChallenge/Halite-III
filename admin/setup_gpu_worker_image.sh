#!/usr/bin/env bash

set -e

echo "This script should be run after the base worker image setup script."

echo "Installing CUDA and cudnn."

# Newer repo doesn't have cuda-9-0 which our version of TensorFlow depends upon.
curl -O http://developer.download.nvidia.com/compute/cuda/repos/ubuntu1604/x86_64/cuda-repo-ubuntu1604_8.0.61-1_amd64.deb

sudo dpkg -i ./cuda-repo-ubuntu1604_8.0.61-1_amd64.deb
sudo apt-get -qq update
# Without force-overwrite nvidia-396 conflicts with libglx0-mesa
sudo apt-get -q -o Dpkg::Options::="--force-overwrite" install nvidia-396 cuda-9-0 -y

# Install libcudnn separately (making sure it matches the cuda version)
wget -O libcudnn.deb https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1604/x86_64/libcudnn7_7.2.1.38-1+cuda9.0_amd64.deb
wget -O libcudnn-dev.deb https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1604/x86_64/libcudnn7-dev_7.2.1.38-1+cuda9.0_amd64.deb

sudo dpkg -i ./libcudnn.deb
sudo dpkg -i ./libcudnn-dev.deb

echo 'export PATH=/usr/local/cuda-9.0/bin${PATH:+:${PATH}}' >> ~/.profile
echo 'export LD_LIBRARY_PATH=/usr/local/cuda-9.0/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}' >> ~/.profile
source ~/.profile

sudo apt-get install -y libcupti-dev libcupti9.1

sudo pip3 install tensorflow-gpu

sudo pip3 install http://download.pytorch.org/whl/cu80/torch-0.2.0.post3-cp36-cp36m-manylinux1_x86_64.whl

# Print out installed versions of packages
PACKAGES="libcupti-dev libcudnn"
PYTHON_PACKAGES="tensorflow-gpu torch"

echo "Packages"
for package in ${PACKAGES}; do
    dpkg-query -W ${package}
done

echo "Python 3.6 Packages"
for package in ${PYTHON_PACKAGES}; do
    echo ${package} $(python3.6 -m pip show ${package} | grep Version | awk '{print $2}')
done
