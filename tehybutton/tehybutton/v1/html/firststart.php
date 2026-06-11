<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<div class="col-md-12 row">
<h2 class="text-center">First start</h2>
<hr>
<div class="col-md-8">
    <p>
        Remember/copy your TeHyBug key: <code id="key">Loading...</code>
    </p>
    <p>
        <b>Step 1.</b><br>
        Pick how the button should report presses on the
        <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'settings', '#right-content');">Data serving settings</a>
        page: an HTTP GET request, an HTTP POST with a JSON body, MQTT, or
        any combination. Use the <code>%state%</code> placeholder for the
        kind of press (<code>click</code>, <code>double click</code>,
        <code>long click</code>) and <code>%key%</code> for the device key.
    </p>
    <p>
        <b>Step 2.</b><br>
        When everything is set,
        <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'setsystem', '#right-content');">disable config mode</a>
        on the System settings page to go live. The button will reboot, power
        itself off, and from then on only wake up when pressed: it joins your
        WiFi, sends the press, and switches itself off again.
    </p>
    <p>
        <b>Getting back here.</b><br>
        To re-enter config mode, hold the mode button while the device powers
        up (press and keep holding the mode button, then press the main
        button). The LED turns blue and this dashboard becomes reachable
        again. See <a href="javascript:void(0);" onclick="javascript:ChangeContent(this, 'about', '#right-content');">About &amp; buttons</a>
        for the full cheat sheet.
    </p>
    <p class="help-text">
        More about TeHyBug devices at tehybug.com or at the Tindie store:
        https://www.tindie.com/stores/gumslone/
    </p>
</div>
</div>

<div class="col-md-12 text-center">
    <hr>
</div>

<script>
    feather.replace();
    connectionStart();
</script>
