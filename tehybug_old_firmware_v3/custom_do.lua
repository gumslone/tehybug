tmr.alarm(3, 3000, tmr.ALARM_SINGLE, function ()
	system_status = nil
	system_config = nil
	dofile("dofile.lua")
end)