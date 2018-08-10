const path = require('path');
const VueLoaderPlugin = require('vue-loader/lib/plugin');

module.exports = {
    entry: {
        main: ['babel-polyfill', './src/index.js'],
        visualizer: ['babel-polyfill', './src/visualizer.js'],
    },
    target: 'electron-renderer',
    module: {
        rules: [
            {
                test: /\.vue$/,
                loader: 'vue-loader',
                options: {
                    optimizeSSR: false,
                },
            },
            {
                test: /\.js$/,
                loader: 'babel-loader',
                exclude: /(libzstd|encoding-indexes)/,
            },
            {
                test: /\.css$/,
                use: [ 'style-loader', 'css-loader' ]
            },
            {
                test: /\.scss$/,
                use: [
                    'style-loader',
                    'css-loader',
                    'sass-loader',
                ]
            },
            {
                test: /\.png$/,
                loader: "file-loader",
            },
        ],
    },
    output: {
        filename: '[name].js',
        path: path.resolve(__dirname, 'dist'),
        publicPath: 'dist/',
    },
    plugins: [
        new VueLoaderPlugin(),
    ],
};
