import { remote as electronRemote } from 'electron';
import JSZip from 'jszip';
import mkdirp from 'mkdirp';
import path from 'path';

import * as assets from "./assets";
import * as python from "./python";
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
            return python.botPythonCommand(this.path);
        }

        throw new Error("Unrecognized bot interpreter ${this.language}");
    }
}

InterpretedBot.languages = {
    Python: "Python",
};

export class CompiledBot extends Bot {
    constructor(name, language, path) {
        super(name);
        this.language = language;
        this.path = path;
    }

    async makePath() {
        if (this.language === CompiledBot.languages.CXX) {
            // Detect path to C++ compiler, then compile and return
            // path to actual binary

            // TODO: how to handle things like multiple CPP files?
            // TODO: offer a bailout for more advanced users?  (a
            // shell script or batch file we invoke instead of trying
            // to figure things out ourselves)
        }

        throw new Error("Unrecognized bot compiler ${this.language}");
    }
}

function findCxxCompiler() {
    if (process.platform === 'darwin') {
    }
}

function findJavaCompiler() {
    if (process.platform === 'darwin') {

    }
    else if (process.platform === 'win32') {

    }
    // Assume it's on PATH
    return 'javac';
}

CompiledBot.languages = {
    CXX: "CXX",
    Java: "Java",
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
            if (!file.dir && path.basename(zipFilePath).startsWith('MyBot')) {
                botPath = zipFilePath;
            }
        }
        await util.extractZip(zip, tempDir);

        const result = path.join(tempDir, botPath);
        console.info('Remote bot temp path:', result);

        const tempBot = new InterpretedBot(this.name, guessInterpreter(result), result);
        return await (tempBot.makePath());
    }
}
