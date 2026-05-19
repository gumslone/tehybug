<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">TeHyBug Cloud Service Settings</h1>
</div>

<div class="card mb-4">
    <div class="card-header">
        Instructions
    </div>
    <div class="card-body">
        <p>This is the easiest way to start using your TeHyBug Device.</p>
        <ol>
            <li>Create an account at <a href="https://tehybug.com" target="_blank">TeHyBug.com cloud service</a>.</li>
            <li>Remember/copy your TeHyBug key: <code id="key">Loading...</code>. You will need to provide it in your account on the cloud service.</li>
            <li>Click the "Save Config" button below. Your device will restart and begin sending data to your account.</li>
        </ol>

        <div class="form-group" style="display:none;">
            <input type="url" class="form-control" id="httpGetURL" minlength="7" value="http://tehybug.com/track/?bug_key=%key%">
            <input type="number" class="form-control dont-change" id="httpGetFrequency" value="900">
            <input type="checkbox" class="form-check-input dont-change" id="httpGetActive" checked>
            <input type="checkbox" class="form-check-input dont-change" id="sleepModeActive" checked>
            <input type="checkbox" class="form-check-input dont-change" id="lightSleepModeActive">
            <input type="checkbox" class="form-check-input dont-change" id="configModeActive">
            <input type="checkbox" class="form-check-input dont-change" id="mqttActive">
            <input type="checkbox" class="form-check-input dont-change" id="httpPostActive">
            <input type="checkbox" class="form-check-input dont-change" id="reboot" checked>
        </div>
    </div>
</div>

<div class="card mb-4">
    <div class="card-header">
        API Endpoint
    </div>
    <div class="card-body">
        <p class="card-text mb-1">TeHyBug.com HTTP GET URL:</p>
        <code>http://tehybug.com/track/?bug_key=%key%</code>
    </div>
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


