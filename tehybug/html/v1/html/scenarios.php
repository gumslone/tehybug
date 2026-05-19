<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
?>

<div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
    <h1 class="h2">Scenarios</h1>
</div>

<div class="row">
    <!-- Scenario 1 -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">Scenario 1</div>
            <div class="card-body">
                <div class="mb-3">
                    <label for="scenario1_type" class="form-label">Type</label>
                    <select class="form-select" name="scenario1_type" id="scenario1_type" aria-label="Type">
                        <option value="get">HTTP GET</option>
                        <option value="post">HTTP POST</option>
                        <option value="io13_1">IO_13 HIGH</option>
                        <option value="io13_0">IO_13 LOW</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario1_url" class="form-label">URL</label>
                    <input type="url" class="form-control" id="scenario1_url" minlength="7" placeholder="https://example.com Loading or no data" pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
                </div>
                <div class="mb-3">
                    <label for="scenario1_message" class="form-label">Post Json (HTTP POST only)</label>
                    <input type="text" class="form-control" id="scenario1_message" placeholder="Loading or no data">
                </div>
                <div class="mb-3">
                    <label for="scenario1_data" class="form-label">Data</label>
                    <select class="form-select" name="scenario1_data" id="scenario1_data" aria-label="Data">
                        <option value="temp">Temperature</option>
                        <option value="humi">Humidity</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario1_condition" class="form-label">Condition</label>
                    <select class="form-select" name="scenario1_condition" id="scenario1_condition" aria-label="Condition">
                        <option value="gt">&gt;</option>
                        <option value="lt">&lt;</option>
                        <option value="eq">=</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario1_value" class="form-label">Value</label>
                    <input type="text" class="form-control" pattern="[0-9]" id="scenario1_value" placeholder="Loading or no value set">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="scenario1_active">
                    <label class="form-check-label" for="scenario1_active">active</label>
                </div>
            </div>
        </div>
    </div>

    <!-- Scenario 2 -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">Scenario 2</div>
            <div class="card-body">
                <div class="mb-3">
                    <label for="scenario2_type" class="form-label">Type</label>
                    <select class="form-select" name="scenario2_type" id="scenario2_type" aria-label="Type">
                        <option value="get">HTTP GET</option>
                        <option value="post">HTTP POST</option>
                        <option value="io13_1">IO_13 HIGH</option>
                        <option value="io13_0">IO_13 LOW</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario2_url" class="form-label">URL</label>
                    <input type="url" class="form-control" id="scenario2_url" minlength="7" placeholder="https://example.com Loading or no data" pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
                </div>
                <div class="mb-3">
                    <label for="scenario2_message" class="form-label">Post Json (HTTP POST only)</label>
                    <input type="text" class="form-control" id="scenario2_message" placeholder="Loading or no data">
                </div>
                <div class="mb-3">
                    <label for="scenario2_data" class="form-label">Data</label>
                    <select class="form-select" name="scenario2_data" id="scenario2_data" aria-label="Data">
                        <option value="temp">Temperature</option>
                        <option value="humi">Humidity</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario2_condition" class="form-label">Condition</label>
                    <select class="form-select" name="scenario2_condition" id="scenario2_condition" aria-label="Condition">
                        <option value="gt">&gt;</option>
                        <option value="lt">&lt;</option>
                        <option value="eq">=</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario2_value" class="form-label">Value</label>
                    <input type="text" class="form-control" pattern="[0-9]" id="scenario2_value" placeholder="Loading or no value set">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="scenario2_active">
                    <label class="form-check-label" for="scenario2_active">active</label>
                </div>
            </div>
        </div>
    </div>

    <!-- Scenario 3 -->
    <div class="col-lg-4 mb-4">
        <div class="card h-100">
            <div class="card-header">Scenario 3</div>
            <div class="card-body">
                <div class="mb-3">
                    <label for="scenario3_type" class="form-label">Type</label>
                    <select class="form-select" name="scenario3_type" id="scenario3_type" aria-label="Type">
                        <option value="get">HTTP GET</option>
                        <option value="post">HTTP POST</option>
                        <option value="io13_1">IO_13 HIGH</option>
                        <option value="io13_0">IO_13 LOW</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario3_url" class="form-label">URL</label>
                    <input type="url" class="form-control" id="scenario3_url" minlength="7" placeholder="https://example.com Loading or no data" pattern="[Hh][Tt][Tt][Pp][Ss]?:\/\/(?:(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)(?:\.(?:[a-zA-Z\u00a1-\uffff0-9]+-?)*[a-zA-Z\u00a1-\uffff0-9]+)*(?:\.(?:[a-zA-Z\u00a1-\uffff]{2,}))(?::\d{2,5})?(?:\/[^\s]*)?">
                </div>
                <div class="mb-3">
                    <label for="scenario3_message" class="form-label">Post Json (HTTP POST only)</label>
                    <input type="text" class="form-control" id="scenario3_message" placeholder="Loading or no data">
                </div>
                <div class="mb-3">
                    <label for="scenario3_data" class="form-label">Data</label>
                    <select class="form-select" name="scenario3_data" id="scenario3_data" aria-label="Data">
                        <option value="temp">Temperature</option>
                        <option value="humi">Humidity</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario3_condition" class="form-label">Condition</label>
                    <select class="form-select" name="scenario3_condition" id="scenario3_condition" aria-label="Condition">
                        <option value="gt">&gt;</option>
                        <option value="lt">&lt;</option>
                        <option value="eq">=</option>
                    </select>
                </div>
                <div class="mb-3">
                    <label for="scenario3_value" class="form-label">Value</label>
                    <input type="text" class="form-control" pattern="[0-9]" id="scenario3_value" placeholder="Loading or no value set">
                </div>
                <div class="form-check form-switch">
                    <input type="checkbox" class="form-check-input" id="scenario3_active">
                    <label class="form-check-label" for="scenario3_active">active</label>
                </div>
            </div>
        </div>
    </div>
