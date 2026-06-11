<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<div class="col-md-12 row">
<h2 class="text-center">Data serving settings</h2>
<hr>
<p class="help-text col-md-12" style="margin-bottom:1rem;">
    Choose how the button reports a press. Every active method runs on each
    press. In text fields, <code>%state%</code> is replaced with the kind of
    press (<code>click</code>, <code>double click</code> or
    <code>long click</code>) and <code>%key%</code> with the device key.
</p>
    <div class="col-md-3">
        <h2 class="text-center">MQTT</h2>
        <hr>
        <div class="form-group">
            <label for="mqttServer">Server</label>
            <input type="text" class="form-control" id="mqttServer" minlength="7" maxlength="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" placeholder="Loading or no data">
            <span class="help-text">IPv4 address of your MQTT broker, e.g. <code>192.168.1.10</code>.</span>
        </div>
        <div class="form-group">
            <label for="mqttPort">Port</label>
            <input type="number" class="form-control" id="mqttPort" placeholder="Loading or no data">
            <span class="help-text">Broker port. Standard MQTT uses <code>1883</code>.</span>
        </div>
        <div class="form-group">
            <label for="mqttUser">User</label>
            <input type="text" class="form-control" id="mqttUser" placeholder="Optional" autocomplete="off">
        </div>
        <div class="form-group">
            <label for="mqttPassword">Password</label>
            <input type="password" class="form-control" id="mqttPassword" placeholder="Optional" autocomplete="off">
            <span class="help-text">Login is only used when <em>both</em> user and password are filled in; otherwise the button connects anonymously.</span>
        </div>
        <div class="form-group">
            <label for="mqttMasterTopic">Topic</label>
            <input type="text" class="form-control" id="mqttMasterTopic">
            <span class="help-text">Topic the press is published to, e.g. <code>/tehybug</code>.</span>
        </div>
        <div class="form-group">
            <label for="mqttMessage">Message</label>
            <input type="text" class="form-control" id="mqttMessage" placeholder="Loading or no data">
            <span class="help-text">Payload to publish. Placeholders are replaced, e.g. <code>{"key":"%key%","state":"%state%"}</code>.</span>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="mqttRetained">
            <label class="form-check-label" for="mqttRetained">MQTT retained</label>
            <span class="help-text">The broker keeps the last message and hands it to new subscribers.</span>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="mqttActive">
            <label class="form-check-label" for="mqttActive">MQTT active</label>
        </div>
    </div>
    <div class="offset-md-1 col-md-3">
        <h2 class="text-center">HTTP GET</h2>
        <hr>
        <div class="form-group">
            <label for="httpGetURL">HTTP Get URL</label>
            <input type="url" class="form-control" id="httpGetURL" minlength="7" placeholder="https://example.com  Loading or no data"
  pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
            <span class="help-text">This URL is requested on every press. Placeholders work inside the URL, e.g.
            <code>http://tehybug.com/track/?bug_key=%key%</code>. Mind that <code>double click</code> and
            <code>long click</code> contain a space, so prefer <code>%state%</code> in a POST body or MQTT message.</span>
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
            <span class="help-text">The press is sent to this URL as an HTTP POST. Placeholders are <em>not</em> replaced in the POST URL — put them in the JSON body below.</span>
        </div>
        <div class="form-group">
            <label for="httpPostJson">Post Json</label>
            <input type="text" class="form-control" id="httpPostJson" placeholder="Loading or no data">
            <span class="help-text">JSON body of the request, sent with <code>Content-Type: application/json</code>. Placeholders are replaced, e.g. <code>{"bug_key":"%key%","state":"%state%"}</code>.</span>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="httpPostActive">
            <label class="form-check-label" for="httpPostActive">HTTP active</label>
        </div>
    </div>
</div>

<?php include dirname(__FILE__) . '/../inc/save_modal.php'; ?>

<div class="col-md-12">
    <div class="tb-card" style="margin-top:1rem;">
        <h3>tehybug.com examples</h3>
        <p>HTTP GET URL:</p>
        <div><code id="url">http://tehybug.com/track/?bug_key=%key%</code></div>
        <p style="margin-top:.75rem;">HTTP POST or MQTT message:</p>
        <div><code>{"bug_key":"%key%"}</code></div>
    </div>
</div>
<script>
    feather.replace();
    connectionStart();
</script>
