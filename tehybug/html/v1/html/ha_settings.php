<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">HomeAssistant Settings</h1>
</div>

<div class="card mb-4">
    <div class="card-header">
        Instructions
    </div>
    <div class="card-body">
        <p>This is another easy way to start using your TeHyBug Device. Provide the data for your HomeAssistant MQTT server below. Click the save config button, and your device will start sending data to your HomeAssistant system.</p>
    </div>
</div>

<div class="row">
    <div class="col-lg-6">
        <div class="card mb-4">
            <div class="card-header">
                HomeAssistant MQTT
            </div>
            <div class="card-body">
                <div class="form-group mb-3">
                    <label for="mqttServer">Server (ip)</label>
                    <input type="text" class="form-control" id="mqttServer" minlength="7" maxlength="15" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" placeholder="Loading or no data">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPort">Port</label>
                    <input type="number" class="form-control" id="mqttPort" placeholder="Loading or no data" value="1883">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttUser">User</label>
                    <input type="text" class="form-control" id="mqttUser" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttPassword">Password</label>
                    <input type="password" class="form-control" id="mqttPassword" placeholder="Optional" autocomplete="off">
                </div>
                <div class="form-group mb-3">
                    <label for="mqttFrequency">Data frequency (seconds)</label>
                    <input type="number" class="form-control" id="mqttFrequency" value="600" autocomplete="off">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="lightSleepModeActive">
                    <label class="form-check-label" for="lightSleepModeActive">Light Sleep</label>
                </div>
                <div class="form-check form-switch" title="makes sense for data frequencies above 300s">
                    <input type="checkbox" class="form-check-input" id="sleepModeActive">
                    <label class="form-check-label" for="sleepModeActive">Deep Sleep</label>
                </div>

                <div style="display:none;">
                    <input type="checkbox" class="form-check-input dont-change" id="httpGetActive">
                    <input type="checkbox" class="form-check-input dont-change" id="configModeActive">
                    <input type="checkbox" class="form-check-input dont-change" id="mqttActive">
                    <input type="checkbox" class="form-check-input dont-change" id="haActive" checked>
                    <input type="checkbox" class="form-check-input dont-change" id="httpPostActive">
                    <input type="checkbox" class="form-check-input dont-change" id="reboot" checked>
                </div>
            </div>
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

<!-- Home Assistant Integration Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-info">
            <div class="card-header bg-info text-white">
                <h4 class="mb-0"><span data-feather="home"></span> Home Assistant Integration</h4>
            </div>
            <div class="card-body">
                <p>TeHyBug integrates seamlessly with Home Assistant using MQTT auto-discovery. Once configured, your sensors will automatically appear in Home Assistant without manual configuration.</p>
                <div class="alert alert-success mb-0">
                    <strong><span data-feather="check-circle"></span> Auto-Discovery:</strong> All active sensors (temperature, humidity, pressure, air quality) will be automatically detected and added to Home Assistant.
                </div>
            </div>
        </div>
    </div>
</div>

