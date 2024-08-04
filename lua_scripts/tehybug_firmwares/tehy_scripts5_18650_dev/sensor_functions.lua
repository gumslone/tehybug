local sf = {}
sf.set = 1
sf.test = ''
sf.gpio0 = nil
sf.gpio2 = nil
sf.sensors = nil
sf.current_sensor = 1
sf.data_sent = 0
function sf.init()
	sf.pcall("config_sensors.lua")
end

function sf.calibrate()
	if calibration ~= nil then
		for k,v in pairs(sensor) do 
			if calibration[k] ~= nil then
				sensor[k] = v + calibration[k]
			end
		end
	end
end

function sf.imperialize()
	if sensor.temp ~= nil then
		sensor.temp_imp = string.format("%.2f", ((sensor.temp*1.8)+32))
	end
	if sensor.dew ~= nil then
		sensor.dew_imp = string.format("%.2f", ((sensor.dew*1.8)+32))
	end
	if sensor.qfe  ~= nil then
		sensor.qfe_imp = string.format("%.3f", (0.02952998751 * sensor.qfe))
	end
	if sensor.qnh ~= nil then
		sensor.qnh_imp = string.format("%.3f", (0.02952998751 * sensor.qnh))
	end
end

function sf.replace_placeholders(s)
	s = string.gsub(s, "%%key%%", config_system.bug_key,1)
	for k,v in pairs(sensor) do 
		s = string.gsub(s, "%%"..k.."%%", v,1)
	end
	s = string.gsub(s, "%%a%%", '')
	return(s)
end

function sf.generate_sensor_data_page()
		fd = file.open('sensor_data.txt', "w+")
		if fd then
			fd:flush()
			if sensor ~= nil then 
				for k,v in pairs(sensor) do
					fd:writeline('"'..k..'":'..v..',')
				end
			end
			fd:close()
			fd = nil
		end
end

function sf.do_calibration()
	if file.exists("config_calibration.lua") then
		sf.pcall("config_calibration.lua")
	end
end

function sf.pcall(i)
	if pcall(function () dofile(i) end) then else end
end

function sf.failsafe_datasend()
	tmr.create():alarm(10*60*1000, tmr.ALARM_SINGLE, function()
		if sf.data_sent==0 then
			dofile("do_sleep.lc")
		end 
	end)
end

function sf.first_sensor()
	sf.current_sensor = 1
	if sf.sensors.first ~= nil then
		sf.pcall("s_"..sf.sensors.first..sf.test..".lc")
	end
end

function sf.second_sensor()
	sf.current_sensor = 2
	if sf.sensors.second ~= nil then
		sf.pcall("s_"..sf.sensors.second..sf.test..".lc")
	end
end

function sf.third_sensor()
	sf.current_sensor = 3
	if sf.sensors.third ~= nil then
		sf.pcall("s_"..sf.sensors.third..sf.test..".lc")
	end
end

function sf.scenario(i)
	f = "config_scenario"..i..".lua"
	if file.exists(f) then
		sf.pcall(f)
		sf.pcall("do_scenario.lc")
		sf.scenario(i+1)
	end
end

function sf.request()
	if config ~= nil and config.request_type == "web" or system.status==0 then
		sf.generate_sensor_data_page()
	end
	if system.status==1 then
		sf.pcall("request_diff.lc")
	end
end

function sf.cir()
	sf.calibrate()
	sf.imperialize()
	sf.request()
end

function sf.deep_sleep()
	collectgarbage("collect")
	sf.scenario(1)

	if sf.gpio0  ~= nil then
		gpio.mode(3, gpio.OUTPUT) -- gpio 0
		if sf.gpio0 == 1 then
			gpio.write(3, gpio.HIGH)
		else
			gpio.write(3, gpio.LOW)
		end
	end
	if sf.gpio2  ~= nil then
		gpio.mode(4, gpio.OUTPUT) -- gpio 2
		if sf.gpio2 == 1 then
			gpio.write(4, gpio.HIGH)
		else
			gpio.write(4, gpio.LOW)
		end
	end

	if sf.sensors.first == "opendoor" or sf.sensors.first == "button" then
		tmr.create():alarm(1*1000, tmr.ALARM_SINGLE, function()
			sf.first_sensor()
		end)
	elseif config == nil or config.data_frequency == nil then
		print('config nil')
	elseif config.data_frequency * 1 < 5 or config.ds == "off" then
		tmr.create():alarm(config.data_frequency*60*1000, tmr.ALARM_SINGLE, function()
			dofile("do_no_sleep.lc")
		end)
	else
		tmr.create():alarm(3*1000, tmr.ALARM_SINGLE, function()
			dofile("do_sleep.lc")
		end)
	end
end

return sf