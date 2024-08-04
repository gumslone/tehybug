tmr.create():alarm(1000, tmr.ALARM_SINGLE, function ()
	gpio.mode(10, gpio.OUTPUT) -- tx pin
	gpio.write(10, gpio.LOW)
end)