const char configPage[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>TeHyBug CO2</title>
</head>
<body>
<div id="page">
<form action="/config" method="POST">
<div>
<input type="checkbox" name="imperial_temp" id="imperial_temp"><label for="imperial_temp">Temperature in Fahrenheit instead of Celcius</label>
<br>
<input type="checkbox" name="scd40_single_shot" id="scd40_single_shot"><label for="scd40_single_shot">SCD4X sensor single shot mode (not recommended)</label>
<br>
<input type="submit" value"Save">
</div>
</form>
</div>
</body>
</html>
)=====";
