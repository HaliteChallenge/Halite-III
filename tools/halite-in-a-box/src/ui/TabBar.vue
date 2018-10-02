<template>
    <section class="tab-bar flex flex-col h-full">
        <section class="flex border-b flex-initial items-center flex-no-shrink">
            <slot name="header"></slot>
            <section class="flex-1">
                <template
                    v-for="(tab, index) in tabs"
                >
                    <a
                        v-if="currentTab === index"
                        :class="{'ml-4': index === 0 }"
                        @click="currentTab = index"
                        class="-mb-px cursor-pointer inline-block py-4 px-4 text-cyan text-shadow bg-gradient-cyan">
                        {{tab}}
                    </a>
                    <a
                        v-else
                        :class="{'ml-4': index === 0 }"
                        @click="currentTab = index"
                        class="-mb-px cursor-pointer inline-block py-4 px-4 text-cyan text-shadow hover-bg-gradient-cyan"
                    >
                        {{tab}}
                    </a>
                </template>
            </section>
            <slot name="right"></slot>
        </section>
        <section v-for="(tab, index) in tabs"
                 :style="{ display: index === currentTab ? 'block' : 'none' }"
                 class="flex-1 p-4">
            <slot v-bind:name="`tab-${index}`"></slot>
        </section>
    </section>
</template>

<script>
    export default {
        name: 'tab-bar',
        props: ['tabs'],
        data() {
            return {
                currentTab: 0,
            };
        },
        mounted() {
        },
        watch: {
            currentTab(newIndex) {
                const tab = this.$slots[`tab-${newIndex}`];
                if (tab && tab[0].componentInstance && tab[0].componentInstance.tabOpened) {
                    tab[0].componentInstance.tabOpened();
                }
            },
        },
    };
</script>
