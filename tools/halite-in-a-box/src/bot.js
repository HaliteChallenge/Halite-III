import * as assets from "./assets";

/**
 * Represents a bot you can play against.
 */
class Bot {
    constructor(name) {
        this.name = name;
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

export class RemoteBot extends Bot {
    async makePath() {

    }
}
