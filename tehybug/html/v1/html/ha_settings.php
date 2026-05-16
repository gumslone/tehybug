<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">HomeAssistant Settings</h1>
</div>

<div class="card mb-4">
    <div class="card-header">
        Instructions
    </div>
    <div class="card-body">
        <p>This is another easy way to start using your TeHyBug Device. Provide the data for your HomeAssistant MQTT server below. Click the save config button, and your device will start sending data to your HomeAssistant system.</p>
    </div>
</div>

<div class="row">
    <div class="col-lg-6">
        <div class="card mb-4">
            <div class="card-header">
                HomeAssistant MQTT
            </div>
            <div class="card-body">
                <div class="form-group mb-3">
                    <label for="mqttServer">Server (ip)</label>
                    <input type="text" class="form-control" id="mqttServer" minlength="7" maxlength="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" placeholder="Loading or no data">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPort">Port</label>
                    <input type="number" class="form-control" id="mqttPort" placeholder="Loading or no data" value="1883">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttUser">User</label>
                    <input type="text" class="form-control" id="mqttUser" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPassword">Password</label>
                    <input type="password" class="form-control" id="mqttPassword" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttFrequency">Data frequency (seconds)</label>
                    <input type="number" class="form-control" id="mqttFrequency" value="600" autocomplete="off">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="lightSleepModeActive">
                    <label class="form-check-label" for="lightSleepModeActive">Light Sleep</label>
                </div>
                <div class="form-check form-switch" title="makes sense for data frequencies above 300s">
                    <input type="checkbox" class="form-check-input" id="sleepModeActive">
                    <label class="form-check-label" for="sleepModeActive">Deep Sleep</label>
                </div>

                <div style="display:none;">
                    <input type="checkbox" class="form-check-input dont-change" id="httpGetActive">
                    <input type="checkbox" class="form-check-input dont-change" id="configModeActive">
                    <input type="checkbox" class="form-check-input dont-change" id="mqttActive">
                    <input type="checkbox" class="form-check-input dont-change" id="haActive" checked>
                    <input type="checkbox" class="form-check-input dont-change" id="httpPostActive">
                    <input type="checkbox" class="form-check-input dont-change" id="reboot" checked>
                </div>
            </div>
        </div>
    </div>
</div>


<div class="col-md-12 text-center">
    <hr>
</div>
<div class="col-md-12 text-center">
    <button type="button" class="btn btn-outline-success"  onclick="SaveConfig()" data-bs-toggle="modal" data-bs-target="#popup"><span data-feather="save"></span> Save
        Config</button>
</div>

<div class="modal fade" id="popup">
    <div class="modal-dialog modal-dialog-centered modal-lg">
        <div class="modal-content">
            <div class="modal-header bg-light">
                <h3 class="modal-title text-success">Config saved!</h3>
            </div>
            <div class="modal-body">
                <h5>System will be restarted, please wait <span id="countdowntimer">12 </span> seconds to reload!
                </h5>
            </div>
        </div>
    </div>
</div>
<script>
    feather.replace();
    connectionStart();   
    $(function () {

        $("#sleepModeActive").change(function() {
            if(this.checked) {
                //$('#am2320_sensor').prop('checked', false);
                $('#lightSleepModeActive').prop('checked', false);
            }
        });
        $("#lightSleepModeActive").change(function() {
            if(this.checked) {
                $('#sleepModeActive').prop('checked', false);
                //$('#am2320_sensor').prop('checked', false);
            }
        });

    });
</script>