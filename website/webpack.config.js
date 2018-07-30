const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;
const merge = require('webpack-merge');
const commonConfig = require('./webpack.common.js');

module.exports = merge(commonConfig, {
    devtool: "source-map",
    plugins: [
        new BundleAnalyzerPlugin({
            analyzerMode: "static",
        }),
    ],
});
