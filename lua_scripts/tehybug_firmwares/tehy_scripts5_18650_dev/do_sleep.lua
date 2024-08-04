-- for sensor_functions.lua
--wifi.suspend(0)
wifi.sta.disconnect()
if (config.powersaving ~= nil and config.powersaving == 0) or (config_system.no_ds ~= nil and config_system.no_ds == 1)then
	
	tmr.create():alarm(config.data_frequency*60*1000, tmr.ALARM_SINGLE, function()
		node.restart()
	end)
else
	node.dsleep(config.data_frequency*60*1000000)
end

