import 'reset-css';
import 'milligram';
import Vue from 'vue';

import './index.scss';
import Visualizer from './Visualizer.vue';

const app = new Vue({
    el: '#app',
    render: h => h(Visualizer),
});
