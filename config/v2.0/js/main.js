(function($) {
    'use strict';

    var trackEvent = function(category, action, label, value) {
        ga('send', 'event', category, action, label, value);
    };

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
            trackEvent(e.currentTarget.id, 'select', e.currentTarget.selectedOptions[0].value);
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
                if (confirm('Load the preset "' + text + '" ?')) {
                    trackEvent('presets', 'load', 'execute');
                    var colors = JSON.parse(localStorage['preset-' + text]);
                    console.log(localStorage['preset-' + text]);
                    for (var item in colors) {
                        var elem = $('#' + item);
                        var value = colors[item];
                        console.log(item + ': ' + value);
                        elem.val(value);
                        elem.parent().find('.item-styled-color .value').css('background-color', value.replace(/^0x/, '#'));
                    }
                } else {
                    trackEvent('presets', 'load', 'dismiss');
                }
            }
        });
    };

    var loadPresets = function() {
        for (var item in localStorage) {
            if (item.indexOf('preset-') !== -1) {
                $('#presetList').append('<label class="item">' + item.substring(7) + '</label>');
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
                    } else if (item.indexOf('textAlign') !== -1) {
                        var tabAligns = $('.tab-align');
                        for (var i = 0; i < tabAligns.length; ++i) {
                            if (tabAligns[i].getAttribute('data-value') === itemValue) {
                                $(tabAligns[i]).addClass('active');
                            } else {
                                $(tabAligns[i]).removeClass('active');
                            }
                        }
                    } else {
                        element.checked = parse(itemValue) || false;
                        if (item.indexOf('enable') !== -1 && element.checked) {
                            var containerId = '#' + item.match(/enable(.*)/)[1].toLowerCase() + 'Configs';
                            $(containerId).toggleClass('hidden');
                        }
                    }
                }
            }
            trackEvent('data', 'loaded', hasData);
        } catch (ex) {
            console.log(ex);
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
            useCelsius: $('#useCelsius')[0].checked,
            timezones: $('#timezones')[0].selectedOptions[0].value,
            fontType: $('#fontType')[0].selectedOptions[0].value,
            bluetoothDisconnect: $('#bluetoothDisconnect')[0].checked,
            overrideLocation: $('#overrideLocation').val(),
            enableAdvanced: $('#enableAdvanced')[0].checked,
            update: $('#update')[0].checked,
            locale: $('#locale')[0].selectedOptions[0].value,
            dateFormat: $('#dateFormat')[0].selectedOptions[0].value,
            textAlign: $('.tab-align.active')[0].getAttribute('data-value')
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
