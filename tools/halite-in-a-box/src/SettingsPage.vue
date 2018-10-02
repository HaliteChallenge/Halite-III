<template>
    <section class="tab-panel">
        <section class="card card-lr">
            <section class="card-l p-4 text-left">
                <h2>Python</h2>
                <ul class="list-reset">
                    <li>
                        <input id="pyint-embedded" name="pyint" type="radio" value="embedded" class="mr-4" checked /><label for="pyint-embedded">Built-In Python (Python 3.7)</label>
                    </li>
                    <li>
                        <input id="pyint-embedded" name="pyint" type="radio" value="embedded" class="mr-4" /><label for="pyint-embedded">Choose Your Own</label>
                    </li>
                </ul>
            </section>

            <section class="card-r p-4">
                <p>
                    Choose what version of Python to run. We include
                    Python in this application, but for libraries like
                    NumPy, you may prefer to use your own Python.
                </p>
            </section>
        </section>
    </section>
</template>

<script>
    export default {
        data() {
            return {};
        },
    };
</script>
