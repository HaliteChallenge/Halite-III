<template functional>
    <modal>
        <h2 slot="header">Upload New Bot</h2>

        <template slot="content">
            <template v-if="props.params.status === 'previewing'">
                <template v-if="props.params.files">
                    <p>Files to be uploaded:</p>
                    <textarea class="w-full my-4">
{{props.params.files.join("\n")}}
                    </textarea>
                    <p>(A zip file will be created for you.)</p>
                </template>
                <p v-else>Uploading {{props.params.localBot}}</p>
            </template>
            <template v-else-if="props.params.status === 'uploading'">
                <p>Uploading...</p>
            </template>
            <template v-else-if="props.params.status === 'uploaded'">
                <p>Upload successful!</p>
            </template>
        </template>

        <template slot="buttons">
            <template v-if="props.params.status === 'previewing'">
                <button class="btn btn-blue" @click="props.event('cancel')">Cancel</button>
                <button class="btn btn-blue" @click="props.event('upload')">Upload</button>
            </template>
            <template v-else-if="props.params.status === 'uploaded'">
                <button class="btn btn-blue" @click="props.event('Ok')">Ok</button>
            </template>
        </template>
    </modal>
</template>

<style lang="scss" scoped>
    p {
        margin-bottom: 0;
    }

    textarea {
        font-family: "Fira Mono", monospace;
        font-size: 1rem;
        height: 30vh;
    }
</style>
