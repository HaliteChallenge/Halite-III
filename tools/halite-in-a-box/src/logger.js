import { default as logger } from 'electron-log';

function doLog(logFunc, args) {
    logFunc(`Error at: ${new Error().stack}`);
    logFunc(...args);
    for (const arg of args) {
        if (arg instanceof Error || arg.stack) {
            logFunc(`Stack trace: \n ${arg.stack}`);
        }
    }
}

export function error(...args) {
    doLog(logger.error, args);
}
export function warn(...args) {
    doLog(logger.warn, args);
}