</div>


<div class="row mt-4">
    <div class="col-12 text-center">
        <hr>
        <button type="button" class="btn btn-outline-success" onclick="SaveConfig()" data-bs-toggle="modal" data-bs-target="#popup">
            <span data-feather="save"></span> Save Scenarios
        </button>
    </div>
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

<!-- Scenarios Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-info">
            <div class="card-header bg-info text-white">
                <h4 class="mb-0"><span data-feather="zap"></span> What are Scenarios?</h4>
            </div>
            <div class="card-body">
                <p>Scenarios allow your TeHyBug to automatically trigger actions when sensor readings meet specific conditions. Think of them as "if-then" rules:</p>
                <div class="alert alert-success mb-0">
                    <strong>Example:</strong> "If temperature > 25°C, then send HTTP request to turn on fan"
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Action Types -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-primary">
            <div class="card-header bg-primary text-white">
                <h4 class="mb-0"><span data-feather="settings"></span> Action Types Explained</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="download"></span> HTTP GET</h5>
                        <ul class="small">
                            <li>Sends a simple web request</li>
                            <li>Best for triggering webhooks</li>
                            <li>Data passed in URL</li>
                            <li>Example: IFTTT, Zapier triggers</li>
                        </ul>
                        <div class="bg-light p-2 rounded small">
                            <code>https://maker.ifttt.com/trigger/high_temp/with/key/YOUR_KEY</code>
                        </div>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="upload"></span> HTTP POST</h5>
                        <ul class="small">
                            <li>Sends JSON data to API</li>
                            <li>More flexible than GET</li>
                            <li>Can include sensor values</li>
                            <li>Example: Home Assistant, custom APIs</li>
                        </ul>
                        <div class="bg-light p-2 rounded small">
                            <code>{"alert":"high_temp","value":%temp%}</code>
                        </div>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="toggle-right"></span> IO_13 HIGH</h5>
                        <ul class="small">
                            <li>Sets GPIO pin 13 to HIGH (3.3V)</li>
                            <li>Can control relay, LED, etc.</li>
                            <li>Direct hardware control</li>
                            <li>No internet required</li>
                        </ul>
                        <div class="alert alert-warning small mb-0">
                            <strong>Max:</strong> 12mA @ 3.3V
                        </div>
                    </div>
                    
                    <div class="col-lg-3 mb-3">
                        <h5><span data-feather="toggle-left"></span> IO_13 LOW</h5>
                        <ul class="small">
                            <li>Sets GPIO pin 13 to LOW (0V)</li>
                            <li>Turns off relay/LED</li>
                            <li>Opposite of IO_13 HIGH</li>
                            <li>Instant local action</li>
                        </ul>
                        <div class="alert alert-info small mb-0">
                            <strong>Use:</strong> Pair with HIGH for on/off control
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Conditions Guide -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-warning">
            <div class="card-header bg-warning text-dark">
                <h4 class="mb-0"><span data-feather="filter"></span> Conditions & Data Types</h4>
            </div>
            <div class="card-body">
                <div class="row">
                    <div class="col-lg-6 mb-3">
                        <h5>Available Conditions</h5>
                        <table class="table table-sm">
                            <thead>
                                <tr>
                                    <th>Symbol</th>
                                    <th>Meaning</th>
                                    <th>Example</th>
                                </tr>
                            </thead>
                            <tbody>
                                <tr>
                                    <td><code>&gt;</code></td>
                                    <td>Greater than</td>
                                    <td>Temperature > 25 (above 25°C)</td>
                                </tr>
                                <tr>
                                    <td><code>&lt;</code></td>
                                    <td>Less than</td>
                                    <td>Humidity < 30 (below 30%)</td>
                                </tr>
                                <tr>
                                    <td><code>=</code></td>
                                    <td>Equal to</td>
                                    <td>Temperature = 20 (exactly 20°C)</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                    
                    <div class="col-lg-6 mb-3">
                        <h5>Available Data Sources</h5>
                        <ul class="small">
                            <li><strong>Temperature:</strong> From any active temperature sensor (DHT, DS18B20, BME680)</li>
                            <li><strong>Humidity:</strong> From DHT or BME680 sensors</li>
                            <li>More data types may be available based on your sensor configuration</li>
                        </ul>
                        <div class="alert alert-info small mb-0">
                            <strong>Note:</strong> Scenarios check conditions every time sensors are read (based on your data frequency settings)
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Practical Examples -->
<div class="row mt-4">
    <div class="col-md-12">
        <div class="card border-success">
            <div class="card-header bg-success text-white">
                <h4 class="mb-0"><span data-feather="book-open"></span> Practical Examples</h4>
            </div>
            <div class="card-body">
                <div class="accordion" id="examplesAccordion">
                    <!-- Example 1 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button" type="button" data-bs-toggle="collapse" data-bs-target="#example1">
                                <strong>Example 1: Temperature Alert</strong>
                            </button>
                        </h2>
                        <div id="example1" class="accordion-collapse collapse show" data-bs-parent="#examplesAccordion">
                            <div class="accordion-body">
                                <p><strong>Goal:</strong> Send notification when temperature exceeds 30°C</p>
                                <ul class="small">
                                    <li><strong>Type:</strong> HTTP GET</li>
                                    <li><strong>URL:</strong> https://maker.ifttt.com/trigger/high_temp/with/key/YOUR_KEY</li>
                                    <li><strong>Data:</strong> Temperature</li>
                                    <li><strong>Condition:</strong> &gt;</li>
                                    <li><strong>Value:</strong> 30</li>
                                    <li><strong>Active:</strong> ✓ Checked</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Example 2 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#example2">
                                <strong>Example 2: Humidity Control</strong>
                            </button>
                        </h2>
                        <div id="example2" class="accordion-collapse collapse" data-bs-parent="#examplesAccordion">
                            <div class="accordion-body">
                                <p><strong>Goal:</strong> Turn on humidifier when humidity drops below 40%</p>
                                <ul class="small">
                                    <li><strong>Type:</strong> IO_13 HIGH</li>
                                    <li><strong>URL:</strong> (not needed for GPIO)</li>
                                    <li><strong>Data:</strong> Humidity</li>
                                    <li><strong>Condition:</strong> &lt;</li>
                                    <li><strong>Value:</strong> 40</li>
                                    <li><strong>Active:</strong> ✓ Checked</li>
                                </ul>
                                <p class="small mb-0"><strong>Tip:</strong> Create a second scenario with "Humidity > 60" and "IO_13 LOW" to turn it off</p>
                            </div>
                        </div>
                    </div>
                    
                    <!-- Example 3 -->
                    <div class="accordion-item">
                        <h2 class="accordion-header">
                            <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#example3">
                                <strong>Example 3: Home Assistant Integration</strong>
                            </button>
                        </h2>
                        <div id="example3" class="accordion-collapse collapse" data-bs-parent="#examplesAccordion">
                            <div class="accordion-body">
                                <p><strong>Goal:</strong> Send data to Home Assistant when temperature changes</p>
                                <ul class="small">
                                    <li><strong>Type:</strong> HTTP POST</li>
                                    <li><strong>URL:</strong> http://homeassistant.local:8123/api/webhook/tehybug_alert</li>
                                    <li><strong>Post Json:</strong> {"sensor":"tehybug","temp":%temp%,"humi":%humi%}</li>
                                    <li><strong>Data:</strong> Temperature</li>
                                    <li><strong>Condition:</strong> &gt;</li>
                                    <li><strong>Value:</strong> 25</li>
                                    <li><strong>Active:</strong> ✓ Checked</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>

