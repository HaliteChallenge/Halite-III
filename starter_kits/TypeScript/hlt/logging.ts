import { createWriteStream, PathLike, WriteStream } from "fs";

export class Logging {
    public static logFile: WriteStream;

    public static setup(filename: PathLike) {
        this.logFile = createWriteStream(filename, { flags: "w" });
    }

    public static format(args: any) {
        return args.map((x: any) => typeof x !== "string" ? x.toString() : x).join(" ");
    }

    public static debug(...args: any[]) {
        this.logFile.write(`DEBUG: ${this.format(args)}\n`);
    }

    public static info(...args: any[]) {
        this.logFile.write(`INFO: ${this.format(args)}\n`);
    }

    public static warn(...args: any[]) {
        this.logFile.write(`WARN: ${this.format(args)}\n`);
    }

    public static error(...args: any[]) {
        this.logFile.write(`ERROR: ${this.format(args)}\n`);
    }
}
