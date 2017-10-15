import LZString from "./lz-string";
import fetch from "isomorphic-fetch";

const getCurrentVersion = () => {
  //return getQueryParam('v');
  return window.timeboxedVersion || getQueryParam("v");
};

const getPlatform = () => {
  //return getQueryParam('p');
  return window.pebblePlatform || getQueryParam("p");
};

const getConfigs = () => {
  let config = decodeURIComponent(
    window.timeboxedConfigs || getQueryParam("c")
  );
  try {
    JSON.parse(decodeURIComponent(config));
    return decodeURIComponent(config);
  } catch (error) {
    return LZString.decompressFromBase64(config);
  }
};

const getReturnUrl = () => {
  //return decodeURIComponent(getQueryParam('return_to', 'pebblejs://close#'));
  return decodeURIComponent(window.pebbleReturnTo || "pebblejs://close#");
};

const getQueryParam = (variable, defaultValue) => {
  let query = location.search.substring(1);
  let vars = query.split("&");
  for (let i = 0; i < vars.length; i++) {
    let pair = vars[i].split("=");
    if (pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || "";
};

const checkForUpdates = callback => {
  if (!getCurrentVersion()) {
    callback(false);
  }
  let url = `http://pblweb.com/api/v1/version/1354d7dc-b9e5-420d-9edf-533ee2fd4520.json?current=${getCurrentVersion()}`;

  fetch(url)
    .then(response => {
      if (response.status >= 400) {
        throw new Error("Bad response from server");
      }
      return response.json();
    })
    .then(json => {
      callback(json.newer, json.version);
    })
    .catch(() => {
      callback(false);
    });
};

const providerUrls = {
  "0":
    "http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3&q=${location}",
  "1":
    "http://api.wunderground.com/api/${apiKey}/conditions/forecast/q/${location}.json",
  "2":
    "https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places%20where%20text%3D%22${location}%22)",
  "3":
    "https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=select%20centroid%20from%20geo.places(1)%20where%20text%3D%22${location}%22"
};

const verifyLocation = (loc, provider, apiKey, callback = () => {}) => {
  if (!loc) {
    callback(false);
    return;
  }
  apiKey = apiKey || "";
  loc = encodeURIComponent(loc);
  let url = providerUrls[provider]
    .replace("${location}", loc)
    .replace("${apiKey}", apiKey);

  fetch(url)
    .then(response => {
      if (response.status >= 400) {
        throw new Error("Bad response from server");
      }
      return response.json();
    })
    .then(json => {
      switch (provider) {
        case "0":
          if (parseInt(json.cod, 10) === 404) {
            alert("Invalid location");
            callback(false);
          } else {
            alert("Valid location!");
            callback(true);
          }
          break;
        case "1":
          if (json.response.error || !json.current_observation) {
            if (
              json.response.error &&
              json.response.error.type === "keynotfound"
            ) {
              alert("Invalid WeatherUnderground Key");
              callback(false);
            } else {
              alert("Invalid location");
              callback(false);
            }
          } else {
            alert("Valid location!");
            callback(true);
          }
          break;
        case "2":
          if (json.query.count === 0) {
            alert("Invalid location");
            callback(false);
          } else {
            alert("Valid location!");
            callback(true);
          }
          break;
        case "3":
          if (json.query.count === 0) {
            alert("Invalid location");
            callback(false);
          } else {
            alert("Valid location!");
            callback(true);
          }
          break;
        default:
          callback(false);
          break;
      }
      return null;
    })
    .catch(ex => {
      console.log(ex.stack); // eslint-disable-line no-console
      callback(false);
    });
};

const fetchMasterKeyData = (email, pin, callback = () => {}) => {
  if (!email || !pin) {
    callback({});
    return;
  }

  let url = `https://pmkey.xyz/search/?email=${email}&pin=${pin}`;
  fetch(url)
    .then(response => {
      if (response.status >= 400) {
        throw new Error("Bad response from server");
      }
      return response.json();
    })
    .then(json => {
      if (!json.success || !json.keys || !json.keys.weather) {
        callback({});
        throw new Error("No keys found");
      } else {
        callback(json.keys.weather);
      }
      return null;
    })
    .catch(ex => {
      console.log(ex.stack); // eslint-disable-line no-console
      alert(ex.message);
      callback({});
    });
};

export {
  getCurrentVersion,
  checkForUpdates,
  verifyLocation,
  getReturnUrl,
  getPlatform,
  getConfigs,
  fetchMasterKeyData
};
