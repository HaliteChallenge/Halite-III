const fs = require('fs');

let logFile;

module.exports = {
    setup(filename) {
        logFile = fs.createWriteStream(filename, { flags: 'w' });
    },

    format(args) {
        return args.map(x => typeof x !== 'string' ? x.toString() : x).join(' ');
    },

    debug(...args) {
        logFile.write(`DEBUG: ${this.format(args)}\n`);
    },

    info(...args) {
        logFile.write(`INFO: ${this.format(args)}\n`);
    },

    warn(...args) {
        logFile.write(`WARN: ${this.format(args)}\n`);
    },

    error(...args) {
        logFile.write(`ERROR: ${this.format(args)}\n`);
    },
};
