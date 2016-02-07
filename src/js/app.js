Pebble.addEventListener("ready",
    function(e) {
        console.log("Pebble Ready!");
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');
        console.log(JSON.stringify(e.payload));
        getWeather(e.payload.KEY_WEATHERKEY, e.payload.KEY_USECELSIUS);
    }                     
);

Pebble.addEventListener('showConfiguration', function(e) {
    Pebble.openURL('http://a4defdab.ngrok.io');
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configData = JSON.parse(decodeURIComponent(e.response));
    console.log(JSON.stringify(configData));

    var dict = {};

    for (item in configData) {
        var key = 'KEY_' + item.toUpperCase();
        var value = configData[item];
        if (String(value).indexOf('0x') !== -1) {
            value = parseInt(value, 16);
        }
        if (String(value).indexOf('|') !== -1) {
            newValue = value.split('|')[1].split(':')[0];
            dict[key + 'CODE'] = value.split('|')[0];
            dict[key + 'MINUTES'] = parseInt(value.split('|')[1].split(':')[1], 10);
            value = parseInt(newValue, 10);
        }
        dict[key] = value;
    }

    Pebble.sendAppMessage(dict, function() {
	console.log('Send config successful: ' + JSON.stringify(dict));
    }, function() {
	console.log('Send failed!');
    }); 
});

function locationSuccess(pos, weatherKey, useCelsius) {
    console.log("Retrieving weather info");

    var url = 'http://api.wunderground.com/api/7456558e28f97728/conditions/forecast/q/' + pos.coords.latitude + ',' + pos.coords.longitude + '.json';
    console.log(url);
    xhrRequest(url, 'GET', function(responseText) {
        try {
            var resp = JSON.parse(responseText);
            var temp = Math.round(useCelsius ? resp.current_observation.temp_c : resp.current_observation.temp_f);
            var highTemp = resp.forecast.simpleforecast.forecastday[0].high;
            var lowTemp = resp.forecast.simpleforecast.forecastday[0].low;
            var max = Math.round(useCelsius ? highTemp.celsius : highTemp.fahrenheit);
            var min = Math.round(useCelsius ? lowTemp.celsius : lowTemp.fahrenheit);
            var icon = resp.current_observation.icon_url.match(/\/([^.\/]*)\.gif/)[1];
            var condition = wu_iconToId[icon];
            if (typeof(condition) === 'undefined') {
                condition = 0;
            }

            var data = {
                'KEY_TEMP': temp,
                'KEY_MAX': max,
                'KEY_MIN': min,
                'KEY_WEATHER': condition
            }

            Pebble.sendAppMessage(data,
                function(e) {
                    console.log('Weather info sent to Pebble successfully!');
                },
                function(e) {
                    console.log('Error sending weather info to Pebble!');
                }
            );

        } catch(ex) {
            console.log(ex);
        }
    });
}

function locationError(err) {
    console.log('Error requesting location!');
}

function getWeather(weatherKey, useCelsius) {
    if (weatherKey) {
        navigator.geolocation.getCurrentPosition(
            function(pos) {
                locationSuccess(pos, weatherKey, useCelsius);   
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
    xhr.open(type, url);
    xhr.send();
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
};
