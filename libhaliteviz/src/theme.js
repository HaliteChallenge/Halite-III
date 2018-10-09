export const THEMES = {
    "Turtles!": {
        tintFactory: false,
        tintShip: false,
        scale: {
            ship: 1,
        },
        playerColors: [0x1BB15A, 0xF54356, 0xFABB2C, 0xE65598],
        colorTheme: "TheSea",
    },
    "Colorblind": {
        tintFactory: false,
        tintShip: false,
        scale: {
            ship: 1,
        },
        colorTheme: "Colorblind",
    },
    "Halite II": {
        tintFactory: true,
        tintShip: true,
        scale: {
            ship: 0.75,
        },
        colorTheme: "GraySpace",
    },
};


let __injectedCSS = null;
export function applyTheme() {
    if (!__injectedCSS) {
        __injectedCSS = document.createElement("style");
    }
    __injectedCSS.innerText = "";
}

export let selectedTheme = "Turtles!";

{
    let savedTheme = window.localStorage.getItem('visualizer-theme');
    if (THEMES[savedTheme]) {
        selectedTheme = savedTheme;
    }
}

export default function theme() {
    return THEMES[selectedTheme];
}

export function setTheme(value) {
    if (!THEMES[value]) {
        console.error(`Theme ${value} does not exist.`);
        return;
    }

    window.localStorage.setItem('visualizer-theme', value);
}
