<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<div class="col-md-12 row">
<h2 class="text-center">About this device</h2>
<hr>

    <div class="col-md-6">
        <div class="tb-card">
            <h3>What it is</h3>
            <p>
                The TeHyButton is a battery powered WiFi button built on an
                ESP8285. It spends almost all of its life completely powered
                off: pressing the button closes a power latch, the firmware
                boots, joins your WiFi, reports the button press over HTTP
                or MQTT, and then cuts its own power again about a second
                later. There is no standby drain between presses.
            </p>
            <p>
                Each device has a unique <em>TeHyBug key</em> (a UUID derived
                from the chip id) that you can use to identify the button in
                your own services or at tehybug.com.
            </p>
        </div>

        <div class="tb-card">
            <h3>Status LED colors</h3>
            <table>
                <tr>
                    <td><span class="led led-red"></span></td>
                    <td><b>Red</b></td>
                    <td>Just powered on, booting.</td>
                </tr>
                <tr>
                    <td><span class="led led-blue"></span></td>
                    <td><b>Blue</b></td>
                    <td>WiFi setup portal is open, or config mode is active.</td>
                </tr>
                <tr>
                    <td><span class="led led-green"></span></td>
                    <td><b>Green</b></td>
                    <td>WiFi connected, sending the button press.</td>
                </tr>
                <tr>
                    <td><span class="led led-off"></span></td>
                    <td><b>Off</b></td>
                    <td>Powered down (normal state between presses).</td>
                </tr>
            </table>
        </div>

        <div class="tb-card">
            <h3>Try it: what a press does</h3>
            <p class="help-text">
                This demo uses the firmware's real timings: a double click
                must happen within <code>400 ms</code>, a long press takes
                <code>1000 ms</code>, and after sending, the device keeps
                power for about <code>1000 ms</code> (plus
                <code>1500 ms</code> for as long as the button is held)
                before switching itself off.
            </p>
            <div class="d-flex align-items-center" style="gap:1.5rem;">
                <div class="tb-demo-button" title="The button"></div>
                <span id="demoLed" class="led led-lg led-off"></span>
                <div>
                    <div id="demoState" class="help-text" style="font-size:.9rem;">Powered off — press a button below.</div>
                </div>
            </div>
            <div class="mt-3">
                <button type="button" class="btn btn-outline-success btn-sm" onclick="demoPress('click')">Click</button>
                <button type="button" class="btn btn-outline-success btn-sm" onclick="demoPress('double click')">Double click</button>
                <button type="button" class="btn btn-outline-success btn-sm" onclick="demoPress('long click')">Long press</button>
            </div>
        </div>
    </div>

    <div class="col-md-6">
        <div class="tb-card">
            <h3>Hardware buttons cheat sheet</h3>
            <table>
                <tr>
                    <td><span class="tb-kbd">press</span></td>
                    <td>Wakes the device and sends the state <code>click</code>.</td>
                </tr>
                <tr>
                    <td><span class="tb-kbd">press &times;2</span></td>
                    <td>Two presses within 0.4&nbsp;s send <code>double click</code>.</td>
                </tr>
                <tr>
                    <td><span class="tb-kbd">hold &ge;1s</span></td>
                    <td>Sends <code>long click</code>.</td>
                </tr>
                <tr>
                    <td><span class="tb-kbd">press &times;3</span></td>
                    <td>Recognized by the firmware but does <em>not</em> send anything.</td>
                </tr>
                <tr>
                    <td><span class="tb-kbd">MODE at boot</span></td>
                    <td>Holding the mode button (GPIO0) for ~0.3&nbsp;s while the
                        device powers up toggles config mode on or off. With
                        config mode on, the LED turns blue and this dashboard
                        is reachable; with it off, the button goes back to
                        live mode.</td>
                </tr>
            </table>
            <p class="help-text">
                The sent state replaces the <code>%state%</code> placeholder
                in your HTTP GET URL, HTTP POST JSON and MQTT message;
                <code>%key%</code> is replaced with the device key. Note that
                <code>double click</code> and <code>long click</code> contain
                a space.
            </p>
        </div>

        <div class="tb-card">
            <h3>WiFi setup</h3>
            <p>
                If the button cannot join a known network, it opens an access
                point named <code>TEHYBUTTON-&lt;chip id&gt;</code> with the
                password <code>TeHyBug123</code> and turns the LED blue.
                Connect to it and pick your WiFi network in the portal. The
                portal stays open for 180 seconds.
            </p>
            <p class="help-text">
                Once connected, the dashboard is reachable on the device's IP
                (port 80) and via mDNS at <code>http://tehybutton.local</code>.
                Live data on this page arrives over a websocket on port 81.
            </p>
        </div>

        <div class="tb-card">
            <h3>Firmware</h3>
            <p>
                Firmware updates can be flashed over the air from the
                <em>Firmware update</em> page (no cables needed). The running
                version is shown on the dashboard; the version number is the
                firmware's build timestamp (<code>YYMMDDHHMM</code>).
            </p>
        </div>
    </div>
</div>

<script>
    feather.replace();

    // Firmware timing constants (tehybutton.ino)
    var DEMO_AFTER_SERVE_MS = 1000;  // updateSleepAfter(1000) after serving
    var DEMO_BOOT_MS = 600;          // boot + wifi join (varies in reality)
    var demoTimers = [];

    function demoSet(ledClass, text) {
        var led = document.getElementById('demoLed');
        led.className = 'led led-lg ' + ledClass;
        document.getElementById('demoState').innerHTML = text;
    }

    function demoPress(state) {
        // cancel a running demo
        for (var i = 0; i < demoTimers.length; i++) {
            clearTimeout(demoTimers[i]);
        }
        demoTimers = [];

        demoSet('led-red', 'Power latch closed &mdash; booting, joining WiFi&hellip;');
        demoTimers.push(setTimeout(function () {
            demoSet('led-green', 'Connected &mdash; sending <code>' + state + '</code>');
        }, DEMO_BOOT_MS));
        demoTimers.push(setTimeout(function () {
            demoSet('led-off', 'Sent. Power latch opened &mdash; device is off again.');
        }, DEMO_BOOT_MS + DEMO_AFTER_SERVE_MS));
    }
</script>
