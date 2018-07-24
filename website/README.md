# Halite Website

## Setup

Assuming you have RubyGems and Yarn installed:

    $ cd libhaliteviz/
    $ yarn install
    $ yarn link
    $ cd website/
    $ sudo gem install bundler
    $ bundle install --path=vendor/bundle
    $ yarn install
    $ yarn link libhaliteviz
    $ yarn build
    $ bundle exec jekyll serve

Then visit <http://localhost:4000>.

You can also try to use NPM, but Yarn tends to handle things better.

If you modify the JavaScript, you might want to run the Webpack watcher, which will rebuild the JavaScript bundle on the fly:

    $ yarn watch

To deploy, You will also want a folder with compiled Halite environment binaries. The binaries should be named after the platform they are for, e.g. `MacOS`, `Linux-x64`, and `Windows.exe`. Run `make_starter_kits.py` to generate all the starter kits and source downloads.

Currently, there is no automation for building the offline visualizer.

    $ # Make sure to kill Jekyll and Webpack beforehand, if they're running
    $ python3 make_starter_kits.py path/to/folder/with/compiled/halite/environments
    $ yarn build
    $ bundle exec jekyll build
