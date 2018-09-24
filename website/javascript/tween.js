export function driveWhile(condition, update) {
    return new Promise((resolve) => {
        const loop = (dt) => {
            if (condition()) {
                update(dt);

                window.requestAnimationFrame(loop);
            }
            else {
                resolve();
            }
        };

        loop(0);
    });
}
