(function($) {
    'use strict';

    var loadedData = false;

    var trackEvent = function(category, action, label, value) {
        if (loadedData) {
            ga('send', 'event', category, action, label, value);
        }
    };

    $('#enableHealth').change(function() {
        $('#healthConfigs').toggleClass('hidden');
    });

    $('#enableWeather').change(function() {
        $('#weatherConfigs').toggleClass('hidden');
    });

    $('#enableAdvanced').change(function() {
        $('#advancedConfigs').toggleClass('hidden');
    });

    $('.item-color').click(function(e) {
        trackEvent('color', e.currentTarget.id, 'click');
    });
    $('.item-color').change(function(e) {
        trackEvent('color', e.currentTarget.id, e.currentTarget.value);
    });

    $('.item-select').click(function(e) {
        trackEvent(e.currentTarget.id, 'click');
    });
    $('.item-select').change(function(e) {
        trackEvent(e.currentTarget.id, 'select', e.currentTarget.selectedOptions[0].value);
    });

    $('.item-toggle').change(function(e) {
        trackEvent(e.currentTarget.id, 'click', e.currentTarget.checked);
    });

    $('.item-input').change(function(e) {
        trackEvent(e.currentTarget.id, e.currentTarget.value ? 'used' : 'not used');
    });

    var parse = function (type) {
        var value;
        try {
            value = typeof(type) === 'string' ? JSON.parse(type) : type;
        } catch (ex) {
            console.log(ex);
        }
        return value;
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
            dataType: 'text',
            success: function(data, status, xhr) {
                if (status === 'success') {
                    try {
                        var resp = JSON.parse(data);
                        var latestVersion = resp.version;
                        var currentVersion = getCurrentVersion();
                        var hasUpdate = latestVersion && currentVersion && latestVersion !== currentVersion;
                        notifyUpdate(
                            hasUpdate,
                            currentVersion,
                            latestVersion);
                        trackEvent('updates', 'success', getCurrentVersion(), hasUpdate);
                    } catch(ex) {
                        console.log(ex);
                        notifyUpdate(false);
                        trackEvent('updates', 'error', getCurrentVersion());
                    }
                } else {
                    console.log('Ajax fail. Status: ' + status);
                    notifyUpdate(false);
                    trackEvent('updates', 'ajax fail', getCurrentVersion(), status);
                }
            },
            error: function() {
                console.log('Ajax error.');
                notifyUpdate(false);
                trackEvent('updates', 'ajax error', getCurrentVersion());
            }
        });
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
        trackEvent('verify location', 'click');
        var weatherKey = $('#weatherKey').val();
        var overrideLocation = $('#overrideLocation').val();
        if (overrideLocation) {
            var url = 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3&q=' + encodeURIComponent(overrideLocation);
            if (weatherKey) {
                url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/q/' + overrideLocation + '.json';
            }
            trackEvent('verify location', weatherKey ? 'wu' : 'owm');
            console.log(url);
            $.ajax({
                type: "GET",
                url: url,
                success: function(data, status, xhr) {
                    if (status === 'success') {
                        try {
                            if (weatherKey) {
                                if (data.response.error || ! data.current_observation) {
                                    if (data.response.error && data.response.error.type === 'keynotfound') {
                                        alert('Invalid WeatherUnderground Key');
                                        trackEvent('verify location', 'wu', 'api error');
                                    } else {
                                        alert('Invalid location');
                                        trackEvent('verify location', 'wu', 'invalid location');
                                    }
                                } else {
                                    alert('Valid location!');
                                    trackEvent('verify location', 'wu', 'valid location');
                                }
                            } else {
                                if (parseInt(data.cod, 10) === 404) {
                                    alert('Invalid location');
                                    trackEvent('verify location', 'owm', 'invalid location');
                                } else {
                                    alert('Valid location!');
                                    trackEvent('verify location', 'owm', 'valid location');
                                }
                            }
                        } catch (e) {
                            alert('Can\'t verify the location now. Please try again later.');
                            trackEvent('verify location', weatherKey ? 'wu' : 'owm', 'exception');
                        }

                    } else {
                        alert('Can\'t verify the location now. Please try again later.');
                        trackEvent('verify location', weatherKey ? 'wu' : 'owm', 'error', status);
                    }
                },
                error: function() {
                    if (weatherKey) {
                        alert('There was a problem with the request. Please verify your API key.');
                    } else {
                        alert('Can\'t verify the location now. Please try again later.');
                    }
                    trackEvent('verify location', weatherKey ? 'wu' : 'owm', 'error');
                }
            });
        } else {
            trackEvent('verify location', 'location empty');
        }
    });

    var loadData = function() {
        var hasData = false;
        try {
            for (var item in localStorage) {
                hasData = true;
                var itemValue = localStorage[item];
                var element = $('#' + item)[0];
                if (element) {
                    if (item.indexOf('Color') !== -1 || item === 'weatherKey' || item === 'timezones' ||
                        item == 'overrideLocation' || item === 'fontType' || item === 'dateFormat' || item === 'locale') {
                        element.value = itemValue;
                    } else {
                        element.checked = parse(itemValue) || false;
                        if (item.indexOf('enable') !== -1 && element.checked) {
                            var containerId = '#' + item.match(/enable(.*)/)[1].toLowerCase() + 'Configs';
                            $(containerId).toggleClass('hidden');
                        }
                    }
                }
            }
            loadedData = true;
            trackEvent('data', 'loaded', hasData);
        } catch (ex) {
            console.log(ex);
            loadedData = true;
            trackEvent('data', 'loaded', 'error');
        }

    };

    var submitBtn = $('#submit');
    submitBtn.on('click', function(e) {
        trackEvent('data', 'saved');
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
            fontType: $('#fontType')[0].selectedOptions[0].value,
            bluetoothDisconnect: $('#bluetoothDisconnect')[0].checked,
            bluetoothColor: $('#bluetoothColor').val(),
            overrideLocation: $('#overrideLocation').val(),
            updateColor: $('#updateColor').val(),
            update: $('#update')[0].checked,
            locale: $('#locale')[0].selectedOptions[0].value,
            dateFormat: $('#dateFormat')[0].selectedOptions[0].value
        };

        for (var item in data) {
            localStorage[item] = data[item];
        }

        console.log(JSON.stringify(data));
        return data;

    };

    $(function() {
        loadData();
        checkForUpdates();
    });

}(Zepto));
