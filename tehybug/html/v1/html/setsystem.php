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


<!-- Power Consumption Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-warning">
            <div class="card-header bg-warning text-dark">
                <h4 class="mb-0"><span data-feather="battery-charging"></span> Power Consumption Guide</h4>
            </div>
            <div class="card-body">
                <div class="table-responsive">
                    <table class="table table-sm">
                        <thead>
                            <tr>
                                <th>Configuration</th>
                                <th>Power Usage</th>
                                <th>Battery Life (2000mAh)</th>
                                <th>Best For</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td><strong>Normal Mode + 60s frequency</strong></td>
                                <td>~80mA continuous (WiFi always on)</td>
                                <td>~24 hours</td>
                                <td>BME680 air quality monitoring, AC powered</td>
                            </tr>
                            <tr>
                                <td><strong>Light Sleep + 300s frequency</strong></td>
                                <td>~5mA sleep + 100mA wake (5s every 5min)</td>
                                <td>~16 days</td>
                                <td>Regular monitoring, moderate battery life</td>
                            </tr>
                            <tr>
                                <td><strong>Deep Sleep + 900s frequency</strong></td>
                                <td>~20µA sleep + 100mA wake (5s every 15min)</td>
                                <td>~3-5 months</td>
                                <td>Long-term battery operation, periodic updates</td>
                            </tr>
                            <tr>
                                <td><strong>Deep Sleep + 3600s frequency</strong></td>
                                <td>~20µA sleep + 100mA wake (5s every hour)</td>
                                <td>~6-12 months</td>
                                <td>Maximum battery life, hourly updates</td>
                            </tr>
                        </tbody>
                    </table>
                </div>
                <div class="alert alert-info small mb-0">
                    <strong><span data-feather="zap"></span> Pro Tip:</strong> For battery operation, use Deep Sleep with 900s (15 min) or higher frequency. For AC power with air quality monitoring, use Normal Mode.
                </div>
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