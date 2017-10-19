//  OpenShift sample Node application
var express = require("express");
var path = require("path");
var app = express();
var morgan = require("morgan");

Object.assign = require("object-assign");

var routes = require("./routes/index");

// view engine setup
app.set("views", path.join(__dirname, "views"));
app.set("view engine", "jade");
app.use(morgan("combined"));
app.use(express.static("dist"));

var port = process.env.PORT || process.env.OPENSHIFT_NODEJS_PORT || 8080,
  ip = process.env.IP || process.env.OPENSHIFT_NODEJS_IP || "0.0.0.0",
  mongoURL = process.env.OPENSHIFT_MONGODB_DB_URL || process.env.MONGO_URL,
  mongoURLLabel = "";

if (mongoURL == null && process.env.DATABASE_SERVICE_NAME) {
  var mongoServiceName = process.env.DATABASE_SERVICE_NAME.toUpperCase(),
    mongoHost = process.env[mongoServiceName + "_SERVICE_HOST"],
    mongoPort = process.env[mongoServiceName + "_SERVICE_PORT"],
    mongoDatabase = process.env[mongoServiceName + "_DATABASE"],
    mongoPassword = process.env[mongoServiceName + "_PASSWORD"];
  mongoUser = process.env[mongoServiceName + "_USER"];

  if (mongoHost && mongoPort && mongoDatabase) {
    mongoURLLabel = mongoURL = "mongodb://";
    if (mongoUser && mongoPassword) {
      mongoURL += mongoUser + ":" + mongoPassword + "@";
    }
    // Provide UI label that excludes user id and pw
    mongoURLLabel += mongoHost + ":" + mongoPort + "/" + mongoDatabase;
    mongoURL += mongoHost + ":" + mongoPort + "/" + mongoDatabase;
  }
}
var db = null,
  dbDetails = {};

var initDb = function(callback) {
  if (mongoURL == null) return;

  var mongodb = require("mongodb");
  if (mongodb == null) return;

  mongodb.connect(mongoURL, function(err, conn) {
    if (err) {
      callback(err);
      return;
    }

    db = conn;
    dbDetails.databaseName = db.databaseName;
    dbDetails.url = mongoURLLabel;
    dbDetails.type = "MongoDB";

    console.log("Connected to MongoDB at: %s", mongoURL);
  });
};

app.use("/", routes);
app.use("/pebble-apps/timeboxed/config/", routes);

// error handling
app.use(function(err, req, res, next) {
  console.error(err.stack);
  res.status(500).send("Something bad happened!");
});

initDb(function(err) {
  console.log("Error connecting to Mongo. Message:\n" + err);
});

app.listen(port, ip);
console.log("Server running on http://%s:%s", ip, port);

module.exports = app;
