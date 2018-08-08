import 'reset-css';
import 'milligram';
import Vue from 'vue';

import './index.scss';
import Main from './Main.vue';
import Modal from './Modal.vue';
import BenchmarkModal from './modal/BenchmarkModal.vue';
import UploadModal from './modal/UploadModal.vue';

Vue.component('modal', Modal);
Vue.component('benchmark-modal', BenchmarkModal);
Vue.component('upload-modal', UploadModal);

const app = new Vue({
    el: '#app',
    render: h => h(Main),
});
