(function($) {
    'use strict';

    var trackEvent = function(category, action, label, value) {
        ga('send', 'event', category, action, label, value);
    };

    var weatherProviders = [
        'openweather',
        'weatherunderground',
        'yahoo',
    ];

    var initEvents = function() {
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
            trackEvent(e.currentTarget.id, 'select', e.currentTarget.options[e.currentTarget.selectedIndex].value);
        });

        $('.item-toggle').change(function(e) {
            trackEvent(e.currentTarget.id, 'click', e.currentTarget.checked);
        });

        $('.item-input').change(function(e) {
            trackEvent(e.currentTarget.id, e.currentTarget.value ? 'used' : 'not used');
        });

        $('.tab-align').click(function(e) {
            trackEvent('align', e.currentTarget.getAttribute('data-value'));
        });

        $('.tab-weather').click(function(e) {
            var value = parseInt(e.currentTarget.getAttribute('data-value'), 10);
            trackEvent('weather provider', 'select', value);
            toggleWeatherKeys(value);
        });

        $('#donateBtn').click(function(e) {
            trackEvent('donate', 'click');
        });

        $('#presetList').on('itemRemoved', function(e, item) {
            trackEvent('presets', 'remove');
            if (localStorage['preset-' + item]) {
                delete localStorage['preset-' + item];
            }
        });
        $('#presetList').on('itemAdded', function(e, item) {
            trackEvent('presets', 'add');
            var colors = getColorValues();
            if (localStorage['preset-' + item]) {
                trackEvent('presets', 'already exists');
                alert('Preset with that name already exists. Replacing.');
                var elems = $('#presetList .item');

                $(elems[elems.length - 2]).remove();
            }
            localStorage['preset-' + item] = JSON.stringify(colors);
        });
        $('#presetList').on('click', '.item', function(e) {
            if (!$(e.currentTarget).hasClass('add-item')) {
                trackEvent('presets', 'load', 'click');
                var text = $(e.currentTarget).text();
                var storageName = $(e.currentTarget).attr('data-preset-name');
                if (confirm('Load the preset "' + text + '" ?')) {
                    trackEvent('presets', 'load', 'execute');
                    var colors = JSON.parse(localStorage[storageName]);
                    console.log(localStorage[storageName]);
                    loadColors(colors);
                } else {
                    trackEvent('presets', 'load', 'dismiss');
                }
            }
        });
    };

    var loadColors = function(colors) {
        for (var item in colors) {
            var elem = $('#' + item);
            var value = colors[item];
            elem.val(value);
            elem.parent().find('.item-styled-color .value').css('background-color', value.replace(/^0x/, '#'));
        }
    };

    var toggleWeatherKeys = function(value) {
        switch(value) {
            case 0:
                $('#weatherUndergroundConfig').addClass('hidden');
                $('#yahooConfig').addClass('hidden');
                break;
            case 1:
                $('#weatherUndergroundConfig').removeClass('hidden');
                $('#yahooConfig').addClass('hidden');
                break;
            case 2:
                $('#weatherUndergroundConfig').addClass('hidden');
                $('#yahooConfig').addClass('hidden');
                break;
            default:
                break;
        }
    };

    var loadPresets = function() {
        localStorage['default-preset-Colorful (default)'] = '{"bgColor":"0x000055","hoursColor":"0xFFFFFF","dateColor":"0xAAFFFF","altHoursColor":"0x00FFFF","batteryColor":"0xAAAAAA","batteryLowColor":"0xFF5500","weatherColor":"0xFFFF00","tempColor":"0xFFFF00","minColor":"0x00FFFF","maxColor":"0xFF5500","stepsColor":"0x55FFFF","distColor":"0x55FFFF","stepsBehindColor":"0xFFFF00","distBehindColor":"0xFFFF00","updateColor":"0x00FF00","bluetoothColor":"0xFF5500"}';
        localStorage['default-preset-Black and white (default)'] = '{"bgColor":"0x000000","hoursColor":"0xFFFFFF","dateColor":"0xFFFFFF","altHoursColor":"0xFFFFFF","batteryColor":"0xFFFFFF","batteryLowColor":"0xFFFFFF","weatherColor":"0xFFFFFF","tempColor":"0xFFFFFF","minColor":"0xFFFFFF","maxColor":"0xFFFFFF","stepsColor":"0xFFFFFF","distColor":"0xFFFFFF","stepsBehindColor":"0xFFFFFF","distBehindColor":"0xFFFFFF","updateColor":"0xFFFFFF","bluetoothColor":"0xFFFFFF"}';
        for (var item in localStorage) {
            if (item.indexOf('preset-') !== -1) {
                $('#presetList').append('<label class="item' + (item.indexOf('default-preset') !== -1 ? ' default-preset' : '') + '" data-preset-name="' + item + '">' + item.substring(item.indexOf('default-') !== -1 ? 15 : 7) + '</label>');
            }
        }
    };

    var initGA = function() {
        ga('set', 'dimension1', getPlatform());
        ga('set', 'dimension2', localStorage.locale || 0);
        ga('set', 'dimension3', localStorage.fontType || 0);
        ga('set', 'dimension4', getCurrentVersion());
        ga('set', 'dimension5', getUserLanguage());

        ga('send', 'pageview');
    };

    var toggleFieldsByPlatform = function() {
        var platform = getPlatform();

        if (platform === 'aplite') {
            $('#healthContainer').addClass('hidden');
        } else if (platform === 'chalk') {
            $('#alignContainer').addClass('hidden');
        }
    };

    var parse = function (type) {
        var value;
        try {
            value = typeof(type) === 'string' ? JSON.parse(type) : type;
        } catch (ex) {
            console.log(ex);
        }
        return value;
    };

    var getUrlParam = function(param) {
        var urlParamList = window.location.search.substring(1);
        var urlParams = urlParamList.split('&');
        for (var item in urlParams) {
            var elem = urlParams[item];
            var keyValue = elem.split('=');
            if (keyValue[0] === param) {
                return keyValue[1];
            }
        }
        return '';
    };

    var getCurrentVersion = function() {
        return getUrlParam('v');
    };

    var getPlatform = function() {
        return getUrlParam('p');
    };

    var getUserLanguage = function() {
        return getUrlParam('l');
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
                        trackEvent('updates', 'success', hasUpdate);
                    } catch(ex) {
                        console.log(ex);
                        notifyUpdate(false);
                        trackEvent('updates', 'error');
                    }
                } else {
                    console.log('Ajax fail. Status: ' + status);
                    notifyUpdate(false);
                    trackEvent('updates', 'ajax fail', status);
                }
            },
            error: function() {
                console.log('Ajax error.');
                notifyUpdate(false);
                trackEvent('updates', 'ajax error');
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
        var url;
        var weatherKey = $('#weatherKey').val();
        var provider = parseInt($('.tab-weather.active')[0].getAttribute('data-value'), 10);
        var overrideLocation = $('#overrideLocation').val();
        if (overrideLocation) {
            switch (provider) {
                case 0:
                    url = 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3&q=' + encodeURIComponent(overrideLocation);
                    break;
                case 1:
                    url = 'http://api.wunderground.com/api/' + weatherKey + '/conditions/forecast/q/' + overrideLocation + '.json';
                    break;
                case 2:
                    var query = encodeURIComponent('select * from weather.forecast where woeid in (select woeid from geo.places where text="' + overrideLocation + '")');
                    url = 'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=' + query;
                    break;
            }
            trackEvent('verify location', weatherProviders[provider]);
            console.log(url);
            $.ajax({
                type: "GET",
                url: url,
                success: function(data, status, xhr) {
                    if (status === 'success') {
                        try {
                            switch(provider) {
                                case 0:
                                    if (parseInt(data.cod, 10) === 404) {
                                        alert('Invalid location');
                                        trackEvent('verify location', weatherProviders[provider], 'invalid location');
                                    } else {
                                        alert('Valid location!');
                                        trackEvent('verify location', weatherProviders[provider], 'valid location');
                                    }
                                    break;
                                case 1:
                                    if (data.response.error || ! data.current_observation) {
                                        if (data.response.error && data.response.error.type === 'keynotfound') {
                                            alert('Invalid WeatherUnderground Key');
                                            trackEvent('verify location', weatherProviders[provider], 'api error');
                                        } else {
                                            alert('Invalid location');
                                            trackEvent('verify location', weatherProviders[provider], 'invalid location');
                                        }
                                    } else {
                                        alert('Valid location!');
                                        trackEvent('verify location', weatherProviders[provider], 'valid location');
                                    }
                                    break;
                                case 2:
                                    if (data.query.count === 0) {
                                        alert('Invalid location');
                                        trackEvent('verify location', weatherProviders[provider], 'invalid location');
                                    } else {
                                        alert('Valid location!');
                                        trackEvent('verify location', weatherProviders[provider], 'valid location');
                                    }
                                    break;
                            }
                        } catch (e) {
                            alert('Can\'t verify the location now. Please try again later.');
                            trackEvent('verify location', weatherProviders[provider], 'exception');
                        }

                    } else {
                        alert('Can\'t verify the location now. Please try again later.');
                        trackEvent('verify location', weatherProviders[provider], 'error', status);
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

    var setTabValues = function(containerId, itemValue) {
        var tabs = $('#' + containerId + ' .tab-button');
        for (var i = 0; i < tabs.length; ++i) {
            if (tabs[i].getAttribute('data-value') === itemValue) {
                $(tabs[i]).addClass('active');
                if (containerId === 'weatherProvider') {
                    toggleWeatherKeys(parseInt(itemValue, 10));
                }
            } else {
                $(tabs[i]).removeClass('active');
            }
        }
    };

    var loadData = function() {
        var hasData = false;
        try {
            for (var item in localStorage) {
                if (item.indexOf('preset-') !== -1) {
                    continue;
                }
                hasData = true;
                var itemValue = localStorage[item];
                var element = $('#' + item)[0];
                if (element) {
                    if (item.indexOf('Color') !== -1 || item === 'weatherKey' || item === 'timezones' ||
                        item == 'overrideLocation' || item === 'fontType' || item === 'dateFormat' || item === 'locale' ||
                        item == 'yahooKey') {
                        element.value = itemValue;
                    } else if (item.indexOf('textAlign') !== -1 || item.indexOf('weatherProvider') !== -1) {
                        setTabValues(item, itemValue);
                    } else {
                        element.checked = parse(itemValue) || false;
                        if (item.indexOf('enable') !== -1 && element.checked) {
                            var containerId = '#' + item.match(/enable(.*)/)[1].toLowerCase() + 'Configs';
                            $(containerId).toggleClass('hidden');
                        }
                    }
                }
            }
            if (!localStorage.weatherProvider && localStorage.weatherKey) {
                setTabValues('weatherProvider', '1');
            }
            trackEvent('data', 'loaded', hasData);
            loadColors(getColorValues());
        } catch (ex) {
            console.log(ex);
            trackEvent('data', 'loaded', 'error');
        }


    };

    var submitBtn = $('#submit');
    submitBtn.on('click', function(e) {
        trackEvent('data', 'saved');
        var returnTo = getQueryParam('return_to', 'pebblejs://close#');

        var selectedWeatherProvider = parseInt($('.tab-weather.active')[0].getAttribute('data-value'), 10);
        var id;
        switch (selectedWeatherProvider) {
            case 1:
                id = '#weatherKey';
                break;
        }

        if (id && !$(id).val()) {
            alert('Please enter a valid API key for the selected weather provider');
            trackEvent('weather provider', 'empty', selectedWeatherProvider);
            return;
        }

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

    var getColorValues = function() {
        return {
            bgColor: $('#bgColor').val(),
            hoursColor: $('#hoursColor').val(),
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
            stepsBehindColor: $('#stepsBehindColor').val(),
            distBehindColor: $('#distBehindColor').val(),
            updateColor: $('#updateColor').val(),
            bluetoothColor: $('#bluetoothColor').val()
        };
    };

    var getAndStoreConfigData = function() {
        var data = {
            enableHealth: $('#enableHealth')[0].checked,
            useKm: $('#useKm')[0].checked,
            showSleep: $('#showSleep')[0].checked,
            enableWeather: $('#enableWeather')[0].checked,
            weatherKey: $('#weatherKey').val(),
            yahooKey: $('#yahooKey').val(),
            useCelsius: $('#useCelsius')[0].checked,
            timezones: $('#timezones')[0].options[$('#timezones')[0].selectedIndex].value,
            fontType: $('#fontType')[0].options[$('#fontType')[0].selectedIndex].value,
            bluetoothDisconnect: $('#bluetoothDisconnect')[0].checked,
            overrideLocation: $('#overrideLocation').val(),
            enableAdvanced: $('#enableAdvanced')[0].checked,
            update: $('#update')[0].checked,
            locale: $('#locale')[0].options[$('#locale')[0].selectedIndex].value,
            dateFormat: $('#dateFormat')[0].options[$('#dateFormat')[0].selectedIndex].value,
            textAlign: $('.tab-align.active')[0].getAttribute('data-value'),
            weatherProvider: $('.tab-weather.active')[0].getAttribute('data-value'),
            leadingZero: $('#leadingZero')[0].checked,
            useCal: $('#useCal')[0].checked,
            slotA: $('#slotA')[0].options[$('#slotA')[0].selectedIndex].value,
            slotB: $('#slotB')[0].options[$('#slotB')[0].selectedIndex].value,
            slotC: $('#slotC')[0].options[$('#slotC')[0].selectedIndex].value,
            slotD: $('#slotD')[0].options[$('#slotD')[0].selectedIndex].value,
            sleepSlotA: $('#sleepSlotA')[0].options[$('#sleepSlotA')[0].selectedIndex].value,
            sleepSlotB: $('#sleepSlotB')[0].options[$('#sleepSlotB')[0].selectedIndex].value,
            sleepSlotC: $('#sleepSlotC')[0].options[$('#sleepSlotC')[0].selectedIndex].value,
            sleepSlotD: $('#sleepSlotD')[0].options[$('#sleepSlotD')[0].selectedIndex].value
        };

        $.extend(data, getColorValues());

        for (var item in data) {
            localStorage[item] = data[item];
        }

        console.log(JSON.stringify(data));
        return data;

    };

    toggleFieldsByPlatform();
    loadPresets();
    initGA();
    loadData();
    initEvents();
    checkForUpdates();

}(Zepto));
