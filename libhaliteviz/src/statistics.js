/**
 * A helper to parse the replay and compute statistics for a match.
 */
export class Statistics {
    constructor(replay) {
        this.frames = [];

        for (let frameIdx = 0; frameIdx < replay.full_frames.length; frameIdx++) {
            const frameStats = {
                players: {},
            };

            const curFrame = replay.full_frames[frameIdx];

            for (const playerId of Object.keys(replay.players)) {
                let playerStats = {
                    totalShips: 0,
                    currentShips: 0,
                    currentEnergy: 0,
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
                for (let event of curFrame.events) {
                }
            }

            this.frames.push(frameStats);
        }
    }
}
