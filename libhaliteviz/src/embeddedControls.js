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

.embedded-toolbar {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    opacity: 0;
    transition: opacity 0.3s ease-in;
    padding: 0.25em 0;
    background: linear-gradient(rgba(0, 0, 0, 0), rgba(0, 0, 0, 1), rgba(0, 0, 0, 0.7));
}

.embedded-toolbar button {
    font-size: inherit;
    font-family: inherit;
    background: none;
    border: 0;
}

.embedded-visualizer:hover .embedded-toolbar {
    display: block;
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

        const toolbar = document.createElement("div");
        toolbar.classList.add("embedded-toolbar");

        const pf = button("<", "embedded-toolbar-prev-frame");
        const play = button("Play", "embedded-toolbar-play");
        const nf = button(">", "embedded-toolbar-next-frame");
        const slower = button("Slower", "embedded-toolbar-slower");
        const faster = button("Faster", "embedded-toolbar-faster");
        const progress = document.createElement("span");

        toolbar.appendChild(pf);
        toolbar.appendChild(play);
        toolbar.appendChild(nf);
        toolbar.appendChild(slower);
        toolbar.appendChild(progress);
        toolbar.appendChild(faster);

        this.onPlay.add(() => {
            play.innerText = "Pause";
        });
        this.onPause.add(() => {
            play.innerText = "Play";
        });
        this.onUpdate.add(() => {
            progress.innerText = `${this.frame}/${this.replay.full_frames.length - 1}`;
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

        container.appendChild(toolbar);

        if (!injectedCSS) {
            const style = document.createElement("style");
            style.innerText = css;
            document.body.appendChild(style);
            injectedCSS = true;
        }
    }
}
