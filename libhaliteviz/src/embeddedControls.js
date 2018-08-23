/**
 * Provide an embeddable visualizer with integrated controls.
 */

import {PLAYER_COLORS} from "./assets";
import {HaliteVisualizer} from "./visualizer";

let injectedCSS = false;

const css = `
.embedded-visualizer {
    position: relative;
    cursor: move;
    color: #FFF;
}

.embedded-selected {
    position: absolute;
    top: 2rem;
    left: 2rem;
    right: 2rem;
    padding: 1rem;

    background: rgba(0,0,0,0.4);
    border-radius: 10px;
    color: rgba(255,255,255,0.8);
    opacity: 0;

    transform: scaleY(0);
    transition: opacity 0.3s ease-in;
}

.embedded-selected-close {
    position: absolute;
    top: 0;
    right: 0;
    background: none;
    border: 0;
    padding: 0 0.5rem 0 1rem;
    margin: 0;
    transition: color 0.3s ease;
    color: #FFF;
}

.embedded-visualizer:hover .embedded-selected.show {
    opacity: 1;
}

.embedded-selected-close:hover {
    color: #FFF;
}

.embedded-clashbar {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    opacity: 0;
    height: 1em;
    transition: opacity 0.3s ease-in;
    display: flex;
    flex-direction: column;
    cursor: help;
    font-size: 1.25em;
}

.embedded-clashbar-energybar {
    height: 1.25em;
    transition: width 0.2s ease;
    color: #FFF;
    font-size: 0.75em;
    line-height: 1.25em;
}

.embedded-toolbar {
    display: flex;
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    opacity: 0;
    transition: opacity 0.3s ease-in;
    padding: 0.25em 0;
    background: linear-gradient(rgba(0, 0, 0, 0), rgba(0, 0, 0, 1), rgba(0, 0, 0, 0.7));
    font-size: 1.25em;
}

.embedded-toolbar span {
    padding: 0 0.5em;
    line-height: 1.5em;
}

.embedded-toolbar input[type=range] {
    flex: 3;
}

.embedded-toolbar .divider {
    border-left: 0.1em solid #FFF;
    margin: 0 0.5em;
    padding: 0;
}

.embedded-toolbar button {
    font-size: inherit;
    font-family: inherit;
    background: none;
    border: 0;
    color: #FFF;
}

.embedded-visualizer:hover .embedded-toolbar {
    opacity: 1;
}
.embedded-visualizer:hover .embedded-clashbar {
    opacity: 1;
}
.embedded-visualizer:hover .embedded-selected {
    transform: none;
}
` + PLAYER_COLORS.map((colorHex, index) => `
.embedded-bg-player-${index + 1} {
    background: #${colorHex.toString(16).padStart(6, '0')};
}
`).join('\n');

const speedList = [
    ['0.5x', 0.5],
    ['1x', 1],
    ['2x', 2],
    ['3x', 3],
    ['4x', 4],
    ['5x', 5],
    ['6x', 6],
    ['7x', 7],
    ['8x', 8],
    ['9x', 9],
    ['10x', 10],
];

function button(label, klass, title=null) {
    const el = document.createElement("button");
    el.innerHTML = label;
    el.classList.add(klass);
    if (title) {
        el.setAttribute("aria-label", title);
        el.setAttribute("title", title);
    }
    return el;
}

