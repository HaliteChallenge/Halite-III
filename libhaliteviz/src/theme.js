export const THEMES = {
    "Turtles!": {
        tintFactory: false,
        tintShip: false,
        scale: {
            ship: 1,
        },
        colorTheme: "TheSea",
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