<!-- MQTT Configuration Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-primary">
            <div class="card-header bg-primary text-white">
                <h4 class="mb-0"><span data-feather="settings"></span> MQTT Configuration</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-6 mb-3">
                        <h5>Required Settings</h5>
                        <table class="table table-sm">
                            <tbody>
                                <tr>
                                    <td><strong>Server (IP)</strong></td>
                                    <td>IP address of your Home Assistant or MQTT broker</td>
                                </tr>
                                <tr>
                                    <td><strong>Port</strong></td>
                                    <td>Default: 1883 (standard MQTT)<br>8883 for MQTT over TLS</td>
                                </tr>
                                <tr>
                                    <td><strong>User</strong></td>
                                    <td>MQTT username (optional if no auth)</td>
                                </tr>
                                <tr>
                                    <td><strong>Password</strong></td>
                                    <td>MQTT password (optional if no auth)</td>
                                </tr>
                                <tr>
                                    <td><strong>Data Frequency</strong></td>
                                    <td>How often to send data (in seconds)</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    
                    <div class="col-lg-6 mb-3">
                        <h5>Finding Your MQTT Broker</h5>
                        <div class="alert alert-info small">
                            <strong>Home Assistant Add-on:</strong>
                            <ol class="mb-0">
                                <li>Go to Settings → Add-ons</li>
                                <li>Install "Mosquitto broker"</li>
                                <li>Start the add-on</li>
                                <li>Create user in Settings → People → Users</li>
                                <li>Use Home Assistant IP as server</li>
                            </ol>
                        </div>
                        <div class="alert alert-warning small mb-0">
                            <strong>External MQTT Broker:</strong><br>
                            If using external broker (like CloudMQTT), enter its IP/hostname and credentials.
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Sleep Modes -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-warning">
            <div class="card-header bg-warning text-dark">
                <h4 class="mb-0"><span data-feather="battery"></span> Power Saving Modes</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-4 mb-3">
                        <h5>Normal Mode (No Sleep)</h5>
                        <ul class="small">
                            <li>Device always on and connected</li>
                            <li>Fastest response time</li>
                            <li>Highest power consumption</li>
                            <li>Best for USB-powered devices</li>
                            <li>Web interface always accessible</li>
                        </ul>
                        <div class="alert alert-info small mb-0">
                            <strong>Power:</strong> ~80mA continuous
                        </div>
                    </div>
                    
                    <div class="col-lg-4 mb-3">
                        <h5>Light Sleep Mode</h5>
                        <ul class="small">
                            <li>CPU sleeps between readings</li>
                            <li>WiFi stays connected</li>
                            <li>Moderate power savings</li>
                            <li>Web interface accessible</li>
                            <li>Good for frequent updates (60-300s)</li>
                        </ul>
                        <div class="alert alert-success small mb-0">
                            <strong>Power:</strong> ~20-40mA average
                        </div>
                    </div>
                    
                    <div class="col-lg-4 mb-3">
                        <h5>Deep Sleep Mode</h5>
                        <ul class="small">
                            <li>Device fully powers down</li>
                            <li>Wakes up only to send data</li>
                            <li>Maximum power savings</li>
                            <li>Web interface unavailable during sleep</li>
                            <li>Best for battery operation (>300s)</li>
                        </ul>
                        <div class="alert alert-success small mb-0">
                            <strong>Power:</strong> ~0.1mA sleep, ~80mA active
                        </div>
                    </div>
                </div>
                <div class="alert alert-warning mb-0">
                    <strong><span data-feather="alert-triangle"></span> Important:</strong> Light Sleep and Deep Sleep cannot be enabled simultaneously. Deep Sleep is recommended only for data frequencies above 300 seconds (5 minutes).
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Data Frequency Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-success">
            <div class="card-header bg-success text-white">
                <h4 class="mb-0"><span data-feather="clock"></span> Choosing Data Frequency</h4>
            </div>
            <div class="card-body">
                <table class="table table-sm">
                    <thead>
                        <tr>
                            <th>Frequency</th>
                            <th>Use Case</th>
                            <th>Recommended Sleep Mode</th>
                            <th>Battery Life (2000mAh)</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr>
                            <td><strong>30-60s</strong></td>
                            <td>Real-time monitoring, quick response</td>
                            <td>Normal or Light Sleep</td>
                            <td>~24 hours (Light Sleep)</td>
                        </tr>
                        <tr>
                            <td><strong>120-300s</strong></td>
                            <td>Regular monitoring, balanced</td>
                            <td>Light Sleep</td>
                            <td>~48 hours</td>
                        </tr>
                        <tr>
                            <td><strong>600s (10min)</strong></td>
                            <td>Standard home monitoring</td>
                            <td>Deep Sleep</td>
                            <td>~2-3 weeks</td>
                        </tr>
                        <tr>
                            <td><strong>1800s (30min)</strong></td>
                            <td>Long-term trends, low power</td>
                            <td>Deep Sleep</td>
                            <td>~2-3 months</td>
                        </tr>
                        <tr>
                            <td><strong>3600s (1hr)</strong></td>
                            <td>Maximum battery life</td>
                            <td>Deep Sleep</td>
                            <td>~6 months</td>
                        </tr>
                    </tbody>
                </table>
                <div class="alert alert-info small mb-0">
                    <strong>Tip:</strong> For USB-powered devices, use 60-600 seconds with Normal or Light Sleep. For battery operation, use 600+ seconds with Deep Sleep.
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Setup Steps -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-primary">
            <div class="card-header bg-primary text-white">
                <h4 class="mb-0"><span data-feather="list"></span> Step-by-Step Setup</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-6 mb-3">
                        <h5>1. Prepare Home Assistant</h5>
                        <ol class="small">
                            <li>Install Mosquitto MQTT broker add-on</li>
                            <li>Start the broker</li>
                            <li>Create MQTT user (Settings → People)</li>
                            <li>Enable MQTT integration (Settings → Integrations)</li>
                            <li>Note your Home Assistant IP address</li>
                        </ol>
                    </div>
                    
                    <div class="col-lg-6 mb-3">
                        <h5>2. Configure TeHyBug</h5>
                        <ol class="small">
                            <li>Enter Home Assistant IP in "Server" field</li>
                            <li>Keep port as 1883 (default)</li>
                            <li>Enter MQTT username and password</li>
                            <li>Set desired data frequency</li>
                            <li>Choose sleep mode if needed</li>
                            <li>Click "Save Config"</li>
                        </ol>
                    </div>
                </div>
                <div class="row">
                    <div class="col-lg-12">
                        <h5>3. Verify in Home Assistant</h5>
                        <ol class="small">
                            <li>Wait for device to restart (12 seconds)</li>
                            <li>Go to Settings → Devices & Services</li>
                            <li>Look for "MQTT" integration</li>
                            <li>Click on it to see discovered devices</li>
                            <li>Your TeHyBug sensors should appear automatically</li>
                            <li>Add them to your dashboard</li>
                        </ol>
                        <div class="alert alert-success small mb-0">
                            <strong><span data-feather="check"></span> Success:</strong> You should see entities like "sensor.tehybug_temperature", "sensor.tehybug_humidity", etc.
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Troubleshooting -->
<div class="row mt-4 mb-4">
    <div class="col-md-12">
        <div class="card border-danger">
            <div class="card-header bg-danger text-white">
                <h4 class="mb-0"><span data-feather="alert-circle"></span> Troubleshooting</h4>
            </div>
            <div class="card-body">
                <div class="accordion" id="troubleshootingAccordion">
                    <!-- Problem 1 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#trouble1">
                                <strong>Sensors not appearing in Home Assistant</strong>
                            </button>
                        </h2>
                        <div id="trouble1" class="accordion-collapse collapse show" data-bs-parent="#troubleshootingAccordion">
                            <div class="accordion-body">
                                <p><strong>Possible causes and solutions:</strong></p>
                                <ul class="small">
                                    <li><strong>MQTT broker not running:</strong> Check if Mosquitto add-on is started in Home Assistant</li>
                                    <li><strong>Wrong credentials:</strong> Verify username and password match your MQTT user</li>
                                    <li><strong>Wrong IP address:</strong> Ensure you're using the correct Home Assistant IP</li>
                                    <li><strong>Firewall blocking:</strong> Check if port 1883 is open on your network</li>
                                    <li><strong>MQTT integration not enabled:</strong> Go to Settings → Integrations and add MQTT if missing</li>
                                    <li><strong>Wait for first data send:</strong> Sensors appear after first data transmission (based on frequency setting)</li>
                                </ul>
                                <div class="alert alert-info small mb-0">
                                    <strong>Quick Test:</strong> Check MQTT logs in Home Assistant (Settings → System → Logs) for connection attempts from TeHyBug
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Problem 2 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#trouble2">
                                <strong>Connection timeout or failed to connect</strong>
                            </button>
                        </h2>
                        <div id="trouble2" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                            <div class="accordion-body">
                                <p><strong>Check these items:</strong></p>
                                <ul class="small">
                                    <li><strong>Network connectivity:</strong> Ensure TeHyBug is connected to WiFi (check WiFi Settings page)</li>
                                    <li><strong>Same network:</strong> TeHyBug and Home Assistant must be on the same network or have routing between them</li>
                                    <li><strong>IP address format:</strong> Use IP address (e.g., 192.168.1.100), not hostname</li>
                                    <li><strong>Port number:</strong> Verify port 1883 is correct (check Mosquitto configuration)</li>
                                    <li><strong>Broker capacity:</strong> Some MQTT brokers have connection limits</li>
                                </ul>
                                <div class="bg-light p-2 rounded small">
                                    <strong>Test command (from another device on network):</strong><br>
                                    <code>telnet YOUR_HA_IP 1883</code><br>
                                    If connection refused, MQTT broker is not accessible
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Problem 3 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#trouble3">
                                <strong>Data not updating or stale values</strong>
                            </button>
                        </h2>
                        <div id="trouble3" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                            <div class="accordion-body">
                                <p><strong>Possible reasons:</strong></p>
                                <ul class="small">
                                    <li><strong>Deep Sleep active:</strong> Device only wakes at set intervals, web interface unavailable during sleep</li>
                                    <li><strong>Long data frequency:</strong> If set to 600s (10 min), updates only happen every 10 minutes</li>
                                    <li><strong>Sensor errors:</strong> Check Sensor Settings page to verify sensors are working</li>
                                    <li><strong>Power issues:</strong> Low battery or unstable power can cause missed transmissions</li>
                                    <li><strong>WiFi disconnections:</strong> Check WiFi signal strength and stability</li>
                                </ul>
                                <div class="alert alert-warning small mb-0">
                                    <strong>Note:</strong> In Deep Sleep mode, the device is completely offline between data transmissions. This is normal behavior for battery conservation.
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Problem 4 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#trouble4">
                                <strong>Authentication failed</strong>
                            </button>
                        </h2>
                        <div id="trouble4" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                            <div class="accordion-body">
                                <p><strong>Steps to fix:</strong></p>
                                <ol class="small">
                                    <li>Verify MQTT user exists in Home Assistant (Settings → People → Users)</li>
                                    <li>Check username and password are typed correctly (case-sensitive)</li>
                                    <li>Ensure user has MQTT permissions</li>
                                    <li>Try creating a new MQTT user specifically for TeHyBug</li>
                                    <li>If using external broker, verify credentials with broker provider</li>
                                </ol>
                                <div class="alert alert-info small mb-0">
                                    <strong>Tip:</strong> Some special characters in passwords may cause issues. Try using alphanumeric passwords only.
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Problem 5 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#trouble5">
                                <strong>Device keeps restarting or unstable</strong>
                            </button>
                        </h2>
                        <div id="trouble5" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                            <div class="accordion-body">
                                <p><strong>Common causes:</strong></p>
                                <ul class="small">
                                    <li><strong>Power supply issues:</strong> Use quality USB power adapter (min 500mA)</li>
                                    <li><strong>Sleep mode conflicts:</strong> Don't enable both Light Sleep and Deep Sleep</li>
                                    <li><strong>Too frequent updates:</strong> Very short data frequencies (<30s) may cause instability</li>
                                    <li><strong>Memory issues:</strong> Multiple active integrations may exceed available memory</li>
                                    <li><strong>Sensor conflicts:</strong> Some sensor combinations may not work together</li>
                                </ul>
                                <div class="alert alert-danger small mb-0">
                                    <strong>Solution:</strong> Try increasing data frequency to 60s or more, disable one sleep mode, and ensure stable power supply.
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                
                <!-- Additional Help -->
                <div class="row mt-3">
                    <div class="col-md-12">
                        <div class="alert alert-primary mb-0">
                            <h5><span data-feather="help-circle"></span> Still Having Issues?</h5>
                            <ul class="small mb-0">
                                <li>Check the <strong>Dashboard</strong> page for system status and error messages</li>
                                <li>Review <strong>Sensor Settings</strong> to ensure sensors are properly configured</li>
                                <li>Verify <strong>WiFi Settings</strong> show strong signal and stable connection</li>
                                <li>Try factory reset and reconfigure from scratch</li>
                                <li>Consult Home Assistant community forums for MQTT-specific issues</li>
                            </ul>
                        </div>
                    </div>
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
                //$('#am2320_sensor').prop('checked', false);
                $('#lightSleepModeActive').prop('checked', false);
            }
        });
        $("#lightSleepModeActive").change(function() {
            if(this.checked) {
                $('#sleepModeActive').prop('checked', false);
                //$('#am2320_sensor').prop('checked', false);
            }
        });

    });
</script>