--bme280 test
dofile("sensor_functions.lc")
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..
--alt = 320
tmr.delay(1*1000000)
local sda, scl = 3, 4
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once

--bme280.setup(nil, nil, nil, 1)
bme280.setup()

bme680.startreadout(150, function ()
	local T, P, H, QNH = bme280.read()
	print(T, P, H, QNH)
	if T ~= nil then
		print(T, P, H, QNH)
		local Tsgn = (T < 0 and -1 or 1); T = Tsgn*T
		temp = string.format("%s%d.%02d", Tsgn<0 and "-" or "", T/100, T%100)
		if calibration ~= nil and calibration.temp ~= nil then
			temp = temp + calibration.temp
		end
		temp_imp = (temp*1.8)+32
		temp_imp = string.format("%.2f", temp_imp)
		qfe = string.format("%d.%02d", P/1000, P%1000)
		if calibration ~= nil and calibration.qfe ~= nil then
			qfe = qfe + calibration.qfe
		end
		qfe_imp = 0.02952998751 * qfe
		qfe_imp = string.format("%.2f", qfe_imp)
	end
	if H ~= nil then
		humi = string.format("%d.%02d", H/1000, H%1000)
		if calibration ~= nil and calibration.humi ~= nil then
			humi = humi + calibration.humi
		end
		local D = bme280.dewpoint(H, T)
		local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
		dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
		dew_imp = (dew*1.8)+32
		dew_imp = string.format("%.2f", dew_imp)
	end
	
	if T ~= nil then
		additional_sensors()

		generate_sensor_data_page()
		dofile("global_sensor_vars_unset.lc") --unset variables temp, humi, air, uv etc..
	end
	
	i2c.stop(0)
	gpio.mode(3, gpio.INPUT)
	gpio.trig(3,"down", debounce(onChange))
	

end)