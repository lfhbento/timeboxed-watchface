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

    var loadData = function() {
        for (item in localStorage) {
            var itemValue = localStorage[item];
            var element = $('#' + item)[0];
            if (element) {
                if (item.indexOf('Color') !== -1 || item === 'weatherKey') {
                    element.value = itemValue;
                } else if (item === 'timezones') {
                    element.value = itemValue
                } else {
                    element.checked = itemValue === 'true';
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
            distColor: $('#distColor').val()
        };

        for (item in data) {
            localStorage[item] = data[item];
        }

        console.log(JSON.stringify(data));
        return data;

    };

    loadData();

}(Zepto));
