#pragma once

// Configuration page served by the device at /config.
// Self-contained: no external assets, so it works offline and in AP mode.
// It pulls live readings from GET / and the saved settings from
// GET /config.json (both served by this firmware).
const char configPage[] PROGMEM = R"=====(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>TeHyBug CO2</title>
<style>
:root{
  --bg:#0e1416;--panel:#16201f;--edge:#24332f;--ink:#d7e4dd;--dim:#7d948a;
  --green:#3ddc84;--yellow:#ffc233;--red:#ff5d5d;--blue:#58b6ff;
}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--ink);
  font:15px/1.55 system-ui,-apple-system,"Segoe UI",sans-serif}
.wrap{max-width:680px;margin:0 auto;padding:20px 16px 48px}
header{display:flex;align-items:baseline;gap:10px;margin:8px 0 4px}
header h1{font-size:22px;margin:0;letter-spacing:.5px}
header h1 b{color:var(--green)}
#fw{color:var(--dim);font-size:12px}
.tagline{color:var(--dim);margin:0 0 18px;font-size:13px}
section{background:var(--panel);border:1px solid var(--edge);
  border-radius:10px;padding:16px 18px;margin:0 0 16px}
h2{font-size:13px;text-transform:uppercase;letter-spacing:1.2px;
  color:var(--dim);margin:0 0 12px}
/* live readings */
.tiles{display:grid;grid-template-columns:repeat(auto-fill,minmax(140px,1fr));gap:10px}
.tile{background:var(--bg);border:1px solid var(--edge);border-radius:8px;padding:10px 12px}
.tile .k{font-size:11px;color:var(--dim);text-transform:uppercase;letter-spacing:.8px}
.tile .v{font-size:22px;font-weight:600;font-variant-numeric:tabular-nums}
.tile .u{font-size:12px;color:var(--dim);margin-left:2px}
#co2tile .v{transition:color .3s}
/* traffic light legend */
.legend{display:flex;gap:14px;flex-wrap:wrap;margin-top:12px;font-size:12.5px;color:var(--dim)}
.dot{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:5px;vertical-align:baseline}
/* oled mock */
.oled{background:#000;border:6px solid #1b2422;border-radius:6px;width:256px;height:128px;
  margin:4px auto 8px;position:relative;font:12px/1 "Courier New",monospace;color:#cfe8ff;
  padding:6px 0 0 64px;box-shadow:0 0 18px rgba(88,182,255,.08) inset}
.oled div{height:24px}
.oled .wifi{position:absolute;left:190px;top:6px}
.oledcap{color:var(--dim);font-size:12px;text-align:center;margin:0}
/* form */
.opt{display:flex;gap:10px;align-items:flex-start;padding:10px 0;border-top:1px solid var(--edge)}
.opt:first-of-type{border-top:none}
.opt input{margin-top:5px;accent-color:var(--green)}
.opt b{display:block;font-size:14.5px}
.opt p{margin:2px 0 0;color:var(--dim);font-size:13px}
button{background:var(--green);border:none;color:#06130c;font-weight:700;
  font-size:14px;padding:9px 22px;border-radius:7px;cursor:pointer;margin-top:12px}
button:hover{filter:brightness(1.1)}
#savemsg{margin-left:10px;font-size:13px}
/* tables / cheat sheet */
table{width:100%;border-collapse:collapse;font-size:13.5px}
td,th{text-align:left;padding:7px 8px;border-top:1px solid var(--edge);vertical-align:top}
th{color:var(--dim);font-weight:600;border-top:none;font-size:12px;
  text-transform:uppercase;letter-spacing:.8px}
td:first-child{white-space:nowrap;color:var(--blue);font-weight:600}
code{background:var(--bg);border:1px solid var(--edge);border-radius:4px;
  padding:1px 5px;font-size:12.5px}
a{color:var(--blue)}
ul{margin:6px 0;padding-left:20px}
li{margin:4px 0}
.note{color:var(--dim);font-size:13px}
</style>
</head>
<body>
<div class="wrap">

<header><h1>TeHy<b>Bug</b> CO2</h1><span id="fw"></span></header>
<p class="tagline">Fresh air makes sense &mdash; CO2, temperature, humidity &amp; pressure monitor</p>

