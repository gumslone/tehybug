--ds18b20 test
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
			print(ind,string.format("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",string.match(rom,"(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+)")),res,tem,tdec,par)
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
				generate_sensor_data_page()
				dofile("global_sensor_vars_unset.lc") --unset variables temp, humi, air, uv etc..
			end
		end,{});
end)
