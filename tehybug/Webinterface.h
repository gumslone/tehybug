const char mainPage[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
<script>
let xhr = new XMLHttpRequest();
xhr.open('GET', '/api/getip');
xhr.send();
xhr.onload = function() {
  if (xhr.status == 200) { 
    document.getElementById("ip").innerHTML = xhr.responseText;
  }
};
</script>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link href="https://tehybug.com/tehybug/v1/css/style.php" rel="stylesheet">
    <script src="https://tehybug.com/tehybug/v1/js/javascript.php"></script>
    <title>TeHyBug</title>
</head>
<body>
<div id="page">
Loading... 
<br>If the page doesnt load: make sure that you are connected to your local home network and then open this ip: <span id="ip">tehybug.local</span> with your browser
</div>
</body>
</html>
)=====";
