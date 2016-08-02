/*globals Pebble*/
/*jshint node: true*/
'use strict';

var currentVersion = "3.3";

var OPEN_WEATHER = 0;
var WUNDERGROUND = 1;
var YAHOO = 2;
var FORECAST = 3;

Pebble.addEventListener("ready",
    function(e) {
        console.log("Pebble Ready!");
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');
        if (e.payload.KEY_HASUPDATE) {
            console.log('Checking for updates...');
            checkForUpdates();
        } else {
            console.log('Fetching weather info...');
            var weatherKey = localStorage.weatherKey;
            var provider = weatherKey ? 1 : 0;
            if (localStorage.weatherProvider) {
                provider = parseInt(localStorage.weatherProvider, 10);
                switch (provider) {
                    case WUNDERGROUND:
                        weatherKey = localStorage.weatherKey;
                        break;
                    case FORECAST:
                        weatherKey = localStorage.forecastKey;
                        break;
                    default:
                        weatherKey = '';
                }
                console.log(weatherKey);
            }
            getWeather(provider, weatherKey, parse(localStorage.useCelsius.toLowerCase()), localStorage.overrideLocation);
        }
    }
);

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL(
        'http://www.lbento.space/pebble-apps/timeboxed/config/?v=' + currentVersion +
        '&p=' + Pebble.getActiveWatchInfo().platform +
        '&l=' + Pebble.getActiveWatchInfo().language)
});

Pebble.addEventListener('webviewclosed', function(e) {
    if (!e.response) {
        return;
    }
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log(JSON.stringify(configData));

    var dict = {};

    for (var item in configData) {
        var key = 'KEY_' + item.toUpperCase();
        var value = configData[item];
        if (String(value).indexOf('0x') !== -1) {
            value = parseInt(value, 16);
        }
        if (String(value).indexOf('|') !== -1) {
            var newValue = value.split('|')[1].split(':')[0];
            dict[key + 'CODE'] = value.split('|')[0];
            dict[key + 'MINUTES'] = parseInt(value.split('|')[1].split(':')[1], 10);
            value = parseInt(newValue, 10);
        }
        if (key === 'KEY_FONTTYPE' || key === 'KEY_DATEFORMAT' || key === 'KEY_LOCALE' ||
                key === 'KEY_TEXTALIGN' || key === 'KEY_WEATHERPROVIDER' || key.indexOf('SLOT') !== -1 ||
                key === 'KEY_SPEEDUNIT') {
            value = parseInt(value, 10);
        }
        dict[key] = value;
    }

    localStorage.weatherEnabled = dict.KEY_ENABLEWEATHER;
    localStorage.useCelsius = dict.KEY_USECELSIUS;
    localStorage.weatherKey = dict.KEY_WEATHERKEY;
    localStorage.overrideLocation = dict.KEY_OVERRIDELOCATION;
    localStorage.weatherProvider = dict.KEY_WEATHERPROVIDER;
    localStorage.forecastKey = dict.KEY_FORECASTKEY;

    delete dict.KEY_WEATHERKEY;
    delete dict.KEY_WEATHERPROVIDER;
    delete dict.KEY_OVERRIDELOCATION;
    delete dict.KEY_FORECASTKEY;
    
    Pebble.sendAppMessage(dict, function() {
	console.log('Send config successful: ' + JSON.stringify(dict));
    }, function() {
	console.log('Send failed!');
    });
});

function parse(type) {
    return typeof type == 'string' ? JSON.parse(type) : type;
}

function locationSuccess(pos, provider, weatherKey, useCelsius, overrideLocation) {
    console.log("Retrieving weather info");

    switch (provider) {
        case OPEN_WEATHER:
            fetchOpenWeatherMapData(pos, useCelsius, overrideLocation);
            break;
        case WUNDERGROUND:
            fetchWeatherUndergroundData(pos, weatherKey, useCelsius, overrideLocation);
            break;
        case YAHOO:
            fetchYahooData(pos, useCelsius, overrideLocation);
            break;
        case FORECAST:
            fetchForecastApiData(pos, weatherKey, useCelsius, overrideLocation);
    }
}

