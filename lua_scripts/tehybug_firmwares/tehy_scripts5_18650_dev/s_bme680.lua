--bme280 test
--alt = 320
--tmr.delay(1*1000000)
i2c.setup(0, 3, 4, i2c.SLOW) -- call i2c.setup() only once
bme680.setup()
tmr.delay(1*1000000)
bme680.read()
--bme680.setup()
-- delay calculated by formula provided by Bosch: 121 ms, minimum working (empirical): 150 ms
bme680.startreadout(150, function ()
	
	local altitude = nil
	sf.do_calibration()
	if calibration ~= nil and calibration.altitude ~= nil then
		altitude = calibration.altitude*1
	end
	local T, P, H, G, QNH = bme680.read(altitude)
	i2c.stop(0)
	print(T, P, H, G, QNH)

	if T ~= nil then
		local Tsgn = (T < 0 and -1 or 1); T = Tsgn*T
		sensor.temp = string.format("%s%d.%02d", Tsgn<0 and "-" or "", T/100, T%100)
		
		sensor.qfe = string.format("%d.%02d", P/100, P%100)

		if QNH ~= nil then
			sensor.qnh = string.format("%d.%02d", QNH/100, QNH%100)
		end
		
		sensor.humi = string.format("%d.%02d", H/1000, H%1000)

		local D = bme680.dewpoint(H, T)
		local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
		sensor.dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
		
		sensor.air = string.format("%d.%02d", G/1000, G%1000)
		
		if T ~= nil then
			if sf.set ~= nil then
				sf.second_sensor()
				sf.cir()
			end
		end
	end
end)