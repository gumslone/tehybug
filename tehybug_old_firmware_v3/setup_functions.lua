function unescape(s)
	s = string.gsub(s, "+", " ")
	s = string.gsub(s, "%%(%x%x)", function (h)
		return string.char(tonumber(h, 16))
	end)
	return s
end

function trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end


function replace_config(s,f)
	
	for k,v in pairs(system_config) do 
		if k=='server_url' then
			v = string.gsub(v, '%%', '%%%%')
		end
		s = string.gsub(s, "system_config."..k, v)
	end
	
	if config ~= nil and (f=='configuration_default.html' or f=='configuration_custom_post.html' or f=='configuration_custom_get.html' or f=='configuration_custom_mqtt.html' or f=='configuration_custom_web.html' or f=='configuration_display_only.html') then
		for k,v in pairs(config) do 
			if k=='mqtt_message' or k=='web_output' or k=='post_content' then
				v = string.gsub(v, '"', '&quot;')
			end
			if k=='mqtt_message' or k=='server_url' or k=='web_output' or k=='post_content' then
				v = string.gsub(v, '%%', '%%%%')
			end
			s = string.gsub(s, "config."..k, 'value="'..v..'"',1)
		end
	end
	
	if display_config ~= nil and f=="configuration_display.html" then
		for k,v in pairs(display_config) do 
			if k=='line1' or k=='line2' or k=='line3' then
				v = string.gsub(v, '%%', '%%%%')
			end
			s = string.gsub(s, "display_config."..k, 'value="'..v..'"',1)
		end
	end
	if out ~= nil and out ~= "" then
		 msg = msg..'<code>'..out..'</code>'
	end
	s = string.gsub(s, '-message-', '->'..msg..'<!-',1)
	
	if f == 'node_info.html' then
		local majorVer, minorVer, devVer, chipid, flashid, flashsize, flashmode, flashspeed = node.info()
		s = string.gsub(s, "node_info_majorVer", majorVer,1)
		s = string.gsub(s, "node_info_minorVer", minorVer,1)
		s = string.gsub(s, "node_info_devVer", devVer,1)
		s = string.gsub(s, "node_info_chipid", chipid,1)
		s = string.gsub(s, "node_info_flashid", flashid,1)
		s = string.gsub(s, "node_info_flashsize", flashsize,1)
		s = string.gsub(s, "node_info_flashmode", flashmode,1)
		s = string.gsub(s, "node_info_flashspeed", flashspeed,1)
	end
	
	return(s)
end

function replace_html(s)
	s = string.gsub(s, "<", "&lt")
	s = string.gsub(s, ">", "&gt")
	return(s)
end

function generate_sensor_data_page()
	file.remove("sensor_data.html")
	file.open("sensor_data.html", "w")
	file.writeline('<html><head><script>var timeleft = 10;var downloadTimer = setInterval(function(){document.getElementById("progressBar").value = 10 - --timeleft;if(timeleft <= 0){clearInterval(downloadTimer);}},1000);</script><meta http-equiv="refresh" content="10"><title>Sensor data</title></head><body><h3>Sensor data</h3><div><progress value="0" max="10" id="progressBar"></progress></div>')
	if temp ~= nil and temp ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Temperature:</b><br>'..temp..' &deg;C / '..temp_imp..' &deg;F</div>')
	end
	if humi ~= nil and humi ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Humidity:</b><br>'..humi..' %</div>')
	end
	if dew ~= nil and dew ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Dew point:</b><br>'..dew..' &deg;C / '..dew_imp..' &deg;F</div>')
	end
	if qfe ~= nil and qfe ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Barometric air pressure:</b><br>'..qfe..' hPa / '..qfe_imp..' inHg</div>')
	end
	if air ~= nil and air ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Air quality:</b><br>'..air..' kOhms</div>')
	end
	if uv ~= nil and uv ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>UV light level:</b><br>'..uv..'</div>')
	end
	if lux ~= nil and lux ~= '' then
		file.writeline('<div style="margin-top: 15px;"><b>Ambient Light luminance:</b><br>'..lux..' Lux</div>')
	end
	
	file.writeline('<div style="margin-top: 15px;"><a href="/sensor_data.json">sensor_data.json</a><br><a href="/">&lt; Main Menu</a></div><link rel="stylesheet" href="/style.css.gz"></body></html>')
	file.close()
	
	file.remove("sensor_data.json")
	file.open("sensor_data.json", "w")
	file.writeline('{')
	if temp ~= nil and temp ~= '' then
		file.writeline('"temp":'..temp..', "temp_imp":'..temp_imp..',')
	end
	if humi ~= nil and humi ~= '' then
		file.writeline('"humi":'..humi..',')
	end
	if dew ~= nil and dew ~= '' then
		file.writeline('"dew":'..dew..', "dew_imp":'..dew_imp..',')
	end
	if qfe ~= nil and qfe ~= '' then
		file.writeline('"qfe":'..qfe..', "qfe_imp":'..qfe_imp..',')
	end
	if air ~= nil and air ~= '' then
		file.writeline('"air":'..air..',')
	end
	if soil ~= nil and soil ~= '' then
		file.writeline('"soil":'..soil..',')
	end
	if uv ~= nil and uv ~= '' then
		file.writeline('"uv":'..uv..'')
	end
	file.writeline('}')
	file.close()
end
