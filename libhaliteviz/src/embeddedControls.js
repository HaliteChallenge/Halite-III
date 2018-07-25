/**
 * Provide an embeddable visualizer with integrated controls.
 */

import {HaliteVisualizer} from "./visualizer";
import $ from "jquery";

let injectedCSS = false;

const css = `
.embedded-visualizer {
    position: relative;
}

.embedded-toolbar {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    opacity: 0;
    display: none;
    transition: opacity 0.3s ease-in;
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

        const play = button("Play", "embedded-toolbar-play");
        const speed = button("Speed", "embedded-toolbar-speed");

        toolbar.appendChild(play);
        toolbar.appendChild(speed);

        play.addEventListener("click", () => {

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
