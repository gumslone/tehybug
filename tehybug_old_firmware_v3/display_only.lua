wifi.setmode(wifi.NULLMODE)
local timeout=0
local first_time=1
tmr.alarm(1, 1000, 1, function() 
		
		timeout = timeout + 1
		if (first_time == 1 and timeout >= 10) or (first_time == 0 and timeout >= config.data_frequency*60) then
			timeout = 0
			first_time = 0
			--print("Timeout!")
			dofile(system_config.sensor_type .. ".lc") --request.lua and show display inside the file
		end
		

end)