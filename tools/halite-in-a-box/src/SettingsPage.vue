<template>
    <section class="tab-panel">
        <section class="card mb-4">
            Engine version: {{haliteVersion}}
        </section>

        <section class="card card-lr">
            <section class="card-l p-4 text-left flex items-center flex-1">
                <div>
                    <h2 class="m-0">Python</h2>

                    <template v-if="pyint === 'embedded'">
                        <p>Running bots via built-in Python (3.7).</p>

                        <button
                            class="btn btn-blue"
                            @click="chooseInterpreter"
                        >
                            Choose Custom Python Interpreter
                        </button>
                        <button
                            class="btn btn-blue"
                            @click="detectInterpreter"
                        >
                            Detect Python Interpreter
                        </button>

                        <p v-if="error" class="error">{{error}}</p>
                    </template>

                    <template v-else>
                        <p>Running bots via custom Python interpreter,
                            located at <tt>{{pyintCustom}}</tt>.</p>
                        <button
                            class="btn btn-blue"
                            @click="resetInterpreter"
                        >
                            Reset to Default Interpreter
                        </button>
                    </template>
                </div>
            </section>

            <section class="card-r p-4 text-left flex-1">
                <p>
                    Choose what version of Python to run. We include
                    Python in this application, but do not include
                    external libraries. To use libraries like NumPy,
                    you may prefer to use your own Python.
                </p>
                <p>
                    Note that NumPy, SciPy, and other libraries are
                    provided for uploaded bots, but you will need a
                    custom install script if you use a library we do
                    not provide by default.
                </p>
            </section>
        </section>
    </section>
</template>

<script>
    import { remote as electronRemote } from 'electron';
    import * as python from './python';
    import * as assets from './assets';

    export default {
        data() {
            return {
                pyint: 'embedded',
                pyintCustom: python.getCustomPython(),
                error: null,
                haliteVersion: 'loading...',
            };
        },
        mounted() {
            assets.getHaliteVersion().then((version) => {
                this.haliteVersion = version;
            });
            setTimeout(() => {
                if (python.getCustomPython()) {
                    this.pyint = 'custom';
                    this.pyintCustom = python.getCustomPython();
                }
            }, 3000);
        },
        methods: {
            chooseInterpreter() {
                this.error = null;
                const paths = electronRemote.dialog.showOpenDialog({
                    title: 'Choose path to Python interpreter',
                    properties: ['openFile'],
                });
                if (paths && paths.length > 0) {
                    const [ path ] = paths;
                    python.validateCustomPython(path).then((result) => {
                        if (!result.valid) {
                            this.error = result.message;
                        }
                        else {
                            python.setCustomPython(path);
                            this.pyint = 'custom';
                            this.pyintCustom = python.getCustomPython();
                        }
                    });
                }
            },
            detectInterpreter() {
                let path = 'python3';
                if (process.platform === 'win32') {
                    path = 'py';
                }
                python.validateCustomPython(path).then((result) => {
                    if (!result.valid) {
                        this.error = result.message;
                    }
                    else {
                        python.setCustomPython(path);
                        this.pyint = 'custom';
                        this.pyintCustom = python.getCustomPython();
                    }
                });
            },
            resetInterpreter() {
                python.setCustomPython('');
                this.pyint = 'embedded';
            },
        },
    };
</script>
