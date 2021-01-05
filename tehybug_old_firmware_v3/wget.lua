function trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function w_get(url,file_name,compile,do_file)
	wget = nil
	collectgarbage()
	http.get(url, nil, function(code, data)
		if (code < 0) then
			return ("HTTP request failed")
		else
			-- print(code, data)
			-- open file in 'w' mode
			if file.open(file_name..'_wget', "w") then
				-- write text to the file
				file.write(data)
				file.flush()
				file.close()
				data = nil
				-- open file.
				local count = 0
				src = file.open(file_name..'_wget', "r")
				if src then
					local line
					repeat
						line = file.readline()
						if line then
							if count==0 then
								wget_first_line = line
							end
							wget_last_line = line
							count = count+1
							-- print(line)
						end
					until line == nil
					file.close() src = nil
				end
				if trim(wget_first_line)=='--start--' and trim(wget_last_line)=='--finish--' then
					print('File '..file_name..' is valid!')
					file.remove(file_name)
					file.rename(file_name..'_wget',file_name)
					if compile  ~= nil then
						node.compile(file_name)
					end
					if do_file  ~= nil then
						collectgarbage()
						dofile(file_name)
					end
					if norestart == nil then
						node.restart()
					end
				else
					print ('File '..file_name..' is NOT valid! Removing....')
					file.remove(file_name..'_wget')
					node.restart()
				end
			end
		end
	end)

end

dofile("config.lua")
wifi.setmode(wifi.STATIONAP)
wifi.nullmodesleep(false)
tmr.delay(1*1000000)
cfg={}
cfg.ssid = config.wifi_router_ssid
cfg.pwd = config.wifi_router_password
cfg.save=false
wifi.sta.config(cfg)
wifi.sta.connect()
cfg = nil
config = nil
wifi.eventmon.register(wifi.eventmon.STA_CONNECTED, function(T)
	print("wifi connected")
	dofile("wget_config.lua")
	w_get(wget.file_url,wget.file_name,wget.file_compile,wget.do_file)
	
	--file.remove("wget_config.lua")
end)