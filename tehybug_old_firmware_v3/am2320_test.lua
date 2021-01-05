-- am2320_test.lua --
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..
--tmr.delay(2*1000000)
dofile("am2320_second.lc")

print(temp, humi, dew)

gpio.mode(3, gpio.INPUT)
gpio.trig(3,"down", debounce(onChange))
generate_sensor_data_page()

dofile("global_sensor_vars_unset.lc") --unset variables temp, humi, air, uv etc..