<section>
<h2>Live readings</h2>
<div class="tiles" id="tiles"><div class="note">Waiting for sensor data&hellip;</div></div>
<div class="legend">
  <span><span class="dot" style="background:var(--green)"></span>good &le; 1000 ppm</span>
  <span><span class="dot" style="background:var(--yellow)"></span>ventilate &gt; 1000 ppm</span>
  <span><span class="dot" style="background:var(--red)"></span>bad air &gt; 1500 ppm</span>
</div>
<p class="note" style="margin-bottom:0">The RGB LED on the device shows the same
traffic light. Readings refresh every 5&nbsp;seconds, matching the sensor
measurement interval.</p>
</section>

<section id="oledsec" style="display:none">
<h2>Display preview</h2>
<div class="oled">
  <div id="ol1"></div><div id="ol2"></div><div id="ol3"></div><div id="ol4"></div>
  <span class="wifi" id="olwifi">^</span>
</div>
<p class="oledcap">Live copy of the OLED. The <code>^</code> in the top right
means WiFi mode is on; it disappears in offline mode.</p>
</section>

<section>
<h2>Settings</h2>
<form id="cfg">
<label class="opt">
  <input type="checkbox" name="imperial_temp" id="imperial_temp">
  <span><b>Temperature in Fahrenheit</b>
  <p>Shows &deg;F instead of &deg;C on the OLED display. The JSON API and
  Home&nbsp;Assistant always get both: <code>temp</code> in &deg;C and
  <code>temp_imp</code> in &deg;F.</p></span>
</label>
<label class="opt">
  <input type="checkbox" name="imperial_qfe" id="imperial_qfe">
  <span><b>Pressure in imperial units</b>
  <p>Stores the imperial-pressure preference (<code>imperial_qfe</code>) in the
  device configuration for clients that read it. The OLED always shows
  hPa.</p></span>
</label>
<label class="opt">
  <input type="checkbox" name="scd40_single_shot" id="scd40_single_shot">
  <span><b>SCD4x single-shot mode (not recommended)</b>
  <p>Requests one measurement at a time from the SCD4x CO2 sensor instead of
  its continuous 5-second mode. Only relevant on SCD4x hardware. The firmware
  currently forces periodic mode at boot, so this switch only affects how
  fresh data is requested between readings.</p></span>
</label>
<button type="submit">Save</button><span id="savemsg"></span>
</form>
</section>

<section>
<h2>Hardware buttons</h2>
<table>
<tr><th>Button</th><th>Action</th><th>What it does</th></tr>
<tr><td>Mode (IO0)</td><td>hold 15&nbsp;s</td>
<td>Factory reset: erases the WiFi configuration and reboots. The device then
opens its own <i>TEHYBUG-CO2-&hellip;</i> setup network again.</td></tr>
<tr><td>IO14 (right)</td><td>hold 1&nbsp;s</td>
<td>Starts SenseAir&nbsp;S8 calibration (S8 hardware only). Put the device
outside in fresh air first &mdash; it waits 6&nbsp;minutes, then calibrates
to the outdoor CO2 level.</td></tr>
<tr><td>IO5 (left)</td><td>hold during power-on</td>
<td>Toggles offline mode. Keep it held while the device boots until the LED
turns pink; offline mode disables WiFi completely and the device becomes a
display-only meter.</td></tr>
<tr><td>Reset</td><td>press</td>
<td>Reboots the device. Combine with Mode held down to enter the serial
flashing mode.</td></tr>
</table>
</section>

<section>
<h2>About this device</h2>
<ul>
<li><b>Sensors:</b> auto-detected at boot &mdash; SCD4x or SenseAir&nbsp;S8
for CO2, plus optional AHT20 (temperature/humidity), BMP280/BME280
(temperature/pressure) and SSD1306 OLED on the 3.5&nbsp;mm jack.</li>
<li><b>JSON API:</b> <a href="/">GET /</a> returns all current readings,
detected I2C devices and the IP address.</li>
<li><b>Home Assistant:</b> with an MQTT broker configured, the device
announces itself via MQTT autodiscovery and publishes its state every
30&nbsp;seconds.</li>
<li><b>Firmware update:</b> upload a new <code>.bin</code> at
<a href="/update">/update</a> (user <code>TeHyBug</code>, password
<code>FreshAirMakesSense</code>).</li>
<li><b>mDNS:</b> reachable as <code>tehybug.local</code> on networks with
mDNS support.</li>
</ul>
</section>

