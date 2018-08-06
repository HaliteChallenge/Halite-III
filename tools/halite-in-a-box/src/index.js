import Vue from "vue";

import Main from "./Main.vue";

const app = new Vue({
    el: "#app",
    render: h => h(Main),
});
