--ds18b20
dofile("sensor_functions.lc")
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
local http_sent = 0
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..

ds18b20.setup(4)
tmr.alarm(5, 1000, 1, function() 
	-- read all sensors and print all measurement results
	ds18b20.read(
		function(ind,rom,res,tem,tdec,par)
			--print(ind,string.format("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",string.match(rom,"(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+)")),res,temp,tdec,par)
			if http_sent == 0 then
				tmr.unregister(5)
				temp = tem
				if calibration ~= nil and calibration.temp ~= nil then
					temp = temp + calibration.temp
				end
				temp = string.format("%.2f", temp)
				temp_imp = (temp*1.8)+32
				temp_imp = string.format("%.2f", temp_imp)
				http_sent = 1
				if(config.request_type == "display" or config.request_type == "web") then
					if file.exists("display_config.lua") then
						dofile("display_config.lua")
						--print("Display config exists")
						dofile("oled128x64.lc")
					end
					
					if config.request_type == "web" and config.web_output ~= nil then
						if temp ~= nil and temp ~= '' then
							write_data('data_temp',temp,10)
							write_data('data_temp_imp',temp_imp,10)
						end
						
					end
				else
					dofile("request.lc")
				end
			end
		end,{});
end)