local timeout=0
local first_time=1
wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)
wifi.sta.sethostname(config_system.setup_ssid) 
wifi.sta.connect()
local wifitmr = tmr.create()	
wifitmr:alarm(1000, tmr.ALARM_AUTO, function() 
	ipAddr = wifi.sta.getip()
	if ipAddr == nil or ipAddr == "0.0.0.0" then
		timeout = timeout + 1
		if timeout >= 10 then
			print("Timeout!")
			if file.exists("wifi_fail_safe.lc") then
				dofile("wifi_fail_safe.lc")
			else
				dofile("do_sleep.lc")
			end
			wifitmr:unregister()
		end
	else
		wifitmr:unregister()
		if sf == nil then sf = require("sensor_functions") end
		sf.init()
		sf.first_sensor()
		if iptimer  ~= nil then
			iptimer:unregister()
		end
		iptimer = tmr.create()
		iptimer:register(5000, tmr.ALARM_AUTO, function()
			ipAddr = wifi.sta.getip()
			if ipAddr == nil or ipAddr == "0.0.0.0" then
				wifi.sta.disconnect()
				wifi.sta.connect()
			end
			
		end)
		iptimer:start()

	end
end)