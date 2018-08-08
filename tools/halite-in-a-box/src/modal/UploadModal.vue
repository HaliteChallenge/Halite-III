<template functional>
    <modal>
        <h1 slot="header">Upload New Bot</h1>

        <template slot="content">
            <template v-if="props.params.status === 'previewing'">
                <template v-if="props.params.files">
                    <p>Files to be uploaded:</p>
                    <ul>
                        <li v-for="file in props.params.files">{{file}}</li>
                    </ul>
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
                <button @click="props.event('cancel')">Cancel</button>
                <button @click="props.event('upload')">Upload</button>
            </template>
            <template v-else-if="props.params.status === 'uploaded'">
                <button @click="props.event('Ok')">Ok</button>
            </template>
        </template>
    </modal>
</template>
