-- dht_test.lua --
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..

dofile("dht_second.lc")

print(status, temp, humi, dew)

gpio.mode(3, gpio.INPUT)
gpio.trig(3,"down", debounce(onChange))
generate_sensor_data_page()

dofile("global_sensor_vars_unset.lc") --unset variables temp, humi, air, uv etc..