<!-- Important Notes -->
<div class="row mt-4 mb-4">
    <div class="col-md-12">
        <div class="card border-danger">
            <div class="card-header bg-danger text-white">
                <h4 class="mb-0"><span data-feather="alert-triangle"></span> Important Notes</h4>
            </div>
            <div class="card-body">
                <ul>
                    <li><strong>Trigger Frequency:</strong> Scenarios are evaluated each time sensor data is read. If your data frequency is set to 60 seconds, scenarios will check every 60 seconds.</li>
                    <li><strong>No Debouncing:</strong> If a condition remains true, the action will trigger repeatedly. Use external logic or pair scenarios (HIGH/LOW) to prevent continuous triggering.</li>
                    <li><strong>GPIO Limitations:</strong> IO_13 can only provide 12mA at 3.3V. Use a relay module or transistor for controlling high-power devices.</li>
                    <li><strong>Network Dependency:</strong> HTTP GET/POST actions require active internet connection. GPIO actions work offline.</li>
                    <li><strong>URL Validation:</strong> Ensure URLs start with http:// or https://. Invalid URLs will cause scenario failures.</li>
                    <li><strong>JSON Variables:</strong> In POST JSON, use %temp% and %humi% as placeholders. They will be replaced with actual sensor values.</li>
                    <li><strong>Active Checkbox:</strong> Remember to check "active" for each scenario you want to enable. Inactive scenarios are saved but not executed.</li>
                    <li><strong>System Restart:</strong> After saving scenarios, the system will restart. All scenarios will begin monitoring after reboot.</li>
                </ul>
                <div class="alert alert-warning mb-0">
                    <strong><span data-feather="alert-circle"></span> Safety Tip:</strong> Always test scenarios with non-critical actions first. Ensure your conditions and values are correct before connecting to important systems or devices.
                </div>
            </div>
        </div>
    </div>
</div>

<script>
    feather.replace();
    connectionStart();    
</script>