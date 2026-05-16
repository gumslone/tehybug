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

<script>
    feather.replace();
    connectionStart();    
</script>