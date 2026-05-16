<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="col-md-12 row">
<h2 class="text-center">Data serving settings</h2>
<hr>
    <div class="col-md-3">
        <h2 class="text-center">MQTT</h2>
        <hr>
        <div class="form-group">
            <label for="mqttServer">Server</label>
            <input type="text" class="form-control" id="mqttServer" minlength="7" maxlength="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" placeholder="Loading or no data">
        </div>
        <div class="form-group">
            <label for="mqttPort">Port</label>
            <input type="number" class="form-control" id="mqttPort" placeholder="Loading or no data"  value="1883">
        </div>
        <div class="form-group">
            <label for="mqttUser">User</label>
            <input type="text" class="form-control" id="mqttUser" placeholder="Optional" autocomplete="off">
        </div>
        <div class="form-group">
            <label for="mqttPassword">Password</label>
            <input type="password" class="form-control" id="mqttPassword" placeholder="Optional" autocomplete="off">
        </div>
        <div class="form-group">
            <label for="mqttMasterTopic">Topic</label>
            <input type="text" class="form-control" id="mqttMasterTopic">
        </div>
        <div class="form-group">
            <label for="mqttMessage">Message</label>
            <input type="text" class="form-control" id="mqttMessage" placeholder="Loading or no data">
        </div>
        <div class="form-group">
            <label for="mqttFrequency">Data Frequency (seconds)</label>
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
    <div class="offset-md-1 col-md-3">
        <h2 class="text-center">HTTP GET</h2>
        <hr>
        <div class="form-group">
            <label for="httpGetURL">HTTP Get URL</label>
            <input type="url" class="form-control" id="httpGetURL" minlength="7" placeholder="https://example.com  Loading or no data"
  pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
        </div>
        <div class="form-group">
            <label for="httpGetFrequency">Data Frequency (seconds)</label>
            <input type="number" class="form-control" id="httpGetFrequency" value="900">
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="httpGetActive">
            <label class="form-check-label" for="httpGetActive">HTTP active</label>
        </div>
    </div>
    <div class="offset-md-1 col-md-3">
        <h2 class="text-center">HTTP POST</h2>
        <hr>
        <div class="form-group">
            <label for="httpPostURL">HTTP Post URL</label>
            <input type="url" class="form-control" id="httpPostURL" minlength="7" placeholder="https://example.com Loading or no data"
  pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
        </div>
        <div class="form-group">
            <label for="httpPostFrequency">Data Frequency (seconds)</label>
            <input type="number" class="form-control" id="httpPostFrequency" value="900">
        </div>
        <div class="form-group">
            <label for="httpPostJson">Post Json</label>
            <input type="text" class="form-control" id="httpPostJson" placeholder="Loading or no data">
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="httpPostActive">
            <label class="form-check-label" for="httpPostActive">HTTP active</label>
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

<div class="col-md-12">
    <h3>Placeholders</h3>
        <div class="table-responsive">
              <table class="table table-striped table-sm" id="table">
                <thead>
                    <tr>
                          <td class="font-weight-bold">Name</td>
                          <td>Placeholder</td>
                          <td>Unit</td>
                    </tr>
                </thead>  
                <tbody id="sensor_data">
                  </tbody>
              </table>
          </div>
    
    <div>
            <h3>TeHyBug.com HTTP GET URL</h3>
            <div><code id="url">http://tehybug.com/track/?bug_key=%key%</code></div>
            <hr>
            <h3>TeHyBug.com HTTP POST or MQTT message</h3>
            <div><code>{"bug_key":"%key%"<i id="mqtt_message"></i>}</code></div>
            <hr>
    </div>
</div>
<script>
    feather.replace();
    connectionStart();    
</script>