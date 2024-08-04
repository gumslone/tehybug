gpio.mode(3, gpio.OUTPUT)
gpio.mode(4, gpio.OUTPUT)
tmr.delay(500)
gpio.write(3, gpio.LOW)
gpio.write(4, gpio.LOW)
tmr.create():alarm(5000, tmr.ALARM_SINGLE, function ()

-- set pin index 2 to GPIO mode, and set the pin to high.

gpio.write(3, gpio.HIGH)
gpio.write(4, gpio.HIGH)

end)