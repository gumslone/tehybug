;
var ipAddress = $(location).attr('hostname');
var pageName = 'dash';
var devMode = false;
var timeleft;
var rebootTimer;
var json;
var boardURL = 'https://tehybug.com/tehybug/v1/';

if (ipAddress.includes('localhost')) {
    devMode = true;
}

$(function () {
    // Active Menu Button select
    $('.nav-link').click(function () {
        $('.nav-link').removeClass('active');
        $(this).addClass('active');
    });

    ChangePage('main', '#page');

    var script = document.createElement('script');
    script.src = 'https://cdn.jsdelivr.net/npm/feather-icons@4.28.0/dist/feather.min.js';
    script.type = 'text/javascript';
    document.getElementsByTagName('head')[0].appendChild(script);
    
    setTimeout(function () {
        connectionStart();
    }, 1000);
});

var connection = null;

function capitalizeFirstLetter(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function connectionStart() {
    if (connection != null && connection.readyState != WebSocket.CLOSED) {
        connection.close();
    }

    var wsServer = ipAddress;
    let socketPageName = pageName;
    
    if (pageName == "cloud_settings" || pageName == "ha_settings") {
        socketPageName = "settings";
    }

    connection = new WebSocket('ws://' + wsServer + ':81/' + socketPageName);

    connection.onopen = function () {
        updateConnectionStatus(true);

        if (pageName == 'setConfig') {
            connection.send(json);
        }

        KeepAlive();
    };

    connection.onclose = function (e) {
        console.log('WebSocket connection closed');
        updateConnectionStatus(false);
    };

    connection.onerror = function (error) {
        console.log('WebSocket Error: ' + error);
        if (connection.readyState !== WebSocket.CLOSED) {
            connection.close();
        }
    };

    connection.onmessage = function (e) {
        console.log('WebSocket incoming message: ' + e.data);
        RefreshData(e.data);
    };

    function KeepAlive() {
        const timeout = 1000;
        if (connection.readyState == WebSocket.OPEN) {
            connection.send("KeepAlive");
        }
        setTimeout(KeepAlive, timeout);
    }
}

function updateConnectionStatus(isOnline) {
    const statusElement = $("#connectionStatus");
    if (isOnline) {
        statusElement.html("Online").removeClass("text-danger").addClass("text-success");
    } else {
        statusElement.html("Offline").removeClass("text-success").addClass("text-danger");
    }
}

function sensorData(key, value) {
    const sensorMap = {
        'temp': { name: "Temperature", unit: "°C", url: '&t=%temp%', mqtt: ', "temp":"%temp%"' },
        'temp_imp': { name: "Temperature", unit: "°F", url: '', mqtt: ', "temp_imp":"%temp_imp%"' },
        'temp2': { name: "Temperature2", unit: "°C", url: '&t=%temp2%', mqtt: ', "temp2":"%temp2%"' },
        'temp2_imp': { name: "Temperature2", unit: "°F", url: '', mqtt: ', "temp2_imp":"%temp2_imp%"' },
        'humi': { name: "Humidity", unit: "%RH", url: '&h=%humi%', mqtt: ', "humi":"%humi%"' },
        'humi2': { name: "Humidity2", unit: "%RH", url: '', mqtt: ', "humi2":"%humi2%"' },
        'ah': { name: "Absolute humidity", unit: "g/m³", url: '&ah=%ah%', mqtt: ', "ah":"%ah%"' },
        'ah2': { name: "Absolute humidity2", unit: "g/m³", url: '', mqtt: ', "ah2":"%ah2%"' },
        'cr': { name: "Comfort ratio", unit: "%", url: '', mqtt: ', "cr":"%cr%"' },
        'cr2': { name: "Comfort ratio2", unit: "%", url: '', mqtt: ', "cr2":"%cr2%"' },
        'dew': { name: "Dew point", unit: "°C", url: '', mqtt: ', "dew":"%dew%"' },
        'dew_imp': { name: "Dew point", unit: "°F", url: '', mqtt: ', "dew_imp":"%dew_imp%"' },
        'hi': { name: "Heat index", unit: "°C", url: '', mqtt: ', "hi":"%hi%"' },
        'hi_imp': { name: "Heat index", unit: "°F", url: '', mqtt: ', "hi_imp":"%hi_imp%"' },
        'air': { name: "Gas resistance", unit: "kOhm", url: '&a=%air%', mqtt: ', "air":"%air%"' },
        'iaq': { name: "Indoor air quality", unit: "", url: '', mqtt: ', "iaq":"%iaq%"' },
        'qfe': { name: "Atmospheric pressure", unit: "hPa", url: '&p=%qfe%', mqtt: ', "qfe":"%qfe%"' },
        'alt': { name: "Altitude", unit: "m", url: '', mqtt: ', "alt":"%alt%"' },
        'eco2': { name: "CO2 equivalent", unit: "", url: '', mqtt: ', "eco2":"%eco2%"' },
        'bvoc': { name: "breath VOC equivalent", unit: "", url: '', mqtt: ', "bvoc":"%bvoc%"' },
        'uv': { name: "UV index", unit: "", url: '&u=%uv%', mqtt: ', "uv":"%uv%"' },
        'lux': { name: "Ambient light", unit: "Lux", url: '&l=%lux%', mqtt: ', "lux":"%lux%"' },
        'adc': { name: "ADC", unit: "ADC", url: '&x=%adc%', mqtt: ', "adc":"%adc%"' }
    };

    if (!sensorMap[key]) {
        return;
    }

    const sensor = sensorMap[key];
    
    $("#url").append(sensor.url);
    $("#mqtt_message").append(sensor.mqtt);

    if (pageName == 'main') {
        $("#sensor_data").append(`<tr><td>${sensor.name}</td><td>${value} ${sensor.unit}</td></tr>`);
    } else if (pageName == 'cloud_settings') {
        $("#httpGetURL").val($("#httpGetURL").val() + sensor.url);
    } else {
        $("#sensor_data").append(`<tr><td>${sensor.name}</td><td><code>%${key}%</code></td><td>${sensor.unit}</td></tr>`);
    }
}

function RefreshData(input) {
    // Validate JSON
    if (!input.startsWith("{")) {
        return;
    }

    const jsonData = $.parseJSON(input);

    // Skip if only contains empty key
    if (Object.keys(jsonData).length === 1 && jsonData.key === "") {
        console.log('Skipping empty key message');
        return;
    }

    // Log JSON
    if (jsonData.log) {
        const logArea = $('#log');
        logArea.append(`${jsonData.log.function}: ${jsonData.log.message}\n`);
        logArea.scrollTop(logArea[0].scrollHeight);
        return;
    }

    if (pageName == 'main') {
        $("#sensor_data").html('');
    }

    $.each(jsonData, function (key, val) {
        // Config JSON
        const configPages = ['settings', 'ha_settings', 'cloud_settings', 'setsensor', 'scenarios', 'setsystem'];
        if (configPages.includes(pageName)) {
            if (typeof val === 'boolean') {
                $("#" + key).not(".dont-change").prop('checked', val);
            } else {
                $("#" + key).not(".dont-change").val(val.toString());
            }
        }

        // SystemInfo JSON
        const infoPages = ['main', 'firststart', 'cloud_settings'];
        if (infoPages.includes(pageName)) {
            if (key === 'key' && val.toString() === "") {
                return; // Do not update if key is empty
            }
            $("#" + key).html(val.toString());
            if (key == 'key') {
                $("." + key).html(val.toString());
            }
        }

        // Sensor data
        const sensorPages = ['settings', 'cloud_settings', 'main'];
        if (sensorPages.includes(pageName)) {
            sensorData(key, val.toString());
        }
    });
}

function SaveConfig() {
    const obj = {};

    // Read all inputs
    $("input").each(function () {
        console.log(`SaveConfig -> ID: ${this.id}, Val: ${this.type == 'checkbox' ? $(this).prop('checked') : $(this).val()}`);

        if (this.type == 'checkbox') {
            obj[this.id] = $(this).prop('checked');
        } else {
            obj[this.id] = $(this).val();
        }
    });

    // Read all selects
    $("select").each(function () {
        console.log(`SaveConfig -> ID: ${this.id}, Val: ${$(this).val()}`);
        obj[this.id] = $(this).val();
    });

    json = JSON.stringify(obj);
    console.log(json);
    pageName = "setConfig";

    connectionStart();

    // Restart Countdown
    const timeout = 12000;
    StartCountDown(timeout / 1000);

    setTimeout(function () {
        $("#popup").modal('hide');
    }, timeout);

    setTimeout(function () {
        location.reload();
    }, timeout + 500);
}

function ChangePage(_pageName, destination) {
    pageName = _pageName;
    $(destination).load(boardURL + 'html/' + _pageName + '.php');
}

function ChangeContent(element, _pageName, destination) {
    pageName = _pageName;
    $(destination).load(boardURL + 'html/' + _pageName + '.php');
    $('.nav-link').removeClass('active');
    $(element).addClass('active');
}

function ChangeContentIframe(element, _pageName, destination) {
    pageName = _pageName;
    $(destination).html(`<iframe src="/${pageName}" title="" width="100%" height="100%"></iframe>`);
    $('.nav-link').removeClass('active');
    $(element).addClass('active');
}

function StartCountDown(_timeleft) {
    timeleft = _timeleft;
    rebootTimer = setInterval(function () {
        timeleft--;
        $("#countdowntimer").html(timeleft);
        if (timeleft <= 0) {
            clearInterval(rebootTimer);
        }
    }, 1000);
}

function SendTest(type, input) {
    if (isNullOrWhitespace(input)) {
        return;
    }

    const obj = {};
    console.log(obj);
    connection.send(obj);
}

function isNullOrWhitespace(input) {
    if (typeof input === 'undefined' || input == null) {
        return true;
    }
    return input.replace(/\s/g, '').length < 1;
}