function executeYahooQuery(pos, useCelsius, woeid, overrideLocation) {
    var url = 'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=';
    var woeidQuery = '';
    if (overrideLocation) {
        woeidQuery = 'select woeid from geo.places(1) where text="' + overrideLocation + '"';
    } else {
        woeidQuery = '' + woeid;
    }

    if (woeidQuery) {
        var query = 'select * from weather.forecast where woeid in (' + woeidQuery + ')';
        url += encodeURIComponent(query);
        console.log(url);

        xhrRequest(url, 'GET', function(responseText) {
            try {
                var resp = JSON.parse(responseText);
                var now = new Date();
                var day = now.getDate();
                var dayUTC = now.getUTCDate();

                var resultIndex = 0;
                var res = resp.query.results.channel;
                var results = res.item;
                var wind = res.wind;
                console.log(JSON.stringify(results.forecast[resultIndex]));
                var temp = Math.round(useCelsius ? fahrenheitToCelsius(results.condition.temp) : results.condition.temp);
                var min = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast[resultIndex].low) : results.forecast[resultIndex].low);
                var max = Math.round(useCelsius ? fahrenheitToCelsius(results.forecast[resultIndex].high) : results.forecast[resultIndex].high);
                var condition = y_iconToId[parseInt(results.condition.code, 10)];
                console.log(results.condition.code);
                var feels = Math.round(useCelsius ? fahrenheitToCelsius(parseInt(wind.chill, 10)) : wind.chill);
                var speed = Math.round(wind.speed);
                var direction = Math.round(wind.direction);

                if (typeof(condition) === 'undefined') {
                    condition = 0;
                }

                sendData(temp, max, min, condition, feels, speed, direction);
            } catch (ex) {
                console.log(ex);
                console.log('Yahoo weather failed, falling back to open weather');
                fetchOpenWeatherMapData(pos, useCelsius, overrideLocation);
            }
        });
    } else {
        console.log('No woeid found, falling back to open weather');
        fetchOpenWeatherMapData(pos, useCelsius, overrideLocation);
    }
}

function fetchYahooData(pos, useCelsius, overrideLocation) {
    if (!overrideLocation) {
        getWoeidAndExecuteQuery(pos, useCelsius);
    } else {
        executeYahooQuery(pos, useCelsius, '', overrideLocation);
    }

}

function fahrenheitToCelsius(temp) {
    return (temp - 32)/1.8;
}

function getWoeidAndExecuteQuery(pos, useCelsius) {
    var truncLat = pos.coords.latitude.toFixed(4);
    var truncLng = pos.coords.longitude.toFixed(4);
    var latLng = truncLat + ',' + truncLng;

    if (localStorage[latLng]) {
        console.log('Got woeid from storage. ' + latLng + ': ' + localStorage[latLng]);
        executeYahooQuery(pos, useCelsius, localStorage[latLng]);
        return;
    }

    var url = 'http://gws2.maps.yahoo.com/findlocation?pf=1&locale=en_US&offset=15&flags=J&q=' + truncLat + '%2c' + truncLng + '&gflags=R&start=0&count=100';

    console.log(url);
    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            if (resp.ResultSet.Error === 0) {
                var woeid = resp.ResultSet.Results[0].woeid;
                console.log('Got woeid from API. ' + latLng + ': ' + woeid);
                localStorage[latLng] = woeid;
                executeYahooQuery(pos, useCelsius, woeid);
            } else {
                console.log('woeid query failed: ' + resp.ResultSet.Error);
                executeYahooQuery(pos, useCelsius, '');
            }
        } catch (ex) {
            console.log(ex.stack);
            console.log('woeid query failed');
            executeYahooQuery(pos, useCelsius, '');
        }
    });
}

