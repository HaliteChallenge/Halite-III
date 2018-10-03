/**
 * Helpers for setting up Python invocations.
 */
import path from 'path';
import { spawn } from 'child_process';

import * as assets from './assets';
import { embeddedResourcesPath } from './assets';

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

let __customPython = null;
{
    const savedPython = window.localStorage.getItem("custom-python");

    if (savedPython) {
        __customPython = savedPython;
        validateCustomPython(savedPython).then((valid) => {
            if (!valid) {
                __customPython = null;
            }
        });
    }
}

export function setCustomPython(path) {
    __customPython = path;
    window.localStorage.setItem("custom-python", __customPython);
}

export function validateCustomPython(path) {
    const subprocess = spawn(path, [
        '-c',
        'import sys; import json; print(json.dumps(list(sys.version_info)))',
    ]);
    const data = [];
    subprocess.stdout.on('data', function(chunk) {
        data.push(chunk);
    });

    return Promise.race([
        new Promise((resolve) => {
            subprocess.stdout.on('end', function() {
                resolve();
            });
        }),
        new Promise((resolve) => {
            setTimeout(function() {
                resolve();
            }, 2000);
        }),
    ]).then(function() {
        const msg = data.join('');
        let response = null;
        try {
            response = JSON.parse(msg);
        }
        catch (e) {
            return {
                valid: false,
                message: 'Could not detect Python version - is this a Python interpreter?',
            };
        }

        if (!Array.isArray(response)) {
            return {
                valid: false,
                message: 'Could not detect Python version - is this a Python interpreter?',
            };
        }

        if (response.length < 2) {
            return {
                valid: false,
                message: 'Could not detect Python version - is this a Python interpreter?',
            };
        }

        if (response[0] < 3) {
            return {
                valid: false,
                message: 'Python 2 is not acceptable.',
            };
        }

        if (response[1] < 6) {
            return {
                valid: false,
                message: 'Python < 3.6 is not acceptable.',
            };
        }

        return {
            valid: true,
        };
    });
}

export function getCustomPython() {
    return __customPython;
}

export function botPythonCommand(botPath) {
    if (getCustomPython()) {
        if (process.platform === 'win32') {
            // TODO: unimplemented
            throw new Error("Can't run custom Python on Windows yet.");
        }
        // Reset the environment so our PYTHONPATH/PYTHONHOME don't
        // mess things up
        return assets.setCwd(path.dirname(botPath), `env -i HOME=$HOME PATH=$PATH "${getCustomPython()}" "${botPath}"`);
    }
    return assets.setCwd(path.dirname(botPath), `"${pythonPath()}" "${botPath}"`);
}
