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
                options: {
                    presets: [
                        ['env', {
                            targets: {
                                electron: '2.0.7',
                            },
                            modules: false,
                            useBuiltIns: true,
                        }],
                    ]
                },
            },
            {
                test: /\.css$/,
                exclude: /node_modules/,
                use: [
                    {
                        loader: 'style-loader',
                    },
                    {
                        loader: 'css-loader',
                        options: {
                            importLoaders: 1,
                        }
                    },
                    {
                        loader: 'postcss-loader'
                    }
                ]
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
                loader: "url-loader",
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
