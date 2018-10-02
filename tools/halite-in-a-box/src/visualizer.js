import Vue from 'vue';

import './index.css';
import Visualizer from './Visualizer.vue';

const app = new Vue({
    el: '#app',
    render: h => h(Visualizer),
});
