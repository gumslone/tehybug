<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<div class="col-md-12 row">

<h2 class="text-center">System settings</h2>
<hr>

    <div class="col-md-6">
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="skipButtonActions">
            <label class="form-check-label" for="skipButtonActions">Skip button actions</label>
            <span class="help-text">When enabled, pressing the button does <em>not</em> send any data.
            Useful to temporarily mute the button without losing your settings.</span>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="configModeActive">
            <label class="form-check-label" for="configModeActive">Config mode active</label>
            <span class="help-text">While config mode is on, the device stays powered and serves this
            dashboard. Turn it <em>off</em> to go live: the button reboots, powers down, and only wakes
            to send a press. To get back here, hold the mode button while the device powers up
            (the LED turns blue).</span>
        </div>
    </div>

</div>

<?php include dirname(__FILE__) . '/../inc/save_modal.php'; ?>

<script>
    feather.replace();
    connectionStart();
</script>
