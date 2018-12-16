class Console {
    constructor(el) {
        this.el = el;
        this.ansiUp = new AnsiUp();
        this._rendering = false;
        this._queue = [];
    }

    clear() {
        this.el.innerHTML = '';
    }

    flush(utf8ArrayToString, tty) {
        const text = utf8ArrayToString(tty.output, 0);
        this.writeLine(text);
        tty.output = [];
    }

    wrapEmscripten(utf8ArrayToString, fs) {
        const flush = (tty) => {
        };

        fs.tty.ops.flush = (tty) => {
            if (tty.output && tty.output.length > 0) {
                this.flush(utf8ArrayToString, tty);
            }
            flush(tty);
        };
        fs.tty.ops.put_char = (tty, val) => {
            if (val === null || val === 10) {
                this.flush(utf8ArrayToString, tty);
            }
            else if (val !== 0) {
                tty.output.push(val);
            }
        };
    }

    writeLine(text) {
        this._queue.push(text);
        if (this._rendering) {
            return;
        }
        this._rendering = true;
        window.requestAnimationFrame(() => {
            const html = this._queue
                  .map((text) => this.ansiUp.ansi_to_html(text))
                  .join('\n') + '\n';
            this._queue = [];
            this.el.innerHTML += html;
            this._rendering = false;
            if (window.requestIdleCallback) {
                window.requestIdleCallback(() => {
                    this.el.scrollTop = this.el.scrollHeight;
                }, { timeout: 100 });
            }
        });
    }
}
