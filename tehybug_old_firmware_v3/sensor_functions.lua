function write_data(f,d,l)
	local file_data
	local i = 1
	local t = {}
	
	t[i] = d
	
	if file.open(f, "r") then
		file_data = file.readline()
		--print(file_data)
		file.close()
	end
	
	if file_data ~= nil then
		for v in string.gmatch(file_data, '([^,]+)') do
			i = i + 1
			t[i] = v
		end
	end
	
	if #t > l then
		t[l+1] = nil
	end
	
	file.remove(f)
	if file.open(f, "a+") then
		file.writeline(table.concat(t,","))
		file.close()
	end
	--for key,value in pairs(t) do print(key,value) end
end

function replace_placeholders(s)
	s = string.gsub(s, "%%temp%%", temp,1)
	s = string.gsub(s, "%%temp_imp%%", temp_imp,1)
	s = string.gsub(s, "%%humi%%", humi,1)
	s = string.gsub(s, "%%qfe%%", qfe,1)
	s = string.gsub(s, "%%qfe_imp%%", qfe_imp,1)
	s = string.gsub(s, "%%dew%%", dew,1)
	s = string.gsub(s, "%%dew_imp%%", dew_imp,1)
	s = string.gsub(s, "%%uv%%", uv,1)
	s = string.gsub(s, "%%lux%%", lux,1)
	s = string.gsub(s, "%%air%%", air,1)
	s = string.gsub(s, "%%key%%", system_config.bug_key,1)
	s = string.gsub(s, "%%chipid%%", system_config.chip_id,1)
	s = string.gsub(s, "%%fw%%", system_config.firmware_version,1)
	s = string.gsub(s, "%%data%%", oled_data,1)
	s = string.gsub(s, "%%vdd%%", vdd,1)
	s = string.gsub(s, "%%soil%%", soil,1)
	return(s)
end

function additional_sensors()
	if file.exists("add_max44009.lua") then
		dofile("add_max44009.lua")
	end
	if file.exists("add_veml6070.lua") then
		dofile("add_veml6070.lua")
	end
	if file.exists("add_am2320.lua") then
		dofile("add_am2320.lua")
	end
end