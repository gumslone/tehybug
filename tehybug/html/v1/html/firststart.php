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
        <div class="alert alert-info" role="alert">
            <strong>Important:</strong> This unique key identifies your device. Save it for future reference and API integrations.
        </div>
        <hr>
        
        <h5 class="card-title">Step 1: Configure Sensors</h5>
        <p class="card-text">Enable the sensor(s) connected to your TeHyBug at the <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsensor', '#right-content');">Sensor settings</a> page.</p>
        <div class="alert alert-warning" role="alert">
            <strong>Supported Sensors:</strong>
            <ul class="mb-0">
                <li><strong>Port A (BLACK):</strong> DHT22, DS18B20, ADC (soil moisture)</li>
                <li><strong>Port B (GREEN):</strong> DHT22, DS18B20, AM2320, I2C Sensors</li>
                <li><strong>I2C Sensors:</strong> BME280, BMP280, BME680, MAX44009, AHT20</li>
            </ul>
        </div>
        <div class="alert alert-info" role="alert">
            <strong>BME680 Note:</strong> If using BME680 for <strong>air quality monitoring (IAQ, eCO2, bVOC)</strong>, <strong>do not enable deep sleep mode</strong>. The air quality algorithm requires continuous operation (30+ minutes) to calibrate properly. Temperature, humidity, and pressure readings work fine in all modes. The BME680 provides <strong>CO2 equivalent (eCO2)</strong> estimates based on VOC levels, not actual CO2 measurements.
        </div>
        <hr>
        
        <h5 class="card-title">Step 2: Data Serving Configuration</h5>
        <p class="card-text">Select your preferred data serving mode at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">Data serving settings</a>.</p>
        <div class="card mb-3">
            <div class="card-body">
                <h6 class="card-subtitle mb-2 text-muted">Available Modes:</h6>
                <ul>
                    <li><strong>HTTP GET:</strong> Send data via URL parameters (e.g., ThingSpeak, custom endpoints)</li>
                    <li><strong>HTTP POST:</strong> Send JSON data to REST APIs</li>
                    <li><strong>MQTT:</strong> Publish to MQTT broker with custom topics</li>
                    <li><strong>Home Assistant:</strong> Auto-discovery integration with HA</li>
                </ul>
                <h6 class="card-subtitle mb-2 text-muted mt-3">Frequency Settings:</h6>
                <ul>
                    <li><strong>High Frequency (10-60s):</strong> Real-time monitoring, higher power consumption</li>
                    <li><strong>Medium Frequency (1-5 min):</strong> Balanced performance and battery life</li>
                    <li><strong>Low Frequency (5-30 min):</strong> Extended battery life, suitable for slow-changing environments</li>
                </ul>
            </div>
        </div>
        <hr>
        
        <h5 class="card-title">Step 3: Power Management</h5>
        <p class="card-text">Configure power settings at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">System settings</a>.</p>
        <div class="card mb-3">
            <div class="card-body">
                <h6 class="card-subtitle mb-2 text-muted">Sleep Modes:</h6>
                <ul>
                    <li><strong>Normal Mode:</strong> Always on, required for BME680 air quality features (IAQ, eCO2, bVOC)</li>
                    <li><strong>Light Sleep:</strong> Reduced power consumption, maintains WiFi connection</li>
                    <li><strong>Deep Sleep:</strong> Lowest power consumption (~20µA), full restart between readings
                        <ul>
                            <li>⚠️ Not compatible with BME680 air quality readings (IAQ, eCO2, bVOC)</li>
                            <li>✓ BME680 temperature, humidity, and pressure work fine</li>
                            <li>⚠️ Configuration mode disabled during sleep</li>
                            <li>✓ Best for battery-powered deployments</li>
                        </ul>
                    </li>
                </ul>
                <div class="alert alert-success" role="alert">
                    <strong>Battery Life Estimates:</strong>
                    <ul class="mb-0">
                        <li>Deep Sleep (30 min interval): ~6-12 months on 2000mAh battery</li>
                        <li>Light Sleep (5 min interval): ~1-2 months on 2000mAh battery</li>
                        <li>Normal Mode: ~1-2 weeks on 2000mAh battery</li>
                    </ul>
                </div>
            </div>
        </div>
        <hr>
        
        <h5 class="card-title">Step 4: Go Live</h5>
        <p class="card-text">Once configured, <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">disable configuration mode</a> to start serving data.</p>
        <div class="alert alert-warning" role="alert">
            <strong>Before Going Live:</strong>
            <ul class="mb-0">
                <li>✓ Verify sensor readings on the <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'main', '#right-content');">Main page</a></li>
                <li>✓ Test data serving endpoint (check your server/broker)</li>
                <li>✓ Confirm sleep mode compatibility with your sensors</li>
                <li>✓ Note: Device will reboot when disabling config mode</li>
            </ul>
        </div>
        <hr>
        
        <h5 class="card-title">Advanced Features</h5>
        <div class="card mb-3">
            <div class="card-body">
                <h6 class="card-subtitle mb-2 text-muted">Scenarios (Automation):</h6>
                <p>Create up to 3 automation rules at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'scenarios', '#right-content');">Scenarios settings</a>:</p>
                <ul>
                    <li>Trigger HTTP requests based on sensor thresholds</li>
                    <li>Control GPIO outputs (relay, LED, etc.)</li>
                    <li>Example: Send alert when temperature exceeds 25°C</li>
                </ul>
                
                <h6 class="card-subtitle mb-2 text-muted mt-3">Calibration:</h6>
                <p>Fine-tune sensor readings at <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">Data serving settings</a>:</p>
                <ul>
                    <li>Adjust temperature offset (±10°C)</li>
                    <li>Adjust humidity offset (±20%)</li>
                    <li>Adjust pressure offset (±50 hPa)</li>
                </ul>
                
                <h6 class="card-subtitle mb-2 text-muted mt-3">Available Sensor Data:</h6>
                <p>Use these placeholders in your HTTP/MQTT messages:</p>
                <ul>
                    <li><code>%temp%</code> - Temperature (°C)</li>
                    <li><code>%temp_imp%</code> - Temperature (°F)</li>
                    <li><code>%humi%</code> - Humidity (%RH)</li>
                    <li><code>%qfe%</code> - Atmospheric pressure (hPa)</li>
                    <li><code>%dew%</code> - Dew point (°C)</li>
                    <li><code>%hi%</code> - Heat index (°C)</li>
                    <li><code>%ah%</code> - Absolute humidity (g/m³)</li>
                    <li><code>%cr%</code> - Comfort ratio (%)</li>
                    <li><code>%iaq%</code> - Indoor air quality (BME680)</li>
                    <li><code>%eco2%</code> - CO2 equivalent (ppm, BME680)</li>
                    <li><code>%bvoc%</code> - VOC equivalent (BME680)</li>
                    <li><code>%lux%</code> - Ambient light (Lux)</li>
                    <li><code>%adc%</code> - ADC value (soil moisture)</li>
                </ul>
            </div>
        </div>
        <hr>
        
        <h5 class="card-title">Troubleshooting</h5>
        <div class="accordion" id="troubleshootingAccordion">
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingOne">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseOne">
                        Device won't connect to WiFi
                    </button>
                </h2>
                <div id="collapseOne" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li>Hold MODE button for 20 seconds during reset for factory reset</li>
                            <li>Connect to TeHyBug WiFi network (password: TeHyBug123)</li>
                            <li>Reconfigure WiFi credentials at http://192.168.4.1/</li>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingTwo">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseTwo">
                        Sensor readings are incorrect
                    </button>
                </h2>
                <div id="collapseTwo" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li>Check sensor connection to audio jack</li>
                            <li>Verify correct sensor type is enabled in settings</li>
                            <li>Use calibration offsets to fine-tune readings</li>
                            <li>For BME680: Allow 30+ minutes for calibration in normal mode</li>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingThree">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseThree">
                        Data not being sent to server/broker
                    </button>
                </h2>
                <div id="collapseThree" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li>Verify internet connectivity (check WiFi signal strength)</li>
                            <li>Test endpoint URL in browser or API client</li>
                            <li>Check MQTT broker credentials and port settings</li>
                            <li>Ensure configuration mode is disabled</li>
                            <li>Review data frequency settings (may need to wait for next transmission)</li>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingFour">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseFour">
                        BME680 air quality readings stuck at baseline
                    </button>
                </h2>
                <div id="collapseFour" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li><strong>Disable deep sleep mode</strong> - Air quality algorithm (IAQ, eCO2, bVOC) requires continuous operation</li>
                            <li>Temperature, humidity, and pressure readings work in all modes</li>
                            <li>Wait 30-60 minutes for initial calibration to complete</li>
                            <li>Check IAQ accuracy indicator (should reach 3 for full calibration)</li>
                            <li>eCO2 and bVOC are <strong>estimates</strong> based on VOC, not actual CO2 measurements</li>
                            <li>Ensure sensor is in a well-ventilated area during calibration</li>
                            <li>Calibration state is saved to SPIFFS every 6 hours</li>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingFive">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseFive">
                        Device keeps rebooting
                    </button>
                </h2>
                <div id="collapseFive" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li>Check power supply (use quality USB adapter, min 500mA)</li>
                            <li>Verify USB cable is not damaged</li>
                            <li>Disable deep sleep if causing issues</li>
                            <li>Perform factory reset if firmware is corrupted</li>
                            <li>Update to latest firmware via OTA</li>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="accordion-item">
                <h2 class="accordion-header" id="headingSix">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#collapseSix">
                        Cannot access web interface
                    </button>
                </h2>
                <div id="collapseSix" class="accordion-collapse collapse" data-bs-parent="#troubleshootingAccordion">
                    <div class="accordion-body">
                        <ul>
                            <li>Ensure device is in configuration mode (LED should indicate config mode)</li>
                            <li>Try accessing via IP address instead of http://tehybug.local/</li>
                            <li>Check if mDNS is supported on your network/device</li>
                            <li>Disable deep sleep mode to access configuration</li>
                            <li>Connect directly to TeHyBug AP if all else fails</li>
                        </ul>
                    </div>
                </div>
            </div>
        </div>
        <hr>
        
        <h5 class="card-title">Additional Resources</h5>
        <p class="card-text">More information about TeHyBug can be found at:</p>
        <ul>
            <li><a href="https://tehybug.com" target="_blank">tehybug.com</a> - Official website</li>
            <li><a href="https://www.tindie.com/stores/gumslone/" target="_blank">Tindie Store</a> - Purchase hardware</li>
            <li><a href="https://github.com/gumslone/tehybug" target="_blank">GitHub Repository</a> - Source code and documentation</li>
        </ul>
        
        <div class="alert alert-info" role="alert">
            <strong>Need Help?</strong> Visit the GitHub repository for detailed documentation, examples, and community support.
        </div>
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