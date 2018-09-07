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
                    currentDropoffs: frameIdx > 0 ?
                        this.frames[frameIdx - 1].players[playerId].currentDropoffs :
                        0,
                    currentCollisions: frameIdx > 0 ?
                        this.frames[frameIdx - 1].players[playerId].currentCollisions :
                        0,
                    depositedHalite: frameIdx > 0 ?
                        this.frames[frameIdx - 1].players[playerId].depositedHalite :
                        0, // net halite
                };

                if (frameIdx > 0) {
                    let prevFrameStats = this.frames[this.frames.length - 1].players[playerId];
                    playerStats = Object.assign({}, prevFrameStats);

                    // Replay records player energy at -end- of turn, annoyingly.
                    const prevFrame = replay.full_frames[frameIdx - 1];
                    if (prevFrame.energy && typeof prevFrame.energy[playerId] !== "undefined") {
                        playerStats.currentEnergy = prevFrame.energy[playerId];
                    }
                }
                else {
                    playerStats.currentEnergy = replay.GAME_CONSTANTS.INITIAL_ENERGY;
                }

                if (curFrame.entities) {
                    playerStats.currentShips = Object.keys(curFrame.entities[playerId] || {}).length;
                }

                if (replay.game_statistics.player_statistics[playerId].turn_deposited) {
                    playerStats.depositedHalite = replay.game_statistics.player_statistics[playerId].turn_deposited[frameIdx];
                }

                frameStats.players[playerId] = playerStats;
            }

            if (curFrame.events) {
                for (const event of curFrame.events) {
                    if (event.type === 'construct') {
                        frameStats.players[event.owner_id].currentDropoffs++;
                    }
                    else if (event.type === 'shipwreck') {
                        // this is a terrible loop complexity-wise.
                        for (const id of event.ships) {
                            for (const [playerId, playerShips] of
                                 Object.entries(curFrame.entities)) {
                                if (playerShips[id]) {
                                    frameStats.players[playerId].currentCollisions++;
                                }
                            }
                        }
                    }
                }
            }

            this.frames.push(frameStats);
        }
    }
}
