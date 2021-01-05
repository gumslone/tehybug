--bme280
dofile("sensor_functions.lc")
if file.exists("calibration_config.lua") then
	dofile("calibration_config.lua")
end
dofile("global_sensor_vars.lc") --variables temp, humi, air, uv etc..
--alt = 320
tmr.delay(1*1000000)
sda, scl = 3, 4
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once
--bme280.init(sda, scl)
--bme280.setup(nil, nil, nil, 1)
bme280.setup()
--T, P, H, QNH = bme280.read()
bme680.startreadout(150, function ()
	T, P, H, QNH = bme280.read()
	if T ~= nil then
		--print(T, P, H, QNH)
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
		D = bme280.dewpoint(H, T)
		local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
		dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
		dew_imp = (dew*1.8)+32
		dew_imp = string.format("%.2f", dew_imp)
	end
	
	if T ~= nil then
		
		additional_sensors()
		
		if(config.request_type == "display" or config.request_type == "web") then
			if file.exists("display_config.lua") then
				dofile("display_config.lua")
				--print("Display config exists")
				dofile("oled128x64.lc")
			end
			
			if config.request_type == "web" and config.web_output ~= nil then
				if temp ~= nil and temp ~= '' then
					write_data('data_temp',temp,10)
					write_data('data_temp_imp',temp_imp,10)
				end
				if humi ~= nil and humi ~= '' then
					write_data('data_humi',humi,10)
				end
				if dew ~= nil and dew ~= '' then
					write_data('data_dew',dew,10)
					write_data('data_dew_imp',dew_imp,10)
				end
				if qfe ~= nil and qfe ~= '' then
					write_data('data_qfe',qfe,10)
					write_data('data_qfe_imp',qfe_imp,10)
				end
				if uv ~= nil and uv ~= '' then
					write_data('data_uv',uv,10)
				end
				if lux ~= nil and lux ~= '' then
					write_data('data_lux',lux,10)
				end
			end
		else
			dofile("request.lc")
		end
	end

end)

--print(config.server_url)