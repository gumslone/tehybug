<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">TeHyBug Cloud Service Settings</h1>
</div>

<div class="card mb-4">
    <div class="card-header">
        Instructions
    </div>
    <div class="card-body">
        <p>This is the easiest way to start using your TeHyBug Device.</p>
        <ol>
            <li>Create an account at <a href="https://tehybug.com" target="_blank">TeHyBug.com cloud service</a>.</li>
            <li>Remember/copy your TeHyBug key: <code id="key">Loading...</code>. You will need to provide it in your account on the cloud service.</li>
            <li>Click the "Save Config" button below. Your device will restart and begin sending data to your account.</li>
        </ol>

        <div class="form-group" style="display:none;">
            <input type="url" class="form-control" id="httpGetURL" minlength="7" value="http://tehybug.com/track/?bug_key=%key%">
            <input type="number" class="form-control dont-change" id="httpGetFrequency" value="900">
            <input type="checkbox" class="form-check-input dont-change" id="httpGetActive" checked>
            <input type="checkbox" class="form-check-input dont-change" id="sleepModeActive" checked>
            <input type="checkbox" class="form-check-input dont-change" id="lightSleepModeActive">
            <input type="checkbox" class="form-check-input dont-change" id="configModeActive">
            <input type="checkbox" class="form-check-input dont-change" id="mqttActive">
            <input type="checkbox" class="form-check-input dont-change" id="httpPostActive">
            <input type="checkbox" class="form-check-input dont-change" id="reboot" checked>
        </div>
    </div>
</div>

<div class="card mb-4">
    <div class="card-header">
        API Endpoint
    </div>
    <div class="card-body">
        <p class="card-text mb-1">TeHyBug.com HTTP GET URL:</p>
        <code>http://tehybug.com/track/?bug_key=%key%</code>
    </div>
</div>

<div class="col-md-12 text-center">
    <button type="button" class="btn btn-outline-success" onclick="SaveConfig()" data-bs-toggle="modal" data-bs-target="#popup"><span data-feather="save"></span> Save
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

    <div>
            <h3>TeHyBug.com HTTP GET URL</h3>
            <div><code id="url">http://tehybug.com/track/?bug_key=%key%</code></div>
            <hr>
    </div>

<script>
    feather.replace();
    connectionStart();    
</script>