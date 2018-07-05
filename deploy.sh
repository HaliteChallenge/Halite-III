#!/bin/bash

if [[ ! -f ../config.py ]]; then
   echo "Can't find config.py in containing directory."
   exit 1
fi

# Make sure we aren't deploying uncommited changes
# https://stackoverflow.com/questions/5143795
if ! git diff-index --quiet HEAD --; then
    echo "This Git repository has uncommited changes."
    echo "Please use a clean clone of the repository."
    exit 1
fi

cp ../config.py apiserver/apiserver/config.py

# Build the website
pushd website
yarn install
bundle install --path=vendor/bundle

pushd ../libhaliteviz
yarn install
popd

npm run build
bundle exec jekyll build

popd

# Wrap directories in archive in top-level Halite/ folder via a
# replacement
rm ../Halite.tgz
tar czf ../Halite.tgz -s ',^,Halite/,' \
    apiserver/ environment/ website/_site/ \

git checkout -- apiserver/apiserver/config.py
git checkout -- website/Gemfile.lock

echo "Uploading new blob to Google Cloud Storage."
pushd admin/deploy/

if [[ ! -d venv ]]; then
    virtualenv -p python3 venv
    source venv/bin/activate
    pip install -r requirements.txt
else
    source venv/bin/activate
fi

export GOOGLE_APPLICATION_CREDENTIALS=../../../serviceaccount.json
python deploy.py
