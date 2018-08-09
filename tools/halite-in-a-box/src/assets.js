import { remote as electronRemote } from 'electron';
import fs from 'fs';
import path from 'path';
import JSZip from 'jszip';

import * as util from './util';

export const BENCHMARK_BOT_DIRECTORY = 'benchmark-bots/';

/**
 * Download the manifest for the benchmark bots from halite.io. Return
 * the cached one if possible.
 *
 * If available from in-memory cache, use that.
 * If available from network, use that.
 * Else try from disk cache.
 */
export async function getBotManifest() {

}

/**
 * Make sure the environment, benchmark bots, and other assets are
 * prepared and up-to-date.
 */
export default async function assets() {
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

    console.info(`Data directory: ${dataDir}`);

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

    // Download halite game executable if necessary
    const environmentPath = path.join(dataDir, environmentBinary);
    if (!fs.existsSync(environmentPath)) {
        console.info(`Downloading environment to ${environmentPath}`);

        const request = await util.download(`assets/downloads/Halite3_${platform}.zip`);
        const rawZip = await request.arrayBuffer();
        const zip = await JSZip.loadAsync(rawZip);
        const binary = await zip.file(environmentBinary).async('uint8array');
        await util.writeFile(environmentPath, binary);
        fs.chmodSync(environmentPath, 0o775);
    }

    // TODO: try executing environment to get version

    // Download benchmark bots
    const botsPath = path.join(dataDir, BENCHMARK_BOT_DIRECTORY);
    if (!fs.existsSync(botsPath)) {
        fs.mkdirSync(botsPath);
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
    const benchmarkBots = bots.map(filename => ({
        path: path.join(botsPath, filename),
        name: filename === 'MyBot.py' ? 'Python Starter Bot' :
            path.basename(filename, path.extname(filename)),
    }));

    return {
        environmentPath,
        benchmarkBots,
    };
}
