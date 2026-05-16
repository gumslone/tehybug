<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">Quick Start Guide</h1>
</div>

<div class="card">
    <div class="card-header">
        Getting Started
    </div>
    <div class="card-body">
        <p>Remember/copy your tehybug key: <code id="key">Loading...</code></p>
        <hr>
        <h5 class="card-title">Step 1 (optional)</h5>
        <p class="card-text">Enable the sensor that you use at the <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsensor', '#right-content');">Sensor settings</a> page. This depends on the sensor you have connected.</p>
        <hr>
        <h5 class="card-title">Step 2</h5>
        <p class="card-text">Select the desired data serving mode at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">Data serving settings</a>. The data frequency setting determines how often sensor data is sent. A lower frequency will extend battery life.</p>
        <hr>
        <h5 class="card-title">Step 3</h5>
        <p class="card-text">Once everything is configured, <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">disable the configuration mode</a> to go live. Also, <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">enable deep sleep</a> for low-power operation to save battery. Your TeHyBug will reboot and start serving sensor data.</p>
        <hr>
        <p class="card-text">More information about TeHyBug can be found at tehybug.com or at the Tindie store: <a href="https://www.tindie.com/stores/gumslone/" target="_blank">https://www.tindie.com/stores/gumslone/</a></p>
    </div>
</div>


<div class="col-md-12 text-center">
    <hr>
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
</script>