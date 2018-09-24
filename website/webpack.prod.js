const merge = require('webpack-merge');
const commonConfig = require('./webpack.common.js');
const webpack = require("webpack");
const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;

module.exports = merge(commonConfig, {
    plugins: [
        new webpack.LoaderOptionsPlugin({
            minimize: true,
            debug: false
        }),
        new webpack.DefinePlugin({
            'process.env': {
                'NODE_ENV': JSON.stringify('production'),
                'BABEL_ENV': 'production',
            }
        }),
        new BundleAnalyzerPlugin({
            analyzerMode: "static",
            openAnalyzer: false,
        }),
    ]
});
