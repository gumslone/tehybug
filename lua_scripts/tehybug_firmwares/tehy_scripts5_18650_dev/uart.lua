wifi.setmode(wifi.NULLMODE)

tmr.create():alarm(10000, tmr.ALARM_AUTO, function() 

	if sf == nil then sf = require("sensor_functions") end
	sf.init()
	sf.first_sensor()

end)