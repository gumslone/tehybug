<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<header class="navbar navbar-dark sticky-top bg-dark flex-md-nowrap p-0 shadow">
  <a class="navbar-brand col-md-3 col-lg-2 me-0 px-3" href="#">TeHyButton</a>
  <button class="navbar-toggler position-absolute d-md-none collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#sidebarMenu" aria-controls="sidebarMenu" aria-expanded="false" aria-label="Toggle navigation">
    <span class="navbar-toggler-icon"></span>
  </button>

  <ul class="navbar-nav px-3" style="margin-right:50px;">
      <li class="nav-item text-nowrap">
          <div class="text-center nav-link active">
              <span id="connectionLed" class="led led-off"></span>Connection: <span id="connectionStatus">Status Unknown</span>
          </div>
      </li>
  </ul>
</header>

<div class="container-fluid">
  <div class="row">
    <nav id="sidebarMenu" class="col-md-3 col-lg-2 d-md-block bg-light sidebar collapse">
      <div class="position-sticky pt-3">
        <ul class="nav flex-column">
          <li class="nav-item">
            <a class="nav-link active" aria-current="page" href="/">
              <span data-feather="home"></span>
              Dashboard
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'firststart', '#right-content');">
              <span data-feather="play"></span>
              First start
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">
              <span data-feather="database"></span>
              Data serving settings
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">
              <span data-feather="tool"></span>
              System settings
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'about', '#right-content');">
              <span data-feather="info"></span>
              About &amp; buttons
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="javascript:void(0);" onclick="javascript:ChangeContentIframe(this, 'update', '#right-content');">
              <span data-feather="upload"></span>
              Firmware update
            </a>
          </li>
          <li class="nav-item">
            <a class="nav-link" href="https://github.com/gumslone/tehybug/tree/master/tehybutton" target="_blank">
              <span data-feather="github"></span>
              GitHub
            </a>
          </li>
        </ul>
      </div>
    </nav>

    <main class="col-md-9 ms-sm-auto col-lg-10 px-md-4" id="right-content">
    <div class="col-md-12 row">
      <div class="col-md-12">
          <h2 class="text-center">Device Info</h2>
          <hr>
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
    <div class="col-md-12 row">
      <div class="col-md-5">
          <h2 class="text-center">System Info</h2>
          <hr>
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

      <div class="col-md-7">
          <h2 class="text-center">Log</h2>
          <hr>
          <textarea class="form-control" id="log" rows="17" wrap="off" disabled="" style="font-size: small"></textarea>
      </div>
      <script>
        connectionStart();
      </script>
    </div>
    </main>
  </div>
</div>
<script>
      feather.replace()
</script>