function fetchWeatherUndergroundData(pos, weatherKey, useCelsius, overrideLocation) {
    var url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/q/';
    if (!overrideLocation) {
        url += pos.coords.latitude + ',' + pos.coords.longitude + '.json';
    } else {
        url += encodeURIComponent(overrideLocation) + '.json';
    }

    console.log(url);

    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var results = resp.current_observation;
            var temp = Math.round((useCelsius ? results.temp_c : results.temp_f));
            var highTemp = resp.forecast.simpleforecast.forecastday[0].high;
            var lowTemp = resp.forecast.simpleforecast.forecastday[0].low;
            var max = Math.round((useCelsius ? highTemp.celsius : highTemp.fahrenheit));
            var min = Math.round((useCelsius ? lowTemp.celsius : lowTemp.fahrenheit));
            var icon = results.icon_url.match(/\/([^.\/]*)\.gif/)[1];
            var condition = wu_iconToId[icon];
            var feels = Math.round((useCelsius ? results.feelslike_c : results.feelslike_f));
            var speed = Math.round(results.wind_mph);
            var direction = results.wind_degrees;

            if (typeof(condition) === 'undefined') {
                condition = 0;
            }

            sendData(temp, max, min, condition, feels, speed, direction);

        } catch(ex) {
            console.log(ex.stack);
            console.log('Falling back to Yahoo');
            fetchYahooData(pos, useCelsius, overrideLocation);
        }
    });
}

function fetchForecastApiData(pos, weatherKey, useCelsius, overrideLocation) {
    if (overrideLocation) {
        findLocationAndExecuteQuery(weatherKey, useCelsius, overrideLocation)
    } else {
        executeForecastQuery(pos, weatherKey, useCelsius);
    }
}

function findLocationAndExecuteQuery(weatherKey, useCelsius, overrideLocation) {
    if (localStorage[overrideLocation]) {
        console.log('Got coords for ' + overrideLocation + ' from storage: ' + localStorage[overrideLocation]);
        executeForecastQuery(JSON.parse(localStorage[overrideLocation]), weatherKey, useCelsius, overrideLocation);
        return;
    }

    var url = 'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=';
    var query = 'select centroid from geo.places(1) where text="' + overrideLocation + '"';
    url += encodeURIComponent(query);
    console.log('Retrieving location data for Forecast.io');
    console.log(url);
    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var res = resp.query.results.place.centroid;
            var pos = { coords: {
                latitude: parseFloat(res.latitude),
                longitude: parseFloat(res.longitude),
            }};

            localStorage[overrideLocation] = JSON.stringify(pos);

            executeForecastQuery(pos, weatherKey, useCelsius, overrideLocation);
        } catch (ex) {
            console.log(ex.stack);
            console.log('Falling back to Yahoo');
            fetchYahooData(pos, useCelsius, overrideLocation);
        }
    });


}

function executeForecastQuery(pos, weatherKey, useCelsius, overrideLocation) {
    console.log(JSON.stringify(pos));
    var truncLat = pos.coords.latitude.toFixed(4);
    var truncLng = pos.coords.longitude.toFixed(4);
    var url = 'https://api.forecast.io/forecast/' + weatherKey + '/' + truncLat  + ',' + truncLng;

    console.log('Retrieving weather data from Forecast.io');
    console.log(url);
    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var temp = Math.round((useCelsius ? fahrenheitToCelsius(resp.currently.temperature) : resp.currently.temperature));
            var max = Math.round((useCelsius ? fahrenheitToCelsius(resp.daily.data[0].temperatureMax) : resp.daily.data[0].temperatureMax));
            var min = Math.round((useCelsius ? fahrenheitToCelsius(resp.daily.data[0].temperatureMin) : resp.daily.data[0].temperatureMin));
            var icon = resp.currently.icon;
            var condition = f_iconToId[icon];
            var feels = Math.round((useCelsius ? fahrenheitToCelsius(resp.currently.apparentTemperature) : resp.currently.apparentTemperature));
            var speed = Math.round(resp.currently.windSpeed);
            var direction = Math.round(resp.currently.windBearing);

            if (typeof(condition) === 'undefined') {
                condition = 0;
            }

            sendData(temp, max, min, condition, feels, speed, direction);
        } catch (ex) {
            console.log(ex.stack);
            console.log('Falling back to Yahoo');
            fetchYahooData(pos, useCelsius, overrideLocation);
        }

    });
}

