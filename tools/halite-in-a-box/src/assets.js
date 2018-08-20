import { remote as electronRemote } from 'electron';
import crypto from 'crypto';
import fs from 'fs';
import path from 'path';
import JSZip from 'jszip';

import * as bot from './bot';
import * as util from './util';

export const BENCHMARK_BOT_DIRECTORY = 'benchmark-bots/';

export function appData() {
    // Create directory in application data directory
    const dataDir = path.join(electronRemote.app.getPath('appData'),
                              'halite-in-a-box');
    try {
        fs.mkdirSync(dataDir);
    }
    catch (e) {
        if (e.code && e.code === 'EEXIST') {
            // Already exists, do nothing
        }
        else {
            console.error(e);
            throw e;
        }
    }
    return dataDir;
}

export function embeddedResourcesPath() {
    // Should use remote.app.isPackaged in newer Electron
    // (see https://github.com/electron/electron/issues/7714)
    if (process.mainModule.filename.indexOf('app.asar') !== -1) {
        // .app/production executable, use resourcesPath
        return path.join(process.resourcesPath, 'extra');
    }
    else {
        // XXX this is fragile but only applies during development
        return path.join(process.cwd(), 'extra');
    }
}

export function pythonPath() {
    if (process.platform === 'darwin') {
        return path.join(pythonBasePath(), 'MacOS/python');
    }
    else if (process.platform === 'win32') {
        return path.join(pythonBasePath(), 'python.exe');
    }
    return 'python3';
}

export function pythonBasePath() {
    if (process.platform === 'darwin') {
        return path.join(embeddedResourcesPath(), 'python-macos/Contents');
    }
    else if (process.platform === 'win32') {
        return path.join(embeddedResourcesPath(), 'python-windows');
    }
    return '';
}

export function pythonHomePath() {
    // Otherwise packaged Python doesn't know where it is
    if (process.platform === 'darwin') {
        return path.join(pythonBasePath(), 'Resources');
    }
    return '';
}

export function pythonPackagePath(otherPackages) {
    const packages = [].concat(otherPackages);
    if (process.platform === 'darwin') {
        packages.push(path.join(pythonBasePath(), 'Resources/lib/python37.zip'));
        packages.push(path.join(pythonBasePath(), 'Resources/lib/python3.7/lib-dynload'));
    }
    // On Windows, portable Python distribution takes care of this
    return packages.join(':');
}

export function toolsPath() {
    return path.join(embeddedResourcesPath(), 'hlt_client');
}

/**
 * Get paths to bundled assets without checking if they're ready.
 */
export function assetPaths() {
    const dataDir = appData();

    console.info(`Data directory: ${dataDir}`);

    const replayDir = path.join(dataDir, 'replays');

    // Determine platform name for downloads
    let platform;
    let environmentBinary;
    if (process.platform === 'darwin') {
        platform = 'MacOS';
        environmentBinary = 'halite';
    }
    else if (process.platform === 'win32') {
        platform = 'Windows-AMD64';
        environmentBinary = 'halite.exe';
    }
    else if (process.platform === 'linux') {
        platform = 'Linux-AMD64';
        environmentBinary = 'halite';
    }

    // Path to halite executable
    const environmentPath = path.join(dataDir, environmentBinary);
    // Path to benchmark bots
    const botsPath = path.join(dataDir, BENCHMARK_BOT_DIRECTORY);

    return {
        dataDir,
        replayDir,
        environmentPath,
        platform,
        environmentBinary,
        botsPath,
    };
}

// Keep track of loading the assets
let _resolveAssets;
const assetsReady = new Promise((resolve) => {
    _resolveAssets = resolve;
});
let loading = false;

/**
 * Make sure the environment, benchmark bots, and other assets are
 * prepared and up-to-date.
 */
export default async function assets() {
    if (loading) {
        // Bail if already loading
        const result = await assetsReady;
        return result;
    }
    loading = true;

    const paths = assetPaths();
    const {
        dataDir,
        replayDir,
        environmentPath,
        platform,
        environmentBinary,
        botsPath,
    } = paths;

    // Download the halite executable if needed
    // TODO: try executing environment to get version
    if (!fs.existsSync(environmentPath)) {
        console.info(`Downloading environment to ${environmentPath}`);

        const request = await util.download(`assets/downloads/Halite3_${platform}.zip`);
        const rawZip = await request.arrayBuffer();
        const zip = await JSZip.loadAsync(rawZip);
        const binary = await zip.file(environmentBinary).async('uint8array');
        await util.writeFile(environmentPath, binary);
        fs.chmodSync(environmentPath, 0o775);
    }

    // Download benchmark bots
    let downloadBots = false;
    if (!fs.existsSync(botsPath)) {
        fs.mkdirSync(botsPath);
        downloadBots = true;
    }
    else {
        try {
            // Grab the manifest and make sure everything is there
            const request = await util.download(`assets/downloads/Halite3Benchmark.json`);
            const manifest = await request.json();
            // Promise is resolved as true if the file needs to be
            // redownloaded
            const promises = [];
            for (const [ filePath, sha256hash ] of manifest.manifest) {
                const destPath = path.join(botsPath, path.basename(filePath));
                promises.push(new Promise((resolve, reject) => {
                    const input = fs.createReadStream(destPath);
                    input.on('error', (err) => {
                        resolve(true);
                    });

                    const hmac = crypto.createHash('sha256');
                    input.pipe(hmac);
                    hmac.on('data', (d) => {
                        resolve(d.toString('hex') !== sha256hash);
                    });
                }));
            }

            const results = await Promise.all(promises);
            // Redownload all bots if any promise was resolved true
            downloadBots = results.some(x => x);
        }
        catch (e) {
            console.warn('Could not download bot manifest?');
            console.warn(e);
            // Couldn't download the manifest or something?
            downloadBots = true;
        }
    }

    if (downloadBots) {
        console.info('Downloading benchmark bots.');
        const request = await util.download(`assets/downloads/Halite3Benchmark.zip`);
        const rawZip = await request.arrayBuffer();
        const zip = await JSZip.loadAsync(rawZip);
        for (const [ zipFilePath, file ] of Object.entries(zip.files)) {
            const fileName = path.basename(zipFilePath);
            const destPath = path.join(botsPath, fileName);
            const binary = await file.async('uint8array');
            await util.writeFile(destPath, binary);
        }
    }

    const bots = await new Promise((resolve) => {
        fs.readdir(botsPath, (err, files) => {
            // TODO: magic string
            resolve(files.filter(f => f.endsWith('.py') && f !== 'hlt.py'));
        });
    });
    const benchmarkBots = bots.map(filename => new bot.InterpretedBot(
        filename === 'MyBot.py' ? 'Python Starter Bot' :
            path.basename(filename, path.extname(filename)),
        bot.InterpretedBot.languages.Python,
        path.join(botsPath, filename)
    ));

    const result = Object.assign({}, paths, {
        benchmarkBots,
    });
    _resolveAssets(result);
    return result;
}
