import { remote as electronRemote } from 'electron';
import crypto from 'crypto';
import fs from 'fs';
import path from 'path';
import JSZip from 'jszip';

import * as bot from './bot';
import * as logger from './logger';
import * as util from './util';

export const BENCHMARK_BOT_DIRECTORY = 'benchmark-bots/';

let __userId = null;
let __apiKey = null;

export function setCredentials(userId, key) {
    __userId = userId;
    __apiKey = key;
}

export function getCredentials() {
    return {
        userId: __userId,
        apiKey: __apiKey,
    };
}

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
            logger.error(e);
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

export function toolsPath() {
    return path.join(embeddedResourcesPath(), 'hlt_client');
}

/**
 * Modify a command to set the CWD before running.
 */
export function setCwd(cwd, cmd) {
    if (process.platform === 'win32') {
        return `cmd.exe /C "cd /d "${cwd}"; ${cmd}"`;
    }
    return `sh -c 'cd "${cwd}"; ${cmd}'`;
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
        environmentBinary = 'halite_engine';
    }
    else if (process.platform === 'win32') {
        platform = 'Windows-AMD64';
        environmentBinary = 'halite_engine.exe';
    }
    else if (process.platform === 'linux') {
        platform = 'Linux-AMD64';
        environmentBinary = 'halite';
    }

    // Path to halite executable
    const environmentPath = path.join(embeddedResourcesPath(), environmentBinary);
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

    // Download benchmark bots
    let downloadBots = false;
    console.log(botsPath);
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
                const destPath = path.join(botsPath, filePath);
                promises.push(new Promise((resolve, reject) => {
                    const input = fs.createReadStream(destPath);
                    input.on('error', (err) => {
                        console.log(`Hash of bot file ${destPath} does not match.`);
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
            logger.warn('Could not download bot manifest?', e);
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
        await util.extractZip(zip, botsPath);
        console.info('Downloaded benchmark bots.');
    }
    else {
        console.info('Benchmark bots already present.');
    }

    const bots = await new Promise((resolve) => {
        fs.readdir(path.join(botsPath, 'benchmark'), (err, files) => {
            // TODO: magic string
            resolve(files.filter(f => f.endsWith('.py') && f !== 'hlt.py'));
        });
    });
    const benchmarkBots = bots.map(filename => new bot.InterpretedBot(
        filename === 'MyBot.py' ? 'Python Starter Bot' :
            path.basename(filename, path.extname(filename)),
        bot.InterpretedBot.languages.Python,
        path.join(botsPath, 'benchmark', filename)
    ));

    const result = Object.assign({}, paths, {
        benchmarkBots,
    });
    _resolveAssets(result);
    return result;
}
