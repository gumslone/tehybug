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
    // Akive Menu Button select 
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
        connectionStart()
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
    if(pageName == "cloud_settings"||pageName == "ha_settings")
    {
        socketPageName = "settings";
    }
    connection = new WebSocket('ws://' + wsServer + ':81/' + socketPageName);
    connection.onopen = function () {
        $("#connectionStatus").html("Online");
        $("#connectionStatus").removeClass("text-danger");
        $("#connectionStatus").addClass("text-success");

        if (pageName == 'setConfig') {
            connection.send(json);
            //connection.close();
        }

        KeepAlive();
    }
    connection.onclose = function (e) {
        // Debug
        console.log('WebSocket connection close');
        $("#connectionStatus").html("Offline");
        $("#connectionStatus").removeClass("text-success");
        $("#connectionStatus").addClass("text-danger");
        
        /*if (pageName == 'main') {
            setTimeout(function () {
                connectionStart()
            }, 1000);
              }*/

    }
    connection.onerror = function (error) {
        // Debug
        console.log('WebSocket Error ' + error);
        if (connection.readyState !== WebSocket.CLOSED) {
            connection.close();
        }
    }
    connection.onmessage = function (e) {
        // Debug
        console.log('WebSocket incomming message: ' + e.data);
        RefershData(e.data)
    }

    function KeepAlive() {
        var timeout = 1000;
        if (connection.readyState == WebSocket.OPEN) {
            connection.send("KeepAlive");
        }
        setTimeout(KeepAlive, timeout);
    }
}

function sensorData(key, value)
{
    let name = '';
    let unit = '';
    let url = '';
    let mqtt = '';
    switch (key) {
        case 'temp':
            name = "Temperature";
            unit = "°C";
            url = '&t=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'temp_imp':
            name = "Temperature";
            unit = "°F";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'temp2':
            name = "Temperature2";
            unit = "°C";
            url = '&t=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'temp2_imp':
            name = "Temperature2";
            unit = "°F";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'humi':
            name = "Humidity";
            unit = "%RH";
            url = '&h=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'humi2':
            name = "Humidity2";
            unit = "%RH";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'ah':
            name = "Absolute humidity";
            unit = "g/m³";
            url = '&ah=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'ah2':
            name = "Absolute humidity2";
            unit = "g/m³";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'cr':
            name = "Comfort ratio";
            unit = "%";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'cr2':
            name = "Comfort ratio2";
            unit = "%";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'dew':
            name = "Dew point";
            unit = "°C";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'dew_imp':
            name = "Dew point";
            unit = "°F";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'hi':
            name = "Heat index";
            unit = "°C";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'hi_imp':
            name = "Heat index";
            unit = "°F";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'air':
            name = "Gas resistance";
            unit = "kOhm";
            url = '&a=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'iaq':
            name = "Indoor air quality";
            unit = "";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'qfe':
            name = "Atmospheric pressure";
            unit = "hPa";
            url = '&p=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'alt':
            name = "Altitude";
            unit = "m";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'eco2':
            name = "CO2 equivalent";
            unit = "";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'bvoc':
            name = "breath VOC equivalent";
            unit = "";
            url = '';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'uv':
            name = "UV index";
            unit = "";
            url = '&u=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'lux':
            name = "Ambient light";
            unit = "Lux";
            url = '&l=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        case 'adc':
            name = "ADC";
            unit = "ADC";
            url = '&x=%' + key + '%';
            mqtt = ', "' + key + '":"%' + key + '%"';
            break;
        default:
            return;
      }
    $("#url").append(url);
    $("#mqtt_message").append(mqtt);
    if (pageName == 'main')
    {
        $("#sensor_data").append('<tr><td>' + name + '</td><td>' + value + ' ' + unit + '</td></tr>');
    }
    else if (pageName == 'cloud_settings')
    {
            $("#httpGetURL").val($("#httpGetURL").val() + url);
    }
    else
    {
        $("#sensor_data").append('<tr><td>' + name + '</td><td><code>%' + key + '%</code></td><td>' + unit + '</td></tr>');
    }
}


function RefershData(input) {
    // validate json
    if (!input.startsWith("{")) {
        return;
    }

    var json = $.parseJSON(input);
    // Log Json
    if (json.log) {
        var logArea = $('#log');
        logArea.append(json.log.function + ": " + json.log.message + "\n");
        logArea.scrollTop(logArea[0].scrollHeight);

    } else {
        if (pageName == 'main') {
            $("#sensor_data").html('');
        }
        $.each(json, function (key, val) {
            // Config Json
            if (
                pageName == 'settings' ||
                pageName == 'ha_settings' ||
                pageName == 'setsensor' ||
                pageName == 'scenarios' ||
                pageName == 'setsystem' 
            ) {
                if (typeof val === 'boolean') {
                    $("#" + key).not( ".dont-change" ).prop('checked', val);
                } else {
                    $("#" + key).not( ".dont-change" ).val(val.toString());
                }
            }
            // SystemInfo Json
            else if (pageName == 'main' ||
                    pageName == 'firststart' ||
                    //pageName == 'ha_settings' ||
                    pageName == 'cloud_settings') {
                $("#" + key).html(val.toString());
                if(key == 'key')
                {
                    $("." + key).html(val.toString());
                }
            }

            if (pageName == 'settings' ||
                pageName == 'cloud_settings' ||
                //pageName == 'ha_settings' ||
                pageName == 'main'){
                 sensorData(key, val.toString());
            }

        });
    }
}

function SaveConfig() {
    var obj = {};
    // read all imputs
    $("input").each(function () {
        // Debug
        console.log('SaveConfig -> ID: ' + this.id + ', Val: ' + (this.type == 'checkbox' ? $(this)
            .prop('checked') : $(this).val()));

        if (this.type == 'checkbox') {
            obj[this.id] = $(this).prop('checked');
        } else {
            obj[this.id] = $(this).val();
        }
    });

    // read all selects
    $("select").each(function () {
        // Debug
        console.log('SaveConfig -> ID: ' + this.id + ', Val: ' + $(this).val());
        obj[this.id] = $(this).val();
    });

    json = JSON.stringify(obj);
    // Debug
    console.log(json);
    pageName = "setConfig";

    connectionStart();

    // Restart Countdown.
    var timeout = 12000;
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

    $(destination).html('<iframe src="/' + pageName + '" title="" width="100%" height="100%"></iframe>');
    $('.nav-link').removeClass('active');
    $(element).addClass('active');

}
// 
// Countdown
function StartCountDown(_timeleft) {
    timeleft = _timeleft;
    rebootTimer = setInterval(function () {
        timeleft--;
        $("#countdowntimer").html(timeleft);
        if (timeleft <= 0)
            clearInterval(rebootTimer);
    }, 1000);
}

function SendTest(type, input) {

    if (isNullOrWhitespace(input)) {
        return;
    }

    var obj = {};
    // Debug
    console.log(obj);
    connection.send(obj);
}

function isNullOrWhitespace(input) {

    if (typeof input === 'undefined' || input == null) {
        return true;
    }
    return input.replace(/\s/g, '').length < 1;
}