export default class EmbeddedVisualizer extends HaliteVisualizer {
    attach(containerEl, keyboardEl=null) {
        const container = typeof containerEl === "string" ?
              document.querySelector(containerEl) : containerEl;
        const realContainer = document.createElement("div");
        container.appendChild(realContainer);
        container.classList.add("embedded-visualizer");

        super.attach(realContainer, container);

        // Default faster
        this.playSpeed = 20;

        // Display of relative energy stores
        const clashbar = document.createElement("div");
        clashbar.classList.add("embedded-clashbar");

        for (let i = 0; i < this.replay.players.length; i++) {
            const bar = document.createElement("div");
            bar.classList.add(`embedded-bg-player-${i+1}`);
            bar.classList.add("embedded-clashbar-energybar");
            clashbar.appendChild(bar);
        }

        // Display of what's selected
        const selected = document.createElement("div");
        const selectedText = document.createElement("span");
        const selectedClose = document.createElement("button");
        selected.classList.add("embedded-selected");
        selectedClose.innerText = "x";
        selectedClose.classList.add("embedded-selected-close");
        selected.appendChild(selectedClose);
        selected.appendChild(selectedText);

        const toolbar = document.createElement("div");
        toolbar.classList.add("embedded-toolbar");

        const pf = button('<i aria-hidden class="fa fa-step-backward"></i>', "embedded-toolbar-prev-frame", "Previous Frame");
        const play = button("", "embedded-toolbar-play");
        const nf = button('<i aria-hidden class="fa fa-step-forward"></i>', "embedded-toolbar-next-frame", "Next Frame");
        const speed = button("", "embedded-toolbar-speed");
        const reset = button('<i aria-hidden class="fa fa-refresh"></i>', "embedded-toolbar-reset", "Reset View");
        const download = button('<i aria-hidden class="fa fa-download"></i>', "embedded-toolbar-download", "Save Replay");
        const progress = document.createElement("span");
        const slider = document.createElement("input");
        const final = document.createElement("span");

        const divider = () => {
            const el = document.createElement("span");
            el.classList.add("divider");
            el.setAttribute("aria-hidden", "aria-hidden");
            return el;
        };

        toolbar.appendChild(speed);
        toolbar.appendChild(divider());
        toolbar.appendChild(pf);
        toolbar.appendChild(play);
        toolbar.appendChild(nf);
        toolbar.appendChild(divider());
        toolbar.appendChild(reset);
        toolbar.appendChild(download);
        toolbar.appendChild(divider());
        toolbar.appendChild(progress);
        toolbar.appendChild(slider);
        toolbar.appendChild(final);

        slider.setAttribute("type", "range");
        slider.setAttribute("min", 0);
        slider.setAttribute("max", this.replay.full_frames.length - 1);
        slider.setAttribute("step", 1);
        final.innerText = this.replay.full_frames.length - 1;

        let selection = null;
        this.onPlay.add(() => {
            play.innerHTML = '<i class="fa fa-pause" title="Pause" aria-hidden></i>';
            play.setAttribute("aria-label", "Pause");
        });
        this.onPause.add(() => {
            play.innerHTML = '<i class="fa fa-play" title="Play" aria-hidden></i>';
            play.setAttribute("aria-label", "Play");
        });
        this.onSelect.add((kind, args) => {
            selection = Object.assign({
                kind,
            }, args);
            console.log(selection);
            selected.classList.add("show");
        });
        selectedClose.addEventListener("click", () => {
            selection = null;
            selected.classList.remove("show");
        });

        let bestEnergySoFar = 500;
        this.onUpdate.add(() => {
            progress.innerText = `${this.frame}`;

            // Only update when necessary so user can still drag the
            // slider
            if (this.frame !== slider.value) {
                slider.value = this.frame;
            }

            if (!this.currentFrame) return;

            if (selection) {
                const lines = [];
                if (selection.kind === "ship") {
                    const ship = this.findShip(this.frame, selection.owner, selection.id);
                    if (ship) {
                        selection.x = ship.x;
                        selection.y = ship.y;
                        lines.push(`${this.replay.players[selection.owner].name} (Player ${selection.owner}) H.M.S. ${selection.id}: ${ship.energy} energy`);
                    }
                }
                if (selection.kind === "point" || selection.kind === "ship") {
                    const energy = this.findCurrentProduction(this.frame, selection.x, selection.y);
                    lines.push(`Point ${selection.x}, ${selection.y}: ${energy} energy`);
                }
                selectedText.innerText = lines.join("\n");
            }

            const energies = this.currentFrame.energy;
            for (const energy of Object.values(energies)) {
                bestEnergySoFar = Math.max(bestEnergySoFar, energy);
            }
            for (const [ id, energy ] of Object.entries(energies)) {
                const width = 100 * Math.max(0.10, energy / bestEnergySoFar);
                const bar = clashbar
                      .querySelector(`div:nth-child(${parseInt(id, 10) + 1})`);
                bar.style.width = `${width}%`;
                bar.innerText = `${energy} energy`;
            }

            const tooltip = [];
            for (let i = 0; i < this.replay.players.length; i++) {
                tooltip.push(`Player ${i} (${this.replay.players[i].name}): ${energies[i]} halite stored.`);
            }
            clashbar.setAttribute("title", tooltip.join(" "));
        });

        play.addEventListener("click", () => {
            if (this.isPlaying()) this.pause();
            else this.play();
        });
        pf.addEventListener("click", () => {
            this.scrub(Math.max(0, this.frame - 1), 0);
        });
        nf.addEventListener("click", () => {
            this.scrub(Math.min(this.replay.full_frames.length - 1, this.frame + 1), 0);
        });

        let speedIndex = 3;
        const updateSpeed = () => {
            speed.innerText = speedList[speedIndex][0];
            speed.setAttribute("title", `Speed multiplier is ${speedList[speedIndex][0]}`);
            this.playSpeed = speedList[speedIndex][1];
        };
        updateSpeed();
        speed.addEventListener("click", () => {
            speedIndex = (speedIndex + 1) % speedList.length;
            updateSpeed();
        });

        reset.addEventListener("click", () => {
            this.camera.reset();
        });

        slider.addEventListener("change", () => {
            if (this.isPlaying()) this.pause();
            progress.innerText = slider.value;
            this.scrub(parseInt(slider.value, 10), 0);
            this.onUpdate.dispatch();
        });

        container.appendChild(clashbar);
        container.appendChild(selected);
        container.appendChild(toolbar);

        if (!injectedCSS) {
            const style = document.createElement("style");
            style.innerText = css;

            const icofont = document.createElement("link");
            icofont.setAttribute("href", "https://unpkg.com/@icon/font-awesome@4.7.0-2/font-awesome.css");
            icofont.setAttribute("rel", "stylesheet");

            document.body.appendChild(style);
            document.querySelector("head").appendChild(icofont);
            injectedCSS = true;
        }

        this.container = container;
    }

    destroy() {
        super.destroy();

        for (const node of this.container.querySelectorAll("*")) {
            node.remove();
        }

        delete this.container;
    }
}
