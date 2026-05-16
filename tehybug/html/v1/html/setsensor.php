<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="col-md-12 row">
<h2 class="text-center">Sensor settings</h2>
<hr>
<div class="col-md-3">
        <h2 class="text-center">Sensors Port B Green</h2>
        <hr>

        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="dht_sensor">
            <label class="form-check-label" for="dht_sensor">DHTXX Active</label>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="ds18b20_sensor">
            <label class="form-check-label" for="ds18b20_sensor">DS18B20 Active</label>
        </div>
    </div>
    <div class="offset-md-1 col-md-3">
        <h2 class="text-center">Sensors Port A Black</h2>
        <hr>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="second_dht_sensor">
            <label class="form-check-label" for="second_dht_sensor">DHTXX Active</label>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="second_ds18b20_sensor">
            <label class="form-check-label" for="second_ds18b20_sensor">DS18B20 Active</label>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="adc_sensor">
            <label class="form-check-label" for="adc_sensor">ADC Active</label>
        </div>
    </div>
    <div class="offset-md-1 col-md-3">
        <h2 class="text-center">Calibration</h2>
        <hr>
        <div class="form-group">
            <label for="calibrationTemp">Temperature</label>
            <input type="number" class="form-control" step=".1" id="calibrationTemp" value="0">
        </div>
        <div class="form-group">
            <label for="calibrationHumi">Humidity</label>
            <input type="number" class="form-control" step=".1" id="calibrationHumi" value="0">
        </div>
        <div class="form-group">
            <label for="calibrationQfe">Barometric Air Pressure</label>
            <input type="number" class="form-control" step="1" id="calibrationQfe" value="0">
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="calibrationActive">
            <label class="form-check-label" for="calibrationActive">Calibration active</label>
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
                <h5>System will be restarted, please wait <span id="countdowntimer">9</span> seconds to reload!
                </h5>
            </div>
        </div>
    </div>
</div>



<script>
    feather.replace();
    connectionStart();   

    $(function () {

        $("#dht_sensor").change(function() {
            if(this.checked) {
                //$('#am2320_sensor').prop('checked', false);
                $('#ds18b20_sensor').prop('checked', false);
            }
        });
        $("#ds18b20_sensor").change(function() {
            if(this.checked) {
                $('#dht_sensor').prop('checked', false);
                //$('#am2320_sensor').prop('checked', false);
            }
        });
        $("#second_ds18b20_sensor").change(function() {
            if(this.checked) {
                $('#adc_sensor').prop('checked', false);
                $('#second_dht_sensor').prop('checked', false);
            }
        });
        $("#adc_sensor").change(function() {
            if(this.checked) {
                $('#second_ds18b20_sensor').prop('checked', false);
                $('#second_dht_sensor').prop('checked', false);
            }
        });
        $("#second_dht_sensor").change(function() {
            if(this.checked) {
                $('#adc_sensor').prop('checked', false);
                $('#second_ds18b20_sensor').prop('checked', false);
            }
        });

        }); 
</script>