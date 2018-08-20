<template functional>
    <modal>
        <h2 slot="header">Confirm Local Bot</h2>

        <template slot="content">
            <p>Selected bot file: {{props.params.localBot}}</p>
            <p>
                Are you sure this is the right bot file? We don't
                recognize the extension <tt>{{props.params.localBotExtension}}</tt>.
                We only support Python3, Java, Node.js, and C++
                directly. (You may also choose a zip file.) For other
                bots, you can override this if you're sure the bot
                will run (e.g. if it's executable
                and has a shebang on GNU/Linux or MacOS, or if it's a
                .exe file on Windows).
            </p>
        </template>

        <template slot="buttons">
            <button @click="props.event('cancel')">Cancel</button>
            <button @click="props.event('override')">Override</button>
        </template>
    </modal>
</template>
