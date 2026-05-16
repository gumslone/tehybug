<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="col-md-12 row">
<h2 class="text-center">TeHyBug.com cloud service settings</h2>
<hr>

    <div class="offset-md-12">
        <p>
			This is probably the easiest way to start using your TeHyBug Device.<br>
            Create an account at <a href="https://tehybug.com" target="_blank">TeHyBug.com cloud service</a>.<br>
            Remember/copy your tehybug key: <code id="key">Loading...</code>, you will have to provide it at your accoount at <a href="https://tehybug.com" target="_blank">TeHyBug.com cloud service</a>.<br>
            Click the save config button below and your device will start sending data to your account at <a href="https://tehybug.com" target="_blank">TeHyBug.com cloud service</a>.<br>
		</p>

        <div class="form-group">
            <input type="url" class="form-control" id="httpGetURL" minlength="7" value="http://tehybug.com/track/?bug_key=%key%" style="display:none;">
            <input type="number" class="form-control dont-change" id="httpGetFrequency" value="900" style="display:none;">
            <input type="checkbox" class="form-check-input dont-change" id="httpGetActive" style="display:none;" checked >
            <input type="checkbox" class="form-check-input dont-change" id="sleepModeActive" style="display:none;" checked >
            <input type="checkbox" class="form-check-input dont-change" id="lightSleepModeActive" style="display:none;" >
            <input type="checkbox" class="form-check-input dont-change" id="configModeActive" style="display:none;"  >
            <input type="checkbox" class="form-check-input dont-change" id="mqttActive" style="display:none;">
            <input type="checkbox" class="form-check-input dont-change" id="httpPostActive" style="display:none;">
            <input type="checkbox" class="form-check-input dont-change" id="reboot" style="display:none;" checked >
        </div>
    </div>
   
</div>


<div class="col-md-12 text-center">
    <hr>
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