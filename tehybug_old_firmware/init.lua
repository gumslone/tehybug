--init.lua
dofile("system_config.lua")
dofile("system_status.lua")
buttonPin = 3 -- this is ESP-01 pin GPIO00
gpio.mode(buttonPin,gpio.INT,gpio.PULLUP)

--chipserial = node.chipid()

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


function debounce (func)
	local last = 0
	local delay = 200000

	return function (...)
		local now = tmr.now()
		if now - last < delay then return end

		last = now
		return func(...)
	end
end

function onChange()
	if gpio.read(buttonPin) == 0 then
		print("Button Pressed!")
		if(system_status.system_status==1) then
			file.remove("system_status.lua")
			file.open("system_status.lua", "w")
			file.writeline('system_status = { ')
			file.writeline('system_status = 0;')
			file.writeline('} ')
			file.close()
			node.restart()
		else
			file.remove("system_status.lua")
			file.open("system_status.lua", "w")
			file.writeline('system_status = { ')
			file.writeline('system_status = 1;')
			file.writeline('} ')
			file.close()
			node.restart()
		end
		tmr.delay(500000)
	end
end

gpio.trig(buttonPin,"down", debounce(onChange))


local timeout=0
if(system_status.system_status==0) then
	print("Enter configuration mode")
	dofile("config.lua")
	dofile("setup.lua")
elseif pcall(function () 
	dofile("config.lua")
end) then
	--print("Connecting to WIFI...")
	--realtype = wifi.sleeptype(wifi.MODEM_SLEEP)
	wifi.setmode(wifi.STATION)
	wifi.sta.config(config.wifi_router_ssid, config.wifi_router_password)
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
					tmr.stop(1)
					print("Station: wrong password") 
					print("Enter configuration mode")
					dofile("setup.lua")
				elseif wifi.sta.status() == STAMODE.STATION_NO_AP_FOUND then
					print("Station: AP not found")
					node.dsleep(10*60*1000000)
				elseif wifi.sta.status() == STAMODE.STATION_CONNECT_FAIL then
					print("Station: connection failed")
					node.dsleep(10*60*1000000)
				else
					node.dsleep(10*60*1000000)
				end
				--node.restart()
			end
            
			
		else
			tmr.stop(1)
			--print("Connected, IP is "..wifi.sta.getip())
			--printIPConf()
			--configDNS()
			collectgarbage()
			if system_config.sensor_type == 'bme280' then
				dofile("bme280.lua")
			elseif system_config.sensor_type == 'ds18b20' then
				dofile("ds18b20.lua")
			else
				dofile("dht.lua")
			end
		end
	end)
else
	print("Enter configuration mode")
	dofile("setup.lua")
end
