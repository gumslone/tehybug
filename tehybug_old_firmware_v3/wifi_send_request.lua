-------- Station modes -------
local STAMODE = {
STATION_IDLE			= 0,
STATION_CONNECTING		= 1,
STATION_WRONG_PASSWORD	= 2,
STATION_NO_AP_FOUND		= 3,
STATION_CONNECT_FAIL	= 4,
STATION_GOT_IP			= 5
}
------------------------------
local function printIPConf()
	print("IP: "..wifi.sta.getip())
	print("Mode: "..wifi.getmode())
	print("Channel: "..wifi.getchannel())
	print("The module MAC address is: " .. wifi.ap.getmac())
end

local function configDNS()
	net.dns.setdnsserver("8.8.8.8", 0)
	net.dns.setdnsserver("8.8.4.4", 1)
	print("DNS 1: "..net.dns.getdnsserver(0)) -- 8.8.8.8
	print("DNS 2: "..net.dns.getdnsserver(1)) -- 8.8.4.4
end


local timeout=0
local first_time=1
wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)
--connect to Access Point (DO NOT save config to flash)
station_cfg={}
station_cfg.ssid = config.wifi_router_ssid
station_cfg.pwd = config.wifi_router_password
station_cfg.save=false
wifi.sta.config(station_cfg)
wifi.sta.connect()

tmr.alarm(1, 1000, 1, function() 
	ipAddr = wifi.sta.getip()
	if ipAddr == nil or ipAddr == "0.0.0.0" then
		--print("IP unavaiable, waiting... " .. timeout)
		timeout = timeout + 1
		if timeout >= 20 then
			
			print("Timeout!")
			if wifi.sta.status() == STAMODE.STATION_IDLE then
				print("Station: idling")
				node.dsleep(10*60*1000000)
			elseif wifi.sta.status() == STAMODE.STATION_CONNECTING then
				print("Station: connecting")
				node.dsleep(10*60*1000000)
			elseif wifi.sta.status() == STAMODE.STATION_WRONG_PASSWORD then
				print("Station: wrong password") 
				print("Enter configuration mode")
				file.remove("system_status.lua")
				file.open("system_status.lua", "w")
				file.writeline('system_status = { ')
				file.writeline('system_status = 0;')
				file.writeline('} ')
				file.close()
				node.restart()
			elseif wifi.sta.status() == STAMODE.STATION_NO_AP_FOUND then
				print("Station: AP not found")
				node.dsleep(10*60*1000000)
			elseif wifi.sta.status() == STAMODE.STATION_CONNECT_FAIL then
				print("Station: connection failed")
				node.dsleep(10*60*1000000)
			else
				node.dsleep(10*60*1000000)
			end
			tmr.unregister(1)
		end
		
	else
		
		
		
		timeout = timeout + 1
		
		
		collectgarbage()
		
		if first_time == 1 or (first_time == 0 and timeout >= config.data_frequency*60) then
			
			if config.data_frequency * 1 > 5 then
				tmr.unregister(1)
			else
				tmr.stop(1) -- stop timer and start in the request.lua file for short data frequencies
			end
			
			timeout = 0
			first_time = 0
			dofile(system_config.sensor_type .. ".lc") --request.lua inside the file
			
		end

		

	end
end)