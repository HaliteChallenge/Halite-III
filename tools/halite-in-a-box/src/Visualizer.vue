<template>
    <div ref="visualizer"></div>
</template>

<script>
    import electron from 'electron';
    import fs from 'fs';
    import * as libhaliteviz from 'libhaliteviz';

    export default {
        data() {
            return {};
        },

        mounted() {
            electron.ipcRenderer.on('watch', (event, path) => {
                fs.readFile(path, async (err, data) => {
                    if (err) {
                        // TODO:
                        return;
                    }

                    const replay = await libhaliteviz.parseReplay(data.buffer);
                    await libhaliteviz.setAssetRoot('');

                    const dim = Math.min(window.innerWidth, window.innerHeight);
                    const visualizer = new libhaliteviz.EmbeddedVisualizer(replay, dim, dim);
                    visualizer.attach(this.$refs.visualizer);
                });
            });
        },
    }
</script>

<style>
</style>
