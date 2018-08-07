import 'reset-css';
import 'milligram';
import Vue from 'vue';

import './index.scss';
import Main from './Main.vue';

const app = new Vue({
    el: '#app',
    render: h => h(Main),
});