<!-- Cloud Service Benefits -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-info">
            <div class="card-header bg-info text-white">
                <h4 class="mb-0"><span data-feather="cloud"></span> Why Use TeHyBug Cloud Service?</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="zap"></span> Quick Setup</h5>
                        <ul class="small">
                            <li>No server configuration needed</li>
                            <li>No MQTT broker setup</li>
                            <li>Works out of the box</li>
                            <li>One-click activation</li>
                        </ul>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="bar-chart-2"></span> Data Visualization</h5>
                        <ul class="small">
                            <li>Historical data charts</li>
                            <li>Temperature trends</li>
                            <li>Humidity graphs</li>
                            <li>Exportable data</li>
                        </ul>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="smartphone"></span> Remote Access</h5>
                        <ul class="small">
                            <li>Access from anywhere</li>
                            <li>Mobile-friendly interface</li>
                            <li>No port forwarding</li>
                            <li>Secure connection</li>
                        </ul>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="bell"></span> Alerts & Notifications</h5>
                        <ul class="small">
                            <li>Email notifications</li>
                            <li>Telegram messenger notifications</li>
                            <li>Custom thresholds</li>
                            <li>Multi-device support</li>
                        </ul>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- How It Works -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-primary">
            <div class="card-header bg-primary text-white">
                <h4 class="mb-0"><span data-feather="info"></span> How It Works</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-6 mb-3">
                        <h5>Data Flow</h5>
                        <ol class="small">
                            <li><strong>Device reads sensors</strong> - Every 15 minutes (900 seconds)</li>
                            <li><strong>Data is sent via HTTP GET</strong> - To TeHyBug.com servers</li>
                            <li><strong>Cloud processes data</strong> - Stores and analyzes readings</li>
                            <li><strong>You access dashboard</strong> - View data from any device</li>
                        </ol>
                        <div class="alert alert-info small mb-0">
                            <strong>Your Device Key:</strong> <code class="key">Loading...</code><br>
                            This unique identifier links your device to your cloud account.
                        </div>
                    </div>
                    
                    <div class="col-lg-6 mb-3">
                        <h5>Power Optimization</h5>
                        <p class="small">Cloud mode automatically enables Deep Sleep for maximum battery life:</p>
                        <ul class="small">
                            <li><strong>15-minute intervals:</strong> Balanced between data freshness and power</li>
                            <li><strong>Deep Sleep enabled:</strong> Device sleeps between transmissions</li>
                            <li><strong>Battery life:</strong> Up to 2-3 months on 2000mAh battery</li>
                            <li><strong>Wake-up process:</strong> Connect → Read → Send → Sleep</li>
                        </ul>
                        <div class="alert alert-warning small mb-0">
                            <strong>Note:</strong> Web interface is unavailable during sleep. Device wakes every 15 minutes.
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Setup Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-success">
            <div class="card-header bg-success text-white">
                <h4 class="mb-0"><span data-feather="check-square"></span> Step-by-Step Setup</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-4 mb-3">
                        <div class="card h-100 border-primary">
                            <div class="card-header bg-primary text-white">
                                <h5 class="mb-0">Step 1: Create Account</h5>
                            </div>
                            <div class="card-body">
                                <ol class="small">
                                    <li>Visit <a href="https://tehybug.com" target="_blank">TeHyBug.com</a></li>
                                    <li>Click "Sign Up" or "Register"</li>
                                    <li>Enter your email and password</li>
                                    <li>Verify your email address</li>
                                    <li>Log in to your account</li>
                                </ol>
                                <div class="alert alert-info small mb-0">
                                    <strong>Free Account:</strong> Includes basic features and data storage
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <div class="col-lg-4 mb-3">
                        <div class="card h-100 border-warning">
                            <div class="card-header bg-warning text-dark">
                                <h5 class="mb-0">Step 2: Add Device</h5>
                            </div>
                            <div class="card-body">
                                <ol class="small">
                                    <li>Go to "Devices" or "Add Device"</li>
                                    <li>Click "Add New Device"</li>
                                    <li>Enter your device key: <code class="key">Loading...</code></li>
                                    <li>Give your device a name (e.g., "Living Room")</li>
                                    <li>Save the device</li>
                                </ol>
                                <div class="alert alert-warning small mb-0">
                                    <strong>Important:</strong> Copy your device key before saving config
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <div class="col-lg-4 mb-3">
                        <div class="card h-100 border-success">
                            <div class="card-header bg-success text-white">
                                <h5 class="mb-0">Step 3: Activate</h5>
                            </div>
                            <div class="card-body">
                                <ol class="small">
                                    <li>Click "Save Config" button above</li>
                                    <li>Wait 12 seconds for restart</li>
                                    <li>Device will enter Deep Sleep mode</li>
                                    <li>First data arrives in ~15 minutes</li>
                                    <li>Check your cloud dashboard</li>
                                </ol>
                                <div class="alert alert-success small mb-0">
                                    <strong>Success:</strong> Data will appear in your cloud account automatically
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Technical Details -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-secondary">
            <div class="card-header bg-secondary text-white">
                <h4 class="mb-0"><span data-feather="code"></span> Technical Details</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-6 mb-3">
                        <h5>Configuration Settings</h5>
                        <table class="table table-sm">
                            <tbody>
                                <tr>
                                    <td><strong>Method</strong></td>
                                    <td>HTTP GET</td>
                                </tr>
                                <tr>
                                    <td><strong>Endpoint</strong></td>
                                    <td><code>http://tehybug.com/track/</code></td>
                                </tr>
                                <tr>
                                    <td><strong>Frequency</strong></td>
                                    <td>900 seconds (15 minutes)</td>
                                </tr>
                                <tr>
                                    <td><strong>Sleep Mode</strong></td>
                                    <td>Deep Sleep (enabled)</td>
                                </tr>
                                <tr>
                                    <td><strong>Data Format</strong></td>
                                    <td>URL parameters</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    
                    <div class="col-lg-6 mb-3">
                        <h5>URL Structure</h5>
                        <div class="bg-light p-3 rounded small">
                            <code>http://tehybug.com/track/?bug_key=%key%</code>
                        </div>
                        <p class="small mt-2">The device automatically appends sensor data:</p>
                        <ul class="small">
                            <li><code>bug_key</code> - Your unique device identifier</li>
                            <li><code>temp</code> - Temperature reading (°C)</li>
                            <li><code>humi</code> - Humidity reading (%)</li>
                            <li><code>pres</code> - Pressure (if sensor active)</li>
                            <li><code>aqi</code> - Air quality (if sensor active)</li>
                        </ul>
                        <div class="alert alert-secondary small mb-0">
                            <strong>Example:</strong><br>
                            <code class="small">http://tehybug.com/track/?bug_key=ABC123&temp=22.5&humi=55</code>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- FAQ -->
