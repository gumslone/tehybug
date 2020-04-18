dofile("system_config.lua")
leds = system_config.leds
brightness = system_config.brightness
tmr.create():alarm(5000, tmr.ALARM_SINGLE, function ()
	dofile("button.lua")
end)
dofile("boot.lua")
dofile("wifi.lua")