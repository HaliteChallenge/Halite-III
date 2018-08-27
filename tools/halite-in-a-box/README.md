# Box<Halite>

This is ~~a heap allocated version of Halite~~ a standalone developer experience for Halite III, making it easy to iterate on your bots and try new strategies.

## Building

We embed Python with this application, to make the experience easier, but this makes building the application painful.

### Windows

This is a __REAL HACK__.

_Note: AppVeyor should be able to do this, once set up._

1. Install Python 3.7 on Windows.
2. `pip install zstd requests trueskill` (i.e. `hlt_client`'s dependencies).
3. Find the `site-packages` folder and zip its contents up as `deps.zip`. Put that into `./extra/`.
4. Find the `zstd.cp37....pyd` file in `site-packages` and copy that too as `./extra/zstd.pyd`.

### MacOS

_Note: Travis-CI builds this for us, thankfully._

From a Mac:

1. Make sure `virtualenv` and `python3` are available.
2. Go to `tools/halite-in-a-box/macos-portable-python`.
3. Run `build.sh`.
4. Copy `./dist/python-macos.app` to `../extra/python-macos`.

_What this does:_ uses `py2app` to build a distribution of Python with our dependencies pre-packaged.

### Linux

Have fun! (Linux has no prerequisites, since currently the application assumes all the requisite Python dependencies are available on the system.)

### The Application Itself

1. Copy `tools/hlt_client/hlt_client` to `tools/halite-in-a-box/extra/hlt_client`.
2. In `libhaliteviz` (in the repo root), run `npm install`.
3. From here, run `npm install`.
4. Run: `$(npm root)/.bin/electron-builder -m` (for Mac) or `./node_modules/.bin/electron-builder -w` (for Windows).
5. Wait a really long time.
6. Look at `./electron-dist`.