function fetchOpenWeatherMapData(pos, useCelsius, overrideLocation) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3';
    var urlForecast = 'http://api.openweathermap.org/data/2.5/forecast/daily?appid=979cbf006bf67bc368a54af240d15cf3&format=json&cnt=3';

    if (!overrideLocation) {
        url += '&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
        urlForecast += '&lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
    } else {
        url += '&q=' + encodeURIComponent(overrideLocation);
        urlForecast += '&q=' + encodeURIComponent(overrideLocation);
    }

    console.log(url);
    console.log(urlForecast);

    xhrRequest(url, 'GET', function(responseText) {
        try {
            console.log('Retrieving current weather from OpenWeatherMap');
            var resp = JSON.parse(responseText);
            var temp = useCelsius ? kelvinToCelsius(resp.main.temp) : kelvinToFahrenheit(resp.main.temp);
            var condition = ow_iconToId[resp.weather[0].icon];
            var feels = temp;
            var speed = Math.round(resp.wind.speed * 2.23694);
            var direction = resp.wind.deg;
            var day = new Date(resp.dt * 1000);

            if (typeof(condition) === 'undefined') {
                condition = 0;
            }

            xhrRequest(urlForecast, 'GET', function(forecastRespText) {
                try {
                    console.log('Retrieving forecast data from OpenWeatherMap');
                    var fResp = JSON.parse(forecastRespText);

                    var max = useCelsius ? kelvinToCelsius(fResp.list[0].temp.max) : kelvinToFahrenheit(fResp.list[0].temp.max);
                    var min = useCelsius ? kelvinToCelsius(fResp.list[0].temp.min) : kelvinToFahrenheit(fResp.list[0].temp.min);

                    for (var fIndex in fResp.list) {
                        var fDay = new Date(fResp.list[fIndex].dt * 1000);
                        if (day.getUTCDate() === fDay.getUTCDate()) {
                            console.log(JSON.stringify(fResp.list[fIndex]));
                            max = useCelsius ? kelvinToCelsius(fResp.list[fIndex].temp.max) : kelvinToFahrenheit(fResp.list[fIndex].temp.max);
                            min = useCelsius ? kelvinToCelsius(fResp.list[fIndex].temp.min) : kelvinToFahrenheit(fResp.list[fIndex].temp.min);
                        }
                    }

                    sendData(temp, max, min, condition, feels, speed, direction);
                } catch (ex) {
                    console.log('Failure requesting forecast data from OpenWeatherMap');
                    console.log(ex.stack);
                }
            });

        } catch (ex) {
            console.log('Failure requesting current weather from OpenWeatherMap');
            console.log(ex.stack);
        }
    });
}

function checkForUpdates() {
    var url = 'http://www.lbento.space/pebble-apps/timeboxed/version.json';
    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var updateAvailable = resp.version !== currentVersion;
            console.log('Current version: ' + currentVersion + ', Latest version: ' + resp.version);
            sendUpdateData(updateAvailable);
        } catch (ex) {
            console.log(ex);
            sendUpdateData(false);
        }
    });
}

function sendUpdateData(updateAvailable) {
    console.log(updateAvailable ? 'Update available!' : 'No updates.');
    Pebble.sendAppMessage({'KEY_HASUPDATE': updateAvailable},
        function(e) {
            console.log('Sent update data to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending update data to Pebble!');
        }
    );
}

function kelvinToCelsius(temp) {
    return Math.round(temp - 273.15);
}

function kelvinToFahrenheit(temp) {
    return Math.round(temp * 1.8 - 459.67);
}

