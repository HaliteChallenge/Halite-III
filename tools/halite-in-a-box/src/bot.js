import { remote as electronRemote } from 'electron';
import JSZip from 'jszip';
import mkdirp from 'mkdirp';
import path from 'path';

import * as assets from "./assets";
import * as util from "./util";

/**
 * Represents a bot you can play against.
 */
class Bot {
    constructor(name) {
        this.name = name;
    }

    get local() {
        return true;
    }

    async makePath() {}
}

export class InterpretedBot extends Bot {
    constructor(name, language, path) {
        super(name);
        this.language = language;
        this.path = path;
    }

    async makePath() {
        if (this.language === InterpretedBot.languages.Python) {
            return `${assets.pythonPath()} ${this.path}`;
        }

        throw new Error("Unrecognized bot interpreter ${this.language}");
    }
}

InterpretedBot.languages = {
    Python: "Python",
};

export function guessInterpreter(path) {
    // TODO: actually guess
    return InterpretedBot.languages.Python;
}

export class RemoteBot extends Bot {
    get local() {
        return false;
    }

    async makePath() {
        const tempDir = electronRemote.app.getPath('temp');
        const { userId, apiKey } = assets.getCredentials();
        const request = await util.fetchApi(
            apiKey,
            `api/user/${userId}/bot/0`,
            {
                headers: {
                    'Accept': 'application/zip',
                },
            });
        const rawZip = await request.arrayBuffer();
        const zip = await JSZip.loadAsync(rawZip);

        let botPath = null;
        for (const [ zipFilePath, file ] of Object.entries(zip.files)) {
            if (path.basename(zipFilePath).startsWith('MyBot')) {
                botPath = zipFilePath;
            }

            const destPath = path.join(tempDir, zipFilePath);
            await mkdirp(path.dirname(destPath));
            const binary = await file.async('uint8array');
            await util.writeFile(destPath, binary);
        }

        const result = path.join(tempDir, botPath);
        console.info('Remote bot temp path:', result);

        const tempBot = new InterpretedBot(this.name, guessInterpreter(result), result);
        return await (tempBot.makePath());
    }
}
