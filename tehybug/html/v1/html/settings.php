<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
?>

<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">Data Serving Settings</h1>
</div>

<div class="row">
    <!-- MQTT Section -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">MQTT</div>
            <div class="card-body">
                <div class="form-group mb-3">
                    <label for="mqttServer" class="form-label">Server</label>
                    <input type="text" class="form-control" id="mqttServer" minlength="7" maxlength="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" placeholder="Loading or no data">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPort" class="form-label">Port</label>
                    <input type="number" class="form-control" id="mqttPort" placeholder="Loading or no data" value="1883">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttUser" class="form-label">User</label>
                    <input type="text" class="form-control" id="mqttUser" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPassword" class="form-label">Password</label>
                    <input type="password" class="form-control" id="mqttPassword" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttMasterTopic" class="form-label">Topic</label>
                    <input type="text" class="form-control" id="mqttMasterTopic">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttMessage" class="form-label">Message</label>
                    <input type="text" class="form-control" id="mqttMessage" placeholder="Loading or no data">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttFrequency" class="form-label">Data Frequency (seconds)</label>
                    <input type="number" class="form-control" id="mqttFrequency" value="900">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="mqttRetained">
                    <label class="form-check-label" for="mqttRetained">MQTT retained</label>
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="mqttActive">
                    <label class="form-check-label" for="mqttActive">MQTT active</label>
                </div>
                <input type="checkbox" class="form-check-input dont-change" id="haActive" style="display:none;">
            </div>
        </div>
    </div>

    <!-- HTTP GET Section -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">HTTP GET</div>
            <div class="card-body">
                <div class="form-group mb-3">
                    <label for="httpGetURL" class="form-label">HTTP Get URL</label>
                    <input type="url" class="form-control" id="httpGetURL" minlength="7" placeholder="https://example.com Loading or no data" pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?" >
                </div>
                <div class="form-group mb-3">
                    <label for="httpGetFrequency" class="form-label">Data Frequency (seconds)</label>
                    <input type="number" class="form-control" id="httpGetFrequency" value="900">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="httpGetActive">
                    <label class="form-check-label" for="httpGetActive">HTTP active</label>
                </div>
            </div>
        </div>
    </div>

    <!-- HTTP POST Section -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">HTTP POST</div>
            <div class="card-body">
                <div class="form-group mb-3">
                    <label for="httpPostURL" class="form-label">HTTP Post URL</label>
                    <input type="url" class="form-control" id="httpPostURL" minlength="7" placeholder="https://example.com Loading or no data" pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?" >
                </div>
                <div class="form-group mb-3">
                    <label for="httpPostFrequency" class="form-label">Data Frequency (seconds)</label>
                    <input type="number" class="form-control" id="httpPostFrequency" value="900">
                </div>
                <div class="form-group mb-3">
                    <label for="httpPostJson" class="form-label">Post Json</label>
                    <input type="text" class="form-control" id="httpPostJson" placeholder="Loading or no data">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="httpPostActive">
                    <label class="form-check-label" for="httpPostActive">HTTP active</label>
                </div>
            </div>
        </div>
    </div>
</div>

<div class="row">
    <div class="col-md-12 my-4">
        <hr>
        <div class="text-center">
            <button type="button" class="btn btn-outline-success" onclick="SaveConfig()" data-bs-toggle="modal" data-bs-target="#popup">
                <span data-feather="save"></span> Save Config
            </button>
        </div>
    </div>
</div>


<!-- Success Modal -->
<div class="modal fade" id="popup">
    <div class="modal-dialog modal-dialog-centered modal-lg">
        <div class="modal-content">
            <div class="modal-header bg-light">
                <h3 class="modal-title text-success">Config saved!</h3>
            </div>
            <div class="modal-body">
                <h5>System will be restarted, please wait <span id="countdowntimer">12</span> seconds to reload!</h5>
            </div>
        </div>
    </div>
</div>

<!-- Placeholders Section -->
<div class="row">
    <div class="col-md-12 mt-4">
        <h3 class="mb-3">Placeholders</h3>
        <div class="table-responsive">
            <table class="table table-striped table-sm" id="table">
                <thead>
                    <tr>
                        <th class="font-weight-bold">Name</th>
                        <th>Placeholder</th>
                        <th>Unit</th>
                    </tr>
                </thead>
                <tbody id="sensor_data"></tbody>
            </table>
        </div>

        <div class="mt-4">
            <h4>TeHyBug.com HTTP GET URL</h4>
            <div class="bg-light p-2 rounded"><code id="url">http://tehybug.com/track/?bug_key=%key%</code></div>
            
            <hr class="my-4">
            
            <h4>TeHyBug.com HTTP POST or MQTT message</h4>
            <div class="bg-light p-2 rounded"><code>{"bug_key":"%key%"<i id="mqtt_message"></i>}</code></div>
        </div>
    </div>
</div>

<script>
    feather.replace();
    connectionStart();
</script>