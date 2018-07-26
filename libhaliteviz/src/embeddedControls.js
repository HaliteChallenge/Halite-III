/**
 * Provide an embeddable visualizer with integrated controls.
 */

import {HaliteVisualizer} from "./visualizer";
import $ from "jquery";

let injectedCSS = false;

const css = `
.embedded-visualizer {
    position: relative;
    cursor: move;
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
    cursor: help;
}

.embedded-clashbar-energybar {
    height: 1.25rem;
    transition: width 0.2s ease;
    color: #FFF;
    font-size: 0.75em;
    line-height: 1.25rem;
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
}

.embedded-toolbar span {
    padding: 0 0.5em;
}

.embedded-toolbar button {
    font-size: inherit;
    font-family: inherit;
    background: none;
    border: 0;
}

.embedded-visualizer:hover .embedded-toolbar {
    opacity: 1;
}
.embedded-visualizer:hover .embedded-clashbar {
    opacity: 1;
}
`;

function button(label, klass) {
    const el = document.createElement("button");
    el.innerText = label;
    el.classList.add(klass);
    return el;
}

export default class EmbeddedVisualizer extends HaliteVisualizer {
    attach(containerEl, keyboardEl=null) {
        const container = $(containerEl)[0];
        const realContainer = document.createElement("div");
        container.appendChild(realContainer);
        container.classList.add("embedded-visualizer");

        super.attach(realContainer, $(containerEl)[0]);

        // Display of relative energy stores
        const clashbar = document.createElement("div");
        clashbar.classList.add("embedded-clashbar");

        for (let i = 0; i < this.replay.players.length; i++) {
            const bar = document.createElement("div");
            bar.classList.add(`bg-player-${i+1}`);
            bar.classList.add("embedded-clashbar-energybar");
            clashbar.appendChild(bar);
        }

        const toolbar = document.createElement("div");
        toolbar.classList.add("embedded-toolbar");

        const pf = button("<", "embedded-toolbar-prev-frame");
        const play = button("Play", "embedded-toolbar-play");
        const nf = button(">", "embedded-toolbar-next-frame");
        const slower = button("Slower", "embedded-toolbar-slower");
        const faster = button("Faster", "embedded-toolbar-faster");
        const progress = document.createElement("span");
        const slider = document.createElement("input");
        const final = document.createElement("span");

        toolbar.appendChild(pf);
        toolbar.appendChild(play);
        toolbar.appendChild(nf);
        toolbar.appendChild(slower);
        toolbar.appendChild(progress);
        toolbar.appendChild(slider);
        toolbar.appendChild(final);
        toolbar.appendChild(faster);

        slider.setAttribute("type", "range");
        slider.setAttribute("min", 0);
        slider.setAttribute("max", this.replay.full_frames.length - 1);
        slider.setAttribute("step", 1);
        final.innerText = this.replay.full_frames.length - 1;

        this.onPlay.add(() => {
            play.innerText = "Pause";
        });
        this.onPause.add(() => {
            play.innerText = "Play";
        });
        this.onUpdate.add(() => {
            progress.innerText = `${this.frame}`;

            // Only update when necessary so user can still drag the
            // slider
            if (this.frame !== slider.value) {
                slider.value = this.frame;
            }

            if (!this.currentFrame) return;

            const energies = this.currentFrame.energy;
            const widths = {};
            let total = 0;
            let usedWidth = 0;
            for (const [ id, energy ] of Object.entries(energies)) {
                total += energy;
                if (energy === 0) {
                    widths[id] = 10;
                    usedWidth += 10;
                }
            }
            for (const [ id, energy ] of Object.entries(energies)) {
                if (typeof widths[id] === "undefined") {
                    widths[id] = (energy / total) * (100 - usedWidth);
                }
                else if (total === 0) {
                    widths[id] = 100 / Object.keys(energies).length;
                }
            }
            for (const [ id, width ] of Object.entries(widths)) {
                const bar = clashbar
                      .querySelector(`div:nth-child(${parseInt(id, 10) + 1})`);
                bar.style.width = `${width}%`;
                bar.innerText = `${energies[id]} energy`;
            }

            const tooltip = [];
            for (let i = 0; i < this.replay.players.length; i++) {
                tooltip.push(`Player ${i}: ${energies[i]} halite stored.`);
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
        slower.addEventListener("click", () => {
            this.playSpeed = Math.max(1, this.playSpeed - 1);
        });
        faster.addEventListener("click", () => {
            this.playSpeed = Math.min(20, this.playSpeed + 1);
        });
        slider.addEventListener("change", () => {
            if (this.isPlaying()) this.pause();
            progress.innerText = slider.value;
            this.scrub(parseInt(slider.value, 10), 0);
            this.onUpdate.dispatch();
        });

        container.appendChild(clashbar);
        container.appendChild(toolbar);

        if (!injectedCSS) {
            const style = document.createElement("style");
            style.innerText = css;
            document.body.appendChild(style);
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
