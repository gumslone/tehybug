<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="col-md-12 row">
<h2 class="text-center">First start</h2>
<hr>
<div>
			<p>
				Remember/copy your tehybug key: <code id="key">Loading...</code></b><br>
			</p>
			<p>
				<b>Step 1 (optional depends on sensor used).</b><br>
				Enable the sensor that you use at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsensor', '#right-content');">Sensor settings</a> page.
			</p>
			<p>
				<b>Step 2.</b><br>
				Select the desired data serving mode at <a  href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">Data serving settings</a>.
				Data frequency setting means, how often the sensor data should be served. The higher you set the data frequency the longer the battery will last.
			</p>
			<p>
				<b>Step 3.</b><br>
				If everything is set, <a  href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">disable the configuration mode</a> to go live and start serving to sensor data. Also <a  href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">enable deep sleep</a> to start low power data serving mode (saves battery power). 
				Your TeHyBug will reboot and start serving the sensor data.
			</p>
			
			<p>
				More infos about TeHyBug can be found at tehybug.com or at the tindie store: https://www.tindie.com/stores/gumslone/
			</p>
			

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