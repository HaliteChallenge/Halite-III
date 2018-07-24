const path = require("path");

module.exports = {
    entry: "./src/main.js",
    output: {
        path: path.resolve(__dirname, "dist"),
        filename: "bundle.js",
        library: "libhaliteviz",
    },
    devtool: "source-map",
    module: {
        noParse: /libzstd/,
        rules: [
            {
                test: /\.js$/,
                exclude: /node_modules/,
                use: {
                    loader: 'babel-loader',
                },
            },
            {
                test: /\.png$/,
                loader: "file-loader",
            },
        ],
    },
};
