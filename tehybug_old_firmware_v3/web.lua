-- starting webserver and set up the files to be served
--print("Wireless client")
wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)
--connect to Access Point (DO NOT save config to flash)
station_cfg={}
station_cfg.ssid = config.wifi_router_ssid
station_cfg.pwd = config.wifi_router_password
station_cfg.save=false
wifi.sta.config(station_cfg)
if wifi.sta.sethostname("tehybug"..system_config.chip_id) == true then
	print("hostname was successfully changed to: tehybug"..system_config.chip_id)
end
wifi.sta.connect()

dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..

local timeout=0
local first_time=1
tmr.alarm(1, 1000, 1, function() 
		
		timeout = timeout + 1
		if (first_time == 1 and timeout >= 10) or (first_time == 0 and timeout >= config.data_frequency*60) then
			timeout = 0
			first_time = 0
			--print("Timeout!")
			if system_config.sensor_type == 'ds18b20' then
				dofile("ds18b20.lc") --display stuff inside the file
			else
				dofile(system_config.sensor_type..".lc")
				if file.exists("display_config.lua") then
					dofile("display_config.lua")
					--print("Display config exists")
					dofile("oled128x64.lc")
				end
			end
		end
	
end)


function web_placeholders(s)
	if string.find(s, "%%temp_values%%") then
		if file.open("data_temp", "r") then
			s = string.gsub(s, "%%temp_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%temp_imp_values%%") then
		if file.open("data_temp_imp", "r") then
			s = string.gsub(s, "%%temp_imp_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%humi_values%%") then
		if file.open("data_humi", "r") then
			s = string.gsub(s, "%%humi_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%dew_values%%") then
		if file.open("data_dew", "r") then
			s = string.gsub(s, "%%dew_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%dew_imp_values%%") then
		if file.open("data_dew_imp", "r") then
			s = string.gsub(s, "%%dew_imp_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%qfe_values%%") then
		if file.open("data_qfe", "r") then
			s = string.gsub(s, "%%qfe_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%qfe_imp_values%%") then
		if file.open("data_qfe_imp", "r") then
			s = string.gsub(s, "%%qfe_imp_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%uv_values%%") then
		if file.open("data_uv", "r") then
			s = string.gsub(s, "%%uv_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%lux_values%%") then
		if file.open("data_lux", "r") then
			s = string.gsub(s, "%%lux_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%air_values%%") then
		if file.open("data_air", "r") then
			s = string.gsub(s, "%%air_values%%", file.readline(),1)
			file.close()
		end
	end
	if string.find(s, "%%soil_values%%") then
		if file.open("data_soil", "r") then
			s = string.gsub(s, "%%soil_values%%", file.readline(),1)
			file.close()
		end
	end
	return(s)
end

srv = nil
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function(T)
	local mytimer = tmr.create()
	
	mytimer:register(3000, tmr.ALARM_SINGLE,function (t) 
		print("Open in your browser http://"..tostring(wifi.sta.getip()).."/") 
		mdns.register("tehybug"..system_config.chip_id, { description="TeHyBug environmental data tracker", service="http", port=80, location='Local network' })
		t:unregister()
	end)
	mytimer:start()
	
	if srv == nil then
			--print("Setting up webserver")
			
			srv = net.createServer(net.TCP)
			srv:listen(80,function(conn)
				conn:on("receive", function(client,request)
				
					local buf = ""
					local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
					if(method == nil)then
						_, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP")
					end
					local _GET = {}
					if (vars ~= nil)then
						for k, v in string.gmatch(vars, "(%w+)=([^%&]+)&*") do
							_GET[k] = unescape(v)
						end
					end
					
					if config.web_output ~= nil then
						buf = replace_placeholders(config.web_output)
					else
						buf = "web output is not defined"
					end
						
						
					client:send(buf)
					client:on("sent",function(client)
					--	print("Closing connection")
						client:close()
					end)
					--client:close()
					collectgarbage()
					--print("Setting html")

					
				end)
			end)
	end
	
	
	
	
	
end)
--end of setup server