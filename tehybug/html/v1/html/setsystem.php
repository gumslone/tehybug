<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
?>

<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">System Settings</h1>
</div>

<div class="row">
    <div class="col-lg-6 mb-4">
        <div class="card">
            <div class="card-header">System</div>
            <div class="card-body">
                <div class="form-check form-switch mb-2">
                    <input type="checkbox" class="form-check-input" id="sleepModeActive">
                    <label class="form-check-label" for="sleepModeActive">Deep Sleep (powersaving for battery operations)</label>
                </div>
                <div class="form-check form-switch mb-2">
                    <input type="checkbox" class="form-check-input" id="lightSleepModeActive">
                    <label class="form-check-label" for="lightSleepModeActive">Light Sleep (WiFi sleep only)</label>
                </div>
                <div class="form-check form-switch mb-2">
                    <input type="checkbox" class="form-check-input" id="configModeActive" checked>
                    <label class="form-check-label" for="configModeActive">Config Mode Active (disable config mode to activate live mode)</label>
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="reboot">
                    <label class="form-check-label" for="reboot">Reboot device after saving</label>
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

<div class="modal fade" id="popup">
    <div class="modal-dialog modal-dialog-centered modal-lg">
        <div class="modal-content">
            <div class="modal-header bg-light">
                <h3 class="modal-title text-success">Config saved!</h3>
            </div>
            <div class="modal-body">
                <h5>System will be restarted, please wait <span id="countdowntimer">9</span> seconds to reload!</h5>
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
                $('#lightSleepModeActive').prop('checked', false);
            }
        });
        $("#lightSleepModeActive").change(function() {
            if(this.checked) {
                $('#sleepModeActive').prop('checked', false);
            }
        });
    });
</script>