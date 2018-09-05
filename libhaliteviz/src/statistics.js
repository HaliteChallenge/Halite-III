/**
 * A helper to parse the replay and compute statistics for a match.
 */
export class Statistics {
    constructor(replay) {
        this.frames = [];

        const cellMap = [];
        for (const row of replay.production_map.grid) {
            for (const cell of row) {
                cellMap.push(cell.energy);
            }
        }
        this.totalHalite = cellMap.reduce((total, cell) => total + cell, 0);

        for (let frameIdx = 0; frameIdx < replay.full_frames.length; frameIdx++) {
            const frameStats = {
                players: {},
                remainingHalite: this.frames.length > 0 ?
                    this.frames[this.frames.length - 1].remainingHalite :
                    this.totalHalite,
            };

            if (frameIdx > 0) {
                const changedCells = replay.full_frames[frameIdx - 1].cells;
                for (const cell of changedCells) {
                    cellMap[cell.y * replay.production_map.width + cell.x] = cell.production;
                }

                frameStats.remainingHalite = cellMap.reduce((total, cell) => total + cell, 0);
            }

            const curFrame = replay.full_frames[frameIdx];

            for (const playerId of Object.keys(replay.players)) {
                let playerStats = {
                    totalShips: 0,
                    currentShips: 0,
                    currentEnergy: 0,
                    currentDropoffs: 0,
                    currentCollisions: 0,
                    depositedHalite: 0, // net halite
                };

                if (frameIdx > 0) {
                    let prevFrameStats = this.frames[this.frames.length - 1].players[playerId];
                    playerStats = Object.assign({}, prevFrameStats);
                }

                if (curFrame.entities) {
                    playerStats.currentShips = Object.keys(curFrame.entities[playerId] || {}).length;
                }
                if (curFrame.energy && typeof curFrame.energy[playerId] !== "undefined") {
                    playerStats.currentEnergy = curFrame.energy[playerId];
                }

                frameStats.players[playerId] = playerStats;
            }

            if (curFrame.events) {
                for (const event of curFrame.events) {
                }
            }

            this.frames.push(frameStats);
        }
    }
}
