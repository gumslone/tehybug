<?php
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin");
header('P3P: CP="CAO PSA OUR"'); // Makes IE to support cookies
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");

?>
<header class="navbar navbar-dark sticky-top bg-dark flex-md-nowrap p-0 shadow">
  <a class="navbar-brand col-md-3 col-lg-2 me-0 px-3" href="#">TeHyBug</a>
  <button class="navbar-toggler position-absolute d-md-none collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#sidebarMenu" aria-controls="sidebarMenu" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>

  <ul class="navbar-nav px-3"  style="margin-right:50px;">
      <li class="nav-item text-nowrap">
          <div class="text-center nav-link active">
              Connection: <span id="connectionStatus">Status Unknown</span>
          </div>
      </li>
  </ul>
</header>

<div class="container-fluid">
  <div class="row">
<!-- Sidebar Navigation -->
        <nav id="sidebarMenu" class="col-md-3 col-lg-2 d-md-block bg-light sidebar collapse">
            <div class="position-sticky pt-3">
                <ul class="nav flex-column">
                    <!-- Dashboard -->
                    <li class="nav-item">
                        <a class="nav-link active" aria-current="page" href="/">
                            <span data-feather="home"></span>
                            Dashboard
                        </a>
                    </li>

                    <li><hr class="dropdown-divider"></li>
                    
                    <!-- Quick Setup -->
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'firststart', '#right-content');">
                            <span data-feather="play"></span>
                            Quick Start Guide
                        </a>
                    </li>

                    <li><hr class="dropdown-divider"></li>
                    <li class="nav-item">
                        <small class="text-muted px-3">CLOUD SERVICES</small>
                    </li>

                    <!-- Cloud Services -->
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'cloud_settings', '#right-content');">
                            <span data-feather="cloud"></span>
                            TeHyBug Cloud
                        </a>
                    </li>
                    
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'ha_settings', '#right-content');">
                            <span data-feather="home"></span>
                            Home Assistant
                        </a>
                    </li>

                    <li><hr class="dropdown-divider"></li>
                    <li class="nav-item">
                        <small class="text-muted px-3">CONFIGURATION</small>
                    </li>

                    <!-- Configuration -->
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'setsensor', '#right-content');">
                            <span data-feather="sliders"></span>
                            Sensor Settings
                        </a>
                    </li>
                    
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'settings', '#right-content');">
                            <span data-feather="database"></span>
                            Data Serving
                        </a>
                    </li>
                    
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'setsystem', '#right-content');">
                            <span data-feather="settings"></span>
                            System Settings
                        </a>
                    </li>
                    
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContent(this, 'scenarios', '#right-content');">
                            <span data-feather="layers"></span>
                            Scenarios
                        </a>
                    </li>

                    <li><hr class="dropdown-divider"></li>
                    <li class="nav-item">
                        <small class="text-muted px-3">MAINTENANCE</small>
                    </li>

                    <!-- Maintenance -->
                    <li class="nav-item">
                        <a class="nav-link" href="javascript:void(0);" 
                           onclick="ChangeContentIframe(this, 'update', '#right-content');">
                            <span data-feather="upload"></span>
                            Firmware Update
                        </a>
                    </li>

                    <li><hr class="dropdown-divider"></li>

                    <!-- External Links -->
                    <li class="nav-item">
                        <a class="nav-link" href="https://github.com/gumslone/tehybug/tree/master/tehybug" target="_blank">
                            <span data-feather="github"></span>
                            GitHub Repository
                            <span data-feather="external-link" style="width: 14px; height: 14px;"></span>
                        </a>
                    </li>
                </ul>
            </div>
        </nav>

<main class="col-md-9 ms-sm-auto col-lg-10 px-md-4" id="right-content">
        <div class="d-flex justify-content-between flex-wrap flex-md-nowrap align-items-center pt-3 pb-2 mb-3 border-bottom">
            <h1 class="h2">Dashboard</h1>
        </div>

        <div class="row">
            <div class="col-md-12">
                <div class="card mb-4">
                    <div class="card-header">
                        Device Info
                    </div>
                    <div class="card-body">
                        <div class="table-responsive">
                            <table class="table table-striped table-sm" id="info">
                                <tbody>
                                    <tr>
                                        <td class="font-weight-bold">TeHyBug Key:</td>
                                        <td id="key">Loading...</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <div class="row">
            <div class="col-lg-3 col-md-6">
                <div class="card mb-4">
                    <div class="card-header">
                        System Info
                    </div>
                    <div class="card-body">
                        <div class="table-responsive">
                            <table class="table table-striped table-sm">
                                <tbody>
                                    <tr>
                                        <td class="font-weight-bold">Version:</td>
                                        <td id="gumboardVersion">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Sketch Size:</td>
                                        <td id="sketchSize">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Free Sketch Space:</td>
                                        <td id="freeSketchSpace">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Wifi RSSI:</td>
                                        <td id="wifiRSSI">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Wifi Quality:</td>
                                        <td id="wifiQuality">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Wifi SSID:</td>
                                        <td id="wifiSSID">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">IP-Address:</td>
                                        <td id="ipAddress">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Free Heap:</td>
                                        <td id="freeHeap">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">ChipID:</td>
                                        <td id="chipID">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">CPU Freq. in MHz:</td>
                                        <td id="cpuFreqMHz">Loading...</td>
                                    </tr>
                                    <tr>
                                        <td class="font-weight-bold">Sleep Mode:</td>
                                        <td id="sleepModeActive">Loading...</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>
            <div class="col-lg-3 col-md-6">
                <div class="card mb-4">
                    <div class="card-header">
                        Sensor
                    </div>
                    <div class="card-body">
                        <div class="table-responsive">
                            <table class="table table-striped table-sm">
                                <tbody id="sensor_data">
                                    
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
            </div>
            <div class="col-lg-6 col-md-12">
                <div class="card mb-4">
                    <div class="card-header">
                        Log
                    </div>
                    <div class="card-body">
                        <textarea class="form-control" id="log" rows="17" wrap="off" disabled="" style="font-size: small; resize: vertical;"></textarea>
                    </div>
                </div>
            </div>
        </div>

        <div class="row">
            <div class="col-md-12">
                <div class="card mb-4">
                    <div class="card-header">
                        API Endpoints
                    </div>
                    <div class="card-body">
                        <p><a href="/api/info" target="_blank">/api/info</a> - Device Info</p>
                        <p><a href="/api/sensor" target="_blank">/api/sensor</a> - Sensor Data</p>
                        <p><a href="/api/config" target="_blank">/api/config</a> - Config Data</p>
                    </div>
                </div>
            </div>
        </div>

    </main>
  </div>
</div>


<script>
      connectionStart();
      feather.replace()
</script>