<div class="row mt-4 mb-4">
    <div class="col-md-12">
        <div class="card border-warning">
            <div class="card-header bg-warning text-dark">
                <h4 class="mb-0"><span data-feather="help-circle"></span> Frequently Asked Questions</h4>
            </div>
            <div class="card-body">
                <div class="accordion" id="faqAccordion">
                    <!-- Question 1 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#faq1">
                                <strong>Is the cloud service free?</strong>
                            </button>
                        </h2>
                        <div id="faq1" class="accordion-collapse collapse show" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p>Yes, TeHyBug.com offers a free tier that includes:</p>
                                <ul>
                                    <li>Unlimited data uploads from your device</li>
                                    <li>30 days of data history</li>
                                    <li>Basic charts and graphs</li>
                                    <li>Email / Telegram messenger notifications</li>
                                </ul>
                                <p class="mb-0">Premium plans are available for extended history, more devices, and advanced features.</p>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 2 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq2">
                                <strong>How secure is my data?</strong>
                            </button>
                        </h2>
                        <div id="faq2" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p>Your data security is important:</p>
                                <ul>
                                    <li><strong>Unique device key:</strong> Only you know your device key</li>
                                    <li><strong>Account protection:</strong> Password-protected access</li>
                                    <li><strong>Data encryption:</strong> HTTPS available for data transmission</li>
                                    <li><strong>Private by default:</strong> Your data is not shared with others</li>
                                    <li><strong>Data ownership:</strong> You can export or delete your data anytime</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 3 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq3">
                                <strong>Can I access the web interface while using cloud mode?</strong>
                            </button>
                        </h2>
                        <div id="faq3" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p><strong>No, not during sleep periods.</strong> Cloud mode enables Deep Sleep for battery efficiency:</p>
                                <ul>
                                    <li>Device is offline between 15-minute intervals</li>
                                    <li>Web interface is only accessible immediately after wake-up</li>
                                    <li>You have ~30-60 seconds to access it before next sleep</li>
                                    <li>To access settings, temporarily disable Deep Sleep or wait for wake cycle</li>
                                </ul>
                                <div class="alert alert-info small mb-0">
                                    <strong>Tip:</strong> Use the cloud dashboard for monitoring. Access local interface only when configuration changes are needed.
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 4 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq4">
                                <strong>What happens if my internet connection is lost?</strong>
                            </button>
                        </h2>
                        <div id="faq4" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p>The device handles connection issues gracefully:</p>
                                <ul>
                                    <li><strong>Retry mechanism:</strong> Device attempts to reconnect automatically</li>
                                    <li><strong>No data loss:</strong> Latest reading is sent when connection restored</li>
                                    <li><strong>Power saving:</strong> Device goes back to sleep if connection fails</li>
                                    <li><strong>Next attempt:</strong> Will try again at next scheduled interval</li>
                                </ul>
                                <div class="alert alert-warning small mb-0">
                                    <strong>Note:</strong> Historical data during offline period is not stored on device. Only the most recent reading is sent when reconnected.
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 5 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq5">
                                <strong>Can I change the data frequency from 15 minutes?</strong>
                            </button>
                        </h2>
                        <div id="faq5" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p><strong>Not in Cloud mode.</strong> The 15-minute interval is optimized for:</p>
                                <ul>
                                    <li>Balance between data freshness and battery life</li>
                                    <li>Reasonable cloud server load</li>
                                    <li>Sufficient data points for trend analysis</li>
                                </ul>
                                <p><strong>Alternative options:</strong></p>
                                <ul>
                                    <li><strong>HTTP GET mode:</strong> Configure custom frequency and endpoint</li>
                                    <li><strong>HTTP POST mode:</strong> Send to your own server with custom intervals</li>
                                    <li><strong>MQTT mode:</strong> Real-time updates to your MQTT broker</li>
                                    <li><strong>Home Assistant mode:</strong> Flexible frequency settings</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 6 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq6">
                                <strong>Where is my device key and can I change it?</strong>
                            </button>
                        </h2>
                        <div id="faq6" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p><strong>Your device key:</strong> <code class="key">Loading...</code></p>
                                <p>The device key is:</p>
                                <ul>
                                    <li>Automatically generated based on device hardware</li>
                                    <li>Unique to your specific TeHyBug device</li>
                                    <li>Permanent and cannot be changed</li>
                                    <li>Used to identify your device in the cloud</li>
                                </ul>
                                <div class="alert alert-success small mb-0">
                                    <strong>Security:</strong> Keep your device key private. Anyone with this key can view data from your device if they add it to their account.
                                </div>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 7 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq7">
                                <strong>Can I use cloud mode with other modes simultaneously?</strong>
                            </button>
                        </h2>
                        <div id="faq7" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p><strong>No, modes are mutually exclusive.</strong> When you activate cloud mode:</p>
                                <ul>
                                    <li>HTTP GET mode is enabled</li>
                                    <li>Deep Sleep is enabled</li>
                                    <li>All other modes (MQTT, HTTP POST, Home Assistant) are disabled</li>
                                    <li>This ensures optimal battery life and prevents conflicts</li>
                                </ul>
                                <p><strong>To use multiple integrations:</strong></p>
                                <ul>
                                    <li>Use HTTP POST or MQTT modes instead</li>
                                    <li>Configure your own server to forward data to multiple destinations</li>
                                    <li>Use Home Assistant with MQTT bridge to cloud services</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Question 8 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#faq8">
                                <strong>How long does the battery last in cloud mode?</strong>
                            </button>
                        </h2>
                        <div id="faq8" class="accordion-collapse collapse" data-bs-parent="#faqAccordion">
                            <div class="accordion-body small">
                                <p><strong>Battery life estimates (2000mAh battery):</strong></p>
                                <table class="table table-sm">
                                    <thead>
                                        <tr>
                                            <th>Scenario</th>
                                            <th>Expected Life</th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        <tr>
                                            <td>Cloud mode (15 min intervals)</td>
                                            <td>2-3 months</td>
                                        </tr>
                                        <tr>
                                            <td>Strong WiFi signal</td>
                                            <td>Up to 3 months</td>
                                        </tr>
                                        <tr>
                                            <td>Weak WiFi signal</td>
                                            <td>1.5-2 months</td>
                                        </tr>
                                        <tr>
                                            <td>Cold environment (&lt;10°C)</td>
                                            <td>Reduced by 20-30%</td>
                                        </tr>
                                    </tbody>
                                </table>
                                <div class="alert alert-info small mb-0">
                                    <strong>Tip:</strong> For maximum battery life, ensure strong WiFi signal and moderate temperature environment.
                                </div>
                            </div>
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
</script>