</div>
<script>
var UNITS={co2:"ppm",eco2:"ppm",temp:"°C",temp2:"°C",
  temp_imp:"°F",temp2_imp:"°F",humi:"%",humi2:"%",qfe:"hPa",
  alt:"m",iaq:"",air:"kΩ"};
var NAMES={co2:"CO2",eco2:"eCO2",temp:"Temperature",temp2:"Temperature 2",
  temp_imp:"Temperature",temp2_imp:"Temperature 2",humi:"Humidity",
  humi2:"Humidity 2",qfe:"Pressure",alt:"Altitude",iaq:"Air quality",
  air:"Gas resistance"};
var ORDER=["co2","temp","temp_imp","humi","qfe","alt","temp2","temp2_imp",
  "humi2","iaq","eco2","air"];
var imperial=false,offline=false;

function co2color(v){
  return v>1500?"var(--red)":v>1000?"var(--yellow)":"var(--green)";
}
function esc(s){return String(s).replace(/[&<>]/g,function(c){
  return {"&":"&amp;","<":"&lt;",">":"&gt;"}[c];});}

function render(d){
  var html="",shown=0;
  ORDER.forEach(function(k){
    if(!(k in d))return;
    if(k==="temp"&&imperial&&"temp_imp" in d)return;
    if(k==="temp_imp"&&!imperial)return;
    if(k==="temp2"&&imperial&&"temp2_imp" in d)return;
    if(k==="temp2_imp"&&!imperial)return;
    var style=k==="co2"?' style="color:'+co2color(+d[k])+'"':"";
    html+='<div class="tile"'+(k==="co2"?' id="co2tile"':'')+'><div class="k">'
      +NAMES[k]+'</div><div class="v"'+style+'>'+esc(d[k])
      +'<span class="u">'+UNITS[k]+'</span></div></div>';
    shown++;
  });
  if(shown)document.getElementById("tiles").innerHTML=html;
  renderOled(d);
}
function renderOled(d){
  // mirrors update_display() in the firmware
  var l1="",l2="",l3="",l4="";
  if("co2" in d)l1="CO2: "+d.co2;
  if("temp" in d)l2="T: "+(imperial&&"temp_imp" in d?d.temp_imp+"°F":d.temp+"°C");
  if("humi" in d)l3="RH: "+d.humi+"%";
  if("qfe" in d)l4="P: "+d.qfe+"hPa";
  if(!l1&&!l2&&!l3){l1="Reading";l2="sensors";l3="...";}
  document.getElementById("ol1").textContent=l1;
  document.getElementById("ol2").textContent=l2;
  document.getElementById("ol3").textContent=l3;
  document.getElementById("ol4").textContent=l4;
  document.getElementById("olwifi").style.display=offline?"none":"";
}
function poll(){
  fetch("/").then(function(r){return r.json();}).then(render)
    .catch(function(){});
}
function loadConfig(){
  fetch("/config.json").then(function(r){return r.json();}).then(function(c){
    ["imperial_temp","imperial_qfe","scd40_single_shot"].forEach(function(k){
      if(k in c)document.getElementById(k).checked=!!c[k];
    });
    imperial=!!c.imperial_temp;
    if(c.version)document.getElementById("fw").textContent="firmware "+c.version;
    if(c.oled)document.getElementById("oledsec").style.display="";
  }).catch(function(){});
}
document.getElementById("cfg").addEventListener("submit",function(e){
  e.preventDefault();
  var form=e.target,body=[];
  ["imperial_temp","imperial_qfe","scd40_single_shot"].forEach(function(k){
    if(form[k].checked)body.push(k+"=on");
  });
  fetch("/config",{method:"POST",
    headers:{"Content-Type":"application/x-www-form-urlencoded"},
    body:body.join("&")})
  .then(function(r){return r.text();})
  .then(function(t){
    imperial=form.imperial_temp.checked;
    var m=document.getElementById("savemsg");
    m.textContent=t;m.style.color="var(--green)";
    setTimeout(function(){m.textContent="";},4000);
  })
  .catch(function(){
    var m=document.getElementById("savemsg");
    m.textContent="Save failed - is the device reachable?";
    m.style.color="var(--red)";
  });
});
loadConfig();poll();setInterval(poll,5000);
</script>
</body>
</html>
)=====";
