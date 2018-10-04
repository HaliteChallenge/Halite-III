<template>
    <section class="tab-panel">
        <section class="card card-lr flex-no-shrink">
            <section class="card-l flex items-center justify-center">
                <h2>Halite in a Box</h2>
            </section>
            <section class="card-r p-4 text-left">
                <p>
                    A tool to help you develop your bot by giving you
                    ways to evaluate how well it performs before
                    submission.
                </p>
                <p>
                    Run games locally with the 'Local Bot' tab, then
                    review past games, or check on how your submitted
                    bot is doing.
                </p>
            </section>
        </section>
        <section class="card card-lr my-4 flex-no-shrink">
            <section class="card-l flex items-center justify-center">
                <h3>Halite Game Docs</h3>
            </section>
            <section class="card-r p-4 text-left">
                <ul>
                    <li
                        v-for="page in pages"
                        class="cursor-pointer underline"
                        @click="helpPage = page.html"
                    >
                        {{page.title}}
                    </li>
                </ul>
            </section>
        </section>
        <template v-if="helpPage">
            <hr class="w-full border-grey border-t" />
            <section v-html="helpPage" class="overflow-y-scroll"></section>
        </template>
    </section>
</template>

<script>
    import apiDocs from '../../../website/learn-programming-challenge/api-docs.md';
    import gameOverview from '../../../website/learn-programming-challenge/game-overview.md';
    import developingBot from '../../../website/learn-programming-challenge/developing-a-bot.md';

    export default {
        data() {
            return {
                helpPage: null,
                pages: [
                    { title: 'Game Overview', html: gameOverview },
                    { title: 'API Documentation', html: apiDocs },
                    { title: 'Developing a Bot', html: developingBot },
                ],
            };
        },
    };
</script>
