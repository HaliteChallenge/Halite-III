import { WriteStream, createWriteStream, PathLike } from 'fs';

export class Logging {
    static logFile: WriteStream;
    
    static setup(filename: PathLike) {
        this.logFile = createWriteStream(filename, { flags: 'w' });
    }
    
    static format(args: any) {
        return args.map((x:any) => typeof x !== 'string' ? x.toString() : x).join(' ');
    }
    
    static debug(...args: any[]) {
        this.logFile.write(`DEBUG: ${this.format(args)}\n`);
    }
    
    static info(...args: any[]) {
        this.logFile.write(`INFO: ${this.format(args)}\n`);
    }
    
    static warn(...args: any[]) {
        this.logFile.write(`WARN: ${this.format(args)}\n`);
    }
    
    static error(...args: any[]) {
        this.logFile.write(`ERROR: ${this.format(args)}\n`);
    }
}
