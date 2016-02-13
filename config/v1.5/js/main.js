'use strict';

(function($) {
    $('#enableHealth').change(function() {
        $('#healthConfigs').toggleClass('hidden');
    });
    $('#enableWeather').change(function() {
        $('#weatherConfigs').toggleClass('hidden');
    });
    $('#enableAdvanced').change(function() {
        $('#advancedConfigs').toggleClass('hidden');
    });

    var parse = function (type) {
        return typeof type == 'string' ? JSON.parse(type) : type;
    };

    var getCurrentVersion = function() {
        var urlParamList = window.location.search.substring(1);
        var urlParams = urlParamList.split('&');
        for (var item in urlParams) {
            var elem = urlParams[item];
            var keyValue = elem.split('=');
            if (keyValue[0] === 'v') {
                return keyValue[1];
            }
        }
        return '';
    };

    var checkForUpdates = function() {
        var url = 'http://www.lbento.space/pebble-apps/timeboxed/version.json';
        $.ajax({
            type: 'GET',
            url: url,
            success: function(data, status, xhr) {
                if (status === 'success') {
                    try {
                        var resp = JSON.parse(data);
                        var latestVersion = resp.version;
                        var currentVersion = getCurrentVersion();
                        notifyUpdate(
                            latestVersion && currentVersion && latestVersion !== currentVersion,
                            currentVersion,
                            latestVersion);
                    } catch(ex) {
                        console.log(ex);
                        notifyUpdate(false)
                    }
                } else {
                    console.log('Ajax fail. Status: ' + status);
                    notifyUpdate(false);
                }
            },
            error: function() {
                console.log('Ajax error.');
                notifyUpdate(false);
            }
        })
    };

    var notifyUpdate = function(hasUpdate, currentVersion, latestVersion) {
        console.log('Notifying update: ' + hasUpdate);
        if (currentVersion) {
            $('#versionText').text('Current Version: ' + currentVersion);
            $('#versionContainer').removeClass('hidden');
        }
        if (hasUpdate) {
            $('#updateText').text('> New version ' + latestVersion + ' available! <').removeClass('hidden');
        }
    };

    $('#verifyLocation').click(function(e) {
        var weatherKey = $('#weatherKey').val();
        var overrideLocation = $('#overrideLocation').val();
        if (overrideLocation) {
            var url = 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3&q=' + encodeURIComponent(overrideLocation);
            if (weatherKey) {
                url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/q/' + overrideLocation + '.json';
            }
            console.log(url);
            $.ajax({
                type: "GET",
                url: url,
                success: function(data, status, xhr) {
                    if (status === 'success') {
                        try {
                            if (weatherKey) {
                                if (data.response.error) {
                                    if (data.response.error.type === 'keynotfound') {
                                        alert('Invalid WeatherUnderground Key');
                                    } else {
                                        alert('Invalid location');
                                    }
                                } else {
                                    alert('Valid location!');
                                }
                            } else {
                                if (parseInt(data.cod, 10) === 404) {
                                    alert('Invalid location');
                                } else {
                                    alert('Valid location!');
                                }
                            }
                        } catch (e) {
                            alert('Can\'t verify the location now. Please try again later.');
                        }

                    } else {
                        alert('Can\'t verify the location now. Please try again later.');
                    }
                },
                error: function() {
                    if (weatherKey) {
                        alert('There was a problem with the request. Please verify your API key.');
                    } else {
                        alert('Can\'t verify the location now. Please try again later.');
                    }
                }
            });
        }
    });

    var loadData = function() {
        for (var item in localStorage) {
            var itemValue = localStorage[item];
            var element = $('#' + item)[0];
            if (element) {
                if (item.indexOf('Color') !== -1 || item === 'weatherKey' || item === 'timezones' || item == 'overrideLocation') {
                    element.value = itemValue;
                } else if (item === 'fontType') {
                    var elements = $(".font-type");
                    for (var i = 0; i < elements.length; ++i) {
                        elements[i].checked = elements[i].value === itemValue ? "checked" : "";
                    }
                } else {
                    element.checked = parse(itemValue);
                    if (item.indexOf('enable') !== -1 && element.checked) {
                        var containerId = '#' + item.match(/enable(.*)/)[1].toLowerCase() + 'Configs';
                        $(containerId).toggleClass('hidden');
                    }
                }
            }
        }
    };

    var submitBtn = $('#submit');
    submitBtn.on('click', function(e) {
        var returnTo = getQueryParam('return_to', 'pebblejs://close#');
        document.location = returnTo + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
    });

    var getQueryParam = function(variable, defaultValue) {
	var query = location.search.substring(1);
	var vars = query.split('&');
	for (var i = 0; i < vars.length; i++) {
	    var pair = vars[i].split('=');
	    if (pair[0] === variable) {
		return decodeURIComponent(pair[1]);
	    }
	}
	return defaultValue || false;
    };

    var getAndStoreConfigData = function() {
        var data = {
            enableHealth: $('#enableHealth')[0].checked,
            useKm: $('#useKm')[0].checked,
            showSleep: $('#showSleep')[0].checked,
            enableWeather: $('#enableWeather')[0].checked,
            weatherKey: $('#weatherKey').val(),
            useCelsius: $('#useCelsius')[0].checked,
            timezones: $('#timezones')[0].selectedOptions[0].value,
            bgColor: $('#bgColor').val(),
            hoursColor: $('#hoursColor').val(),
            enableAdvanced: $('#enableAdvanced')[0].checked,
            dateColor: $('#dateColor').val(),
            altHoursColor: $('#altHoursColor').val(),
            batteryColor: $('#batteryColor').val(),
            batteryLowColor: $('#batteryLowColor').val(),
            weatherColor: $('#weatherColor').val(),
            tempColor: $('#tempColor').val(),
            minColor: $('#minColor').val(),
            maxColor: $('#maxColor').val(),
            stepsColor: $('#stepsColor').val(),
            distColor: $('#distColor').val(),
            fontType: $('.font-type[name="font-selection"]:checked').val(),
            bluetoothDisconnect: $('#bluetoothDisconnect')[0].checked,
            bluetoothColor: $('#bluetoothColor').val(),
            overrideLocation: $('#overrideLocation').val(),
            updateColor: $('#updateColor').val(),
            update: $('#update')[0].checked
        };

        for (var item in data) {
            localStorage[item] = data[item];
        }

        console.log(JSON.stringify(data));
        return data;

    };

    loadData();
    checkForUpdates();

}(Zepto));
