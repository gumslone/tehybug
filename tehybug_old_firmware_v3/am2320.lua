-- am2320.lua --
dofile("sensor_functions.lc")
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..

dofile("am2320_second.lc")

--print(status, temp, humi, temp_dec, humi_dec)
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
		if humi ~= nil and humi ~= '' then
			write_data('data_humi',humi,10)
		end
		if dew ~= nil and dew ~= '' then
			write_data('data_dew',dew,10)
			write_data('data_dew_imp',dew_imp,10)
		end
	end
else
	dofile("request.lc")
end
--gpio.mode(3,gpio.INPUT)
