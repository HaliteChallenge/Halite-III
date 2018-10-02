import Vue from 'vue';

import * as logger from './logger';

import './index.css';
import Main from './Main.vue';
import Modal from './Modal.vue';
import BenchmarkModal from './modal/BenchmarkModal.vue';
import LocalBotModal from './modal/LocalBotConfirm.vue';
import UploadModal from './modal/UploadModal.vue';
import TabBar from './ui/TabBar.vue';


Vue.component('modal', Modal);
Vue.component('tab-bar', TabBar);
Vue.component('benchmark-modal', BenchmarkModal);
Vue.component('local-bot-confirm-modal', LocalBotModal);
Vue.component('upload-modal', UploadModal);

window.addEventListener('unhandledrejection', (event) => {
    console.error(event.promise);
    console.error(event.reason);

    logger.error('Unhandled rejection!', event.promise);
    logger.error('Reason:', event.reason);
});

process.on('uncaughtException', (err) => {
    logger.error('Unhandled exception!', err);
});

const app = new Vue({
    el: '#app',
    render: h => h(Main),
});