function sendData(temp, max, min, condition, feels, speed, direction) {
    var data = {
        'KEY_TEMP': temp,
        'KEY_MAX': max,
        'KEY_MIN': min,
        'KEY_WEATHER': condition,
        'KEY_FEELS': feels,
        'KEY_SPEED': speed,
        'KEY_DIRECTION': direction,
    };

    console.log(JSON.stringify(data));

    Pebble.sendAppMessage(data,
        function(e) {
            console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending weather info to Pebble!');
        }
    );
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getWeather(provider, weatherKey, useCelsius, overrideLocation) {
    console.log('Requesting weather: ' + provider + ', ' + weatherKey + ', ' + useCelsius + ', ' + overrideLocation);
    provider = provider || 0;
    weatherKey = weatherKey || '';
    useCelsius = useCelsius || false;
    overrideLocation = overrideLocation || '';
    if (overrideLocation) {
        locationSuccess(null, provider, weatherKey, useCelsius, overrideLocation);
    } else {
        navigator.geolocation.getCurrentPosition(
            function(pos) {
                locationSuccess(pos, provider, weatherKey, useCelsius, overrideLocation);
            },
            locationError,
            {timeout: 15000, maximumAge: 60000}
        );
    }
}


var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };

    try {
        xhr.open(type, url);
        xhr.send();
    } catch (ex) {
        console.log(ex);
        sendError();
    }
};

var sendError = function() {
    Pebble.sendAppMessage({'KEY_ERROR': true},
        function(e) {
            console.log('Sent empty state to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending empty state to Pebble!');
        }
    );
};

var wu_iconToId = {
    'unknown': 0,
    'clear': 1,
    'sunny': 2,
    'partlycloudy': 3,
    'mostlycloudy': 4,
    'mostlysunny': 5,
    'partlysunny': 6,
    'cloudy': 7,
    'rain': 8,
    'snow': 9,
    'tstorms': 10,
    'sleat': 11,
    'flurries': 12,
    'hazy': 13,
    'chancetstorms': 14,
    'chancesnow': 15,
    'chancesleat': 16,
    'chancerain': 17,
    'chanceflurries': 18,
    'nt_unknown': 19,
    'nt_clear': 20,
    'nt_sunny': 21,
    'nt_partlycloudy': 22,
    'nt_mostlycloudy': 23,
    'nt_mostlysunny': 24,
    'nt_partlysunny': 25,
    'nt_cloudy': 26,
    'nt_rain': 27,
    'nt_snow': 28,
    'nt_tstorms': 29,
    'nt_sleat': 30,
    'nt_flurries': 31,
    'nt_hazy': 32,
    'nt_chancetstorms': 33,
    'nt_chancesnow': 34,
    'nt_chancesleat': 35,
    'nt_chancerain': 36,
    'nt_chanceflurries': 37,
    'fog': 38,
    'nt_fog': 39
};

var f_iconToId = {
    'clear-day': 1,
    'clear-night': 20,
    'rain': 8,
    'snow': 9,
    'sleet': 11,
    'wind': 44,
    'fog': 38,
    'cloudy': 7,
    'partly-cloudy-day': 6,
    'partly-cloudy-night': 25
};

var ow_iconToId = {
    '01d': 1,
    '02d': 3,
    '03d': 7,
    '04d': 7,
    '09d': 8,
    '10d': 8,
    '13d': 9,
    '11d': 10,
    '50d': 13,
    '01n': 20,
    '02n': 22,
    '03n': 26,
    '04n': 26,
    '09n': 27,
    '10n': 27,
    '13n': 28,
    '11n': 29,
    '50n': 32,
};

var y_iconToId = {
    '3200': 0,
    '32': 1,
    '34': 1,
    '30': 5,
    '28': 4,
    '26': 7,
    '44': 7,
    '10': 8,
    '11': 8,
    '12': 8,
    '40': 8,
    '13': 9,
    '14': 9,
    '15': 9,
    '16': 9,
    '41': 9,
    '42': 9,
    '43': 9,
    '46': 9,
    '1': 10,
    '3': 10,
    '4': 10,
    '37': 10,
    '38': 10,
    '39': 10,
    '45': 10,
    '47': 10,
    '5': 11,
    '6': 11,
    '7': 11,
    '8': 40,
    '9': 40,
    '18': 11,
    '17': 41,
    '35': 41,
    '21': 13,
    '19': 13,
    '31': 20,
    '33': 20,
    '27': 23,
    '29': 24,
    '20': 38,
    '25': 42,
    '36': 43,
    '23': 44,
    '24': 44,
    '0': 45,
    '2': 46
};
