export const THEMES = {
    "Turtles!": {
        tintFactory: false,
        tintShip: false,
        rotateShip: true,
        scaleMapSprite: true,
        scale: {
            ship: 1,
        },
        playerColors: [0x1BB15A, 0xF54356, 0xFABB2C, 0xffa2e2],
        colorTheme: "TheSea",
    },
    "Colorblind": {
        tintFactory: false,
        tintShip: false,
        rotateShip: true,
        scaleMapSprite: true,
        scale: {
            ship: 1,
        },
        playerColors: [0x5E5EFF, 0xAEAEE6, 0x575738, 0xBFBF00],
        colorTheme: "Colorblind",
    },
    "Halite II": {
        tintFactory: true,
        tintShip: true,
        rotateShip: true,
        scaleMapSprite: true,
        scale: {
            ship: 0.75,
        },
        playerColors: [0xBD00DB, 0x63CECA, 0xFFBE00, 0xC5EC98],
        colorTheme: "GraySpace",
    },
    "Roguelike": {
        tintFactory: true,
        tintShip: true,
        scaleMapSprite: false,
        mapSprite: true,
        dropoffSprite: true,
        rotateShip: false,
        borderSprite: true,
        scale: {
            ship: 1,
        },
        playerColors: [0x1BB15A, 0xF54356, 0xFABB2C, 0xffa2e2],
        colorTheme: "TheSea",
    },
};


export function applyTheme() {
    if (theme().playerColors) {
        const { playerColors } = theme();

        const rules = [];
        const sheet = window.document.styleSheets[0];
        for (let i = 0; i < playerColors.length; i++) {
            sheet.insertRule(`.color-${i+1} { color: #${playerColors[i].toString(16)} !important; }`,
                             sheet.cssRules.length);
        }
    }
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
