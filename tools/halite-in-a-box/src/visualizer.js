import 'reset-css';
import Vue from 'vue';

import './index.scss';
import Visualizer from './Visualizer.vue';

const app = new Vue({
    el: '#app',
    render: h => h(Visualizer),
});
