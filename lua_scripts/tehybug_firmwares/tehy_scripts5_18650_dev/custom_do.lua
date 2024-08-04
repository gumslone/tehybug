tmr.create():alarm(3000, tmr.ALARM_SINGLE, function ()
	system_status = nil
	config_system = nil
	dofile("dofile.lua")
end)