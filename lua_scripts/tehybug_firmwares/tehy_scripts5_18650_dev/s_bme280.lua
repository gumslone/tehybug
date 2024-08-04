--bme280 test
--alt = 320
i2c.setup(0, 3, 4, i2c.SLOW) -- call i2c.setup() only once

--bme280.setup(nil, nil, nil, 1)
bme280.setup()

bme680.startreadout(150, function ()
	local altitude = nil
	sf.do_calibration()
	if calibration ~= nil and calibration.altitude ~= nil then
		altitude = calibration.altitude*1
	end
	local T, P, H, QNH = bme280.read(altitude)
	i2c.stop(0)
	if T ~= nil then
		print(T, P, H, QNH)
		local Tsgn = (T < 0 and -1 or 1); T = Tsgn*T
		sensor.temp = string.format("%s%d.%02d", Tsgn<0 and "-" or "", T/100, T%100)

		sensor.qfe = string.format("%d.%02d", P/1000, P%1000)
		
		if QNH ~= nil then
			sensor.qnh = string.format("%d.%02d", QNH/1000, QNH%1000)
		end
		
	end
	if H ~= nil then
		sensor.humi = string.format("%d.%02d", H/1000, H%1000)

		local D = bme280.dewpoint(H, T)
		local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
		sensor.dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
	end
	
	if T ~= nil then
		if sf.set ~= nil then
			sf.second_sensor()
			sf.cir()
		end
	end
end)
