import { spawn } from 'child_process';
import { remote as electronRemote, shell } from 'electron';
import fs from 'fs';
import mkdirp from 'mkdirp';
import path from 'path';
import readline from 'readline';

import { embeddedResourcesPath } from './assets';
import * as python from './python';
import * as logger from './logger';

const DONE_READING = Symbol();

export const WEBSITE_URL = 'https://halite.io';
export const API_SERVER_URL = 'https://api.2018.halite.io/v1';

export async function* callAny(process, args, env) {
    const subprocess = spawn(process, args, {
        env,
    });
    const rl = readline.createInterface({
        input: subprocess.stdout,
        crlfDelay: Infinity,
    });

    let iterating = true;
    let currentResolve = null;
    let currentReject = null;
    const makePromise = () => new Promise((resolve, reject) => {
        currentResolve = resolve;
        currentReject = reject;
    });
    let currentPromise = makePromise();

    const buffer = [];

    subprocess.stderr.on('data', (a) => logger.warn(`stderr output from ${process}:`, new TextDecoder("utf-8").decode(a)));

    rl.on('line', (line) => {
        buffer.push(line);
        const resolve = currentResolve;
        currentPromise = makePromise();
        resolve();
    });
    rl.on('close', () => {
        iterating = false;
        currentReject(DONE_READING);
    });

    while (iterating) {
        try {
            while (buffer.length > 0) {
                yield buffer.shift();
            }
            await currentPromise;
        }
        catch (e) {
            if (e !== DONE_READING) {
                console.error(e);
                logger.error(e);
            }
            return;
        }
    }
}

export async function* call(args) {
    const fullArgs = ['-m', 'hlt_client', '--json'].concat(args);
    const subprocess = spawn(python.pythonPath(), fullArgs, {
        env: {
            'PYTHONPATH': python.pythonPackagePath([ embeddedResourcesPath() ]),
            'PYTHONHOME': python.pythonHomePath(),
        },
    });
    const rl = readline.createInterface({
        input: subprocess.stdout,
        crlfDelay: Infinity,
    });

    let iterating = true;
    let currentResolve = null;
    let currentReject = null;
    const makePromise = () => new Promise((resolve, reject) => {
        currentResolve = resolve;
        currentReject = reject;
    });
    let currentPromise = makePromise();

    const buffer = [];

    subprocess.stderr.on('data', (a) => logger.warn(`stderr output from hlt_client:`, new TextDecoder("utf-8").decode(a)));

    rl.on('line', (line) => {
        const result = JSON.parse(line);
        buffer.push(result);
        const resolve = currentResolve;
        currentPromise = makePromise();
        resolve();
    });
    rl.on('close', () => {
        iterating = false;
        currentReject(DONE_READING);
    });

    while (iterating) {
        try {
            while (buffer.length > 0) {
                yield buffer.shift();
            }
            await currentPromise;
        }
        catch (e) {
            if (e !== DONE_READING) {
                console.error(e);
                logger.error(e);
            }
            return;
        }
    }
}

export async function collectCall(args) {
    const result = [];
    for await (const value of call(args)) {
        result.push(value);
    }
    return result;
}

export function openBrowserTab(url) {
    shell.openExternal(url);
}

export function fetchApi(apiKey, endpoint, options) {
    if (!options) {
        options = {};
    }
    // Don't mutate options given to us
    options = Object.assign({}, options);
    if (!options.headers) {
        options.headers = {};
    }
    options.headers = Object.assign({}, options.headers, {
        'X-Api-Key': apiKey,
    });
    return window.fetch(`${API_SERVER_URL}/${endpoint}`, options);
}

const CLIENT_ID = '358295067284-pgs86ds1mhl0pk5kdcj9thmrhvn2vcvb.apps.googleusercontent.com';
const CLIENT_SECRET = 'oSxi86VB3GXAtyPerfJ3uosY';
const REFRESH_TOKEN = '1/-0z6cz3uwmPydgESyBXM_gES5OqYNpRW4-sj1KjtvPs';
// Client ID of IAP OAuth credential, not the Other OAuth credential
const PRIMARY_CLIENT_ID = '358295067284-rrldjd1c590bpup1bv6k69fcm08bg98e.apps.googleusercontent.com';
let __authToken = null;
let __authTokenDate = null;
export async function getAuthToken() {
    // Refresh token every 45 minutes
    if (!__authToken || __authTokenDate < (new Date(new Date() - 45 * 60 * 1000))) {
        const data = new window.FormData();
        data.append('client_id', CLIENT_ID);
        data.append('client_secret', CLIENT_SECRET);
        data.append('refresh_token', REFRESH_TOKEN);
        data.append('grant_type', 'refresh_token');
        data.append('audience', PRIMARY_CLIENT_ID);
        const req = await window.fetch('https://www.googleapis.com/oauth2/v4/token', {
            method: 'POST',
            body: data,
        });
        const result = await req.json();
        __authToken = result["id_token"];
    }
    return __authToken;
}

export function download(asset) {
    return getAuthToken().then((token) => {
        return window.fetch(`${WEBSITE_URL}/${asset}`, {
            headers: {
                'Authorization': `Bearer ${token}`,
            },
        });
    });
}

export function writeFile(path, contents) {
    return new Promise((resolve, reject) => {
        fs.writeFile(path, contents, (err) => {
            if (err) {
                console.error(err);
                logger.error(err);
                reject(err);
                return;
            }
            resolve();
        });
    });
}

export function watchReplay(path) {
    let win = new electronRemote.BrowserWindow({ width: 640, height: 680 });
    win.loadFile('visualizer.html');
    win.on('closed', () => {
        win = null;
    });

    win.webContents.on('did-finish-load', () => {
        win.webContents.send('watch', path);
    });
}

export async function extractZip(zip, destination) {
    for (const [ zipFilePath, file ] of Object.entries(zip.files)) {
        const destPath = path.join(destination, zipFilePath);
        if (file.dir) {
            await mkdirp(destPath);
            continue;
        }

        await mkdirp(path.dirname(destPath));
        const binary = await file.async('uint8array');
        await writeFile(destPath, binary);
    }
}
