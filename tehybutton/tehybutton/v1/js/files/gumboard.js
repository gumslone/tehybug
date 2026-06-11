;
var ipAddress = $(location).attr('hostname');
var pageName = 'main';
var devMode = false;
var timeleft;
var rebootTimer;
var json;
var boardURL = 'https://tehybug.com/tehybutton/v1/';


if (ipAddress.includes('localhost')) {
    devMode = true;
}

$(function () {
    // Active menu button select
    $('.nav-link').click(function () {
        $('.nav-link').removeClass('active');
        $(this).addClass('active');
    });

    ChangePage('main', '#page');

    setTimeout(function () {
        connectionStart()
    }, 1000);
});

var connection = null;
var keepAliveTimer = null;

function capitalizeFirstLetter(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

function connectionStart() {
    if (connection != null && connection.readyState != WebSocket.CLOSED) {
        connection.close();
    }
    var wsServer = ipAddress;

    connection = new WebSocket('ws://' + wsServer + ':81/' + pageName);
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
        RefreshData(e.data)
    }

    function KeepAlive() {
        var timeout = 1000;
        if (connection.readyState == WebSocket.OPEN) {
            connection.send("KeepAlive");
        }
        keepAliveTimer = setTimeout(KeepAlive, timeout);
    }

    // Only ever run one KeepAlive chain, even after reconnects.
    if (keepAliveTimer != null) {
        clearTimeout(keepAliveTimer);
        keepAliveTimer = null;
    }
}

function RefreshData(input) {
    // validate json
    if (!input.startsWith("{")) {
        return;
    }

    var json = $.parseJSON(input);
    // Log Json
    if (json.log) {
        var logArea = $('#log');
        logArea.append("[" + json.log.timeStamp + "] " + json.log.function + ": " + json.log.message + "\n");
        logArea.scrollTop(logArea[0].scrollHeight);

    } else {
        $.each(json, function (key, val) {
            // Config Json
            if (
                pageName == 'settings' ||
                pageName == 'setsystem' 
            ) {
               
                if (typeof val === 'boolean') {
                    $("#" + key).prop('checked', val);
                } else {
                    $("#" + key).val(val.toString());
                }
            }
            // SystemInfo Json
            else if (pageName == 'main' ||
                    pageName == 'firststart') {
                $("#" + key).html(val.toString());
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

function isNullOrWhitespace(input) {

    if (typeof input === 'undefined' || input == null) {
        return true;
    }
    return input.replace(/\s/g, '').length < 1;
}