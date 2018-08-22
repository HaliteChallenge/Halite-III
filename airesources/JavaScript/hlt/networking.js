module.exports = {
    /**
     * Sends a list of commands to the engine.
     * @param commands The list of commands to send.
     * @returns a Promise fulfilled once stdout is drained.
     */
    sendCommands: function sendCommands(commands) {
        return new Promise((resolve) => {
            process.stdout.write(commands.join(' ') + '\n', function() {
                resolve();
            });
        });
    },
};
