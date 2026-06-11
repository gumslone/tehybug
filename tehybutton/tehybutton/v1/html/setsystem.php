<?php include dirname(__FILE__) . '/../inc/cors.php'; ?>
<div class="col-md-12 row">

<h2 class="text-center">System settings</h2>
<hr>

    <div class="col-md-5">
        <h2 class="text-center">System</h2>
        <hr>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="skipButtonActions">
            <label class="form-check-label" for="skipButtonActions">Skip Button Actions<br>(dont serve data on button actions)</label>
        </div>
        <div class="form-check form-switch">
            <input type="checkbox" class="form-check-input" id="configModeActive">
            <label class="form-check-label" for="configModeActive">Config Mode Active<br>(disable config mode to activate live mode)</label>
        </div>
        
    </div>
    
    
</div>


<?php include dirname(__FILE__) . '/../inc/save_modal.php'; ?>

<script>
    feather.replace();
    connectionStart();    
</script>