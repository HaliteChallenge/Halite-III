#!/bin/bash

COLOR_INFO='\033[1;35m'
COLOR_ERROR='\033[0;31m'
RESET='\033[0m'

function info {
    echo -e "${COLOR_INFO}$1${RESET}"
}

function error {
    echo -e "${COLOR_ERROR}$1${RESET}"
}

# Get copies of MacOS Python, Windows Python, and hlt_client
info "Getting python-macos."
if [[ -d extra/python-macos ]]; then
    echo "python-macos already present."
elif [[ $(uname) == "Darwin" ]]; then
    info "Building python-macos"
    pushd macos-portable-python
    ./build.sh
    popd

    cp -r macos-portable-python/dist/python-macos.app extra/python-macos
elif [[ ! -f python-macos.zip ]]; then
    error "python-macos.zip not implemented"
    exit 1
else
    error "Could not find python-macos.zip and not on Darwin. Exiting."
    exit 1
fi

info "Getting python-windows."
if [[ -d extra/python-windows ]]; then
    echo "python-windows already present."
else
    curl -o python.zip https://www.python.org/ftp/python/3.7.0/python-3.7.0-embed-amd64.zip
    unzip python.zip -d extra/python-windows
    # Add module search directory to sys.path
    # (embedded Python ignores things like PYTHONPATH)
    # https://bugs.python.org/issue28245
    echo ".." >> extra/python-windows/python37._pth
fi

info "Copying hlt_client."
if [[ -d extra/hlt_client ]]; then
    rm -r extra/hlt_client
fi
rsync -r \
      --exclude __pycache__ --exclude '*~' --exclude '*.pyc' \
      ../hlt_client/hlt_client/ extra/hlt_client

$(npm root)/.bin/electron-builder -mwl
