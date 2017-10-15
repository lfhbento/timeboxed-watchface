/* globals __dirname, module */
/* eslint-disable */
var webpack = require("webpack");
var autoprefixer = require("autoprefixer");

var config = {
  context: __dirname + "/app",
  entry: {
    app: "./js/main.js",
    "app.min": "./js/main.js"
  },
  output: {
    path: __dirname + "/dist",
    filename: "[name].js"
  },
  plugins: [
    new webpack.ProvidePlugin({
      $: "jquery",
      jQuery: "jquery",
      "window.jQuery": "jquery"
    }),
    new webpack.DefinePlugin({
      "process.env": {
        NODE_ENV: JSON.stringify("production")
      }
    }),
    new webpack.optimize.UglifyJsPlugin({
      minimize: true,
      compress: { warnings: false },
      include: /\.min\.js$/
    })
  ],
  devtool: "source-map",
  module: {
    loaders: [
      {
        test: /\.js/,
        loader: "babel-loader",
        exclude: /node_modules\//,
        query: { presets: ["es2015", "react", "stage-2"] }
      },
      {
        test: /\.scss$/,
        loaders: [
          "style-loader",
          "css-loader?-autoprefixer",
          "postcss-loader",
          "sass-loader"
        ]
      },
      { test: /\.css$/, loaders: ["style-loader", "css-loader?-autoprefixer"] },
      {
        test: /\.woff(2)?(\?v=[0-9]\.[0-9]\.[0-9])?$/,
        loader: "url-loader?limit=10000&minetype=application/font-woff"
      },
      {
        test: /\.(ttf|eot|svg)(\?v=[0-9]\.[0-9]\.[0-9])?$/,
        loader: "file-loader?name=/[hash].[ext]"
      }
    ]
  },
  postcss: [autoprefixer({ browsers: ["iOS 8"] })]
};

module.exports = config;
