-- starting webserver and set up the files to be served
filePos=0
msg = ""
act = ""
out = ""
dofile("setup_functions.lc")
dofile("setup_led.lc")
--setup server
print("Setting up Wifi AP")
--wifi.setmode(wifi.SOFTAP)
wifi.setmode(wifi.STATIONAP)
wifi.nullmodesleep(false)
cfg = {}
cfg.ssid = system_config.setup_ssid
wifi.ap.config(cfg)
cfg = nil
wifi.ap.setip({ip="192.168.1.1",netmask="255.255.255.0",gateway="192.168.1.1"})
--print("Setting up webserver")
s = nil
s=net.createServer(net.TCP)
s:listen(80,function(c)

	c:on("receive", function(c,rq)
		--print("[New Request]")
		out = ""
		msg = ""
		node.output(nil) 
		
		local timeout = 0
		local _, _, method, path, vars = string.find(rq, "([A-Z]+) (.+)?(.+) HTTP")
		if method == nil then
			_, _, method, path = string.find(rq, "([A-Z]+) (.+) HTTP")
		end
		local _GET = {}
		if vars ~= nil then
			for k, v in string.gmatch(vars, "([%w_-]+)=([^%&]+)&*") do
				--print("["..k.."="..v.."]")
				_GET[k] = unescape(v)
			end
		end
		
		act = _GET.action
		_GET.action = nil
		rq = nil
		if path=="/" then
			rqFile = "index.html"
		else
			rqFile = string.gsub(path, "/", "")
		end
		
		--content
		if file.exists(rqFile) then
			
			filePos=0
			print("[Sending file "..rqFile.."]")
			
			if act == "save_configuration" then
				file.remove(_GET.configuration_file)
				file.open(_GET.configuration_file, "w")
				file.writeline(_GET.configuration_name .. ' = { ')
				
				if(_GET.configuration_name == 'config' and _GET.server_url == nil) then
					file.writeline('server_url = "' .. trim(system_config.server_url) .. '";')
				end
				
				_GET.configuration_name = nil
				_GET.configuration_file = nil
				for k,v in pairs(_GET) do
					file.writeline(''..k..' = ' .. string.format("%q", trim(v)) .. ';')
				end
				
				file.writeline('} ')
				file.close()
				msg = '<div class="message success">Configuration file updated successfully! You can <a href="/?action=activate_live_mode">activate</a> the live mode to start tracking the environmental data or <a href="/?action=restart" target="_blank">restart</a> the TeHyBug.</div>'
				if _GET.configuration_name == "wget" then
					node.restart()
				end
			elseif act == "reset_config" then
				file.remove("config.lua")
				file.open("config.lua", "w")
				file.writeline('config = { ')
				file.writeline('wifi_router_ssid = "MyWifi-123";')
				file.writeline('wifi_router_password = "abc123";')
				file.writeline('data_frequency = "30";')
				file.writeline('server_url = "' .. trim(system_config.server_url) .. '";')
				file.writeline('} ')
				file.close()
				
				msg = '<div class="message success">Configuration file reseted successfully!</div>'
			elseif act == "remove_display" or act == "remove_calibration" then
				msg = '<div class="message success">Configuration removed successfully!</div>'
				if act == "remove_calibration" then
					file.remove("calibration_config.lua")
				else
					file.remove("display_config.lua")
				end
			elseif act == "execute" then
				out = ""
				msg = '<div class="message info">Command '.. string.format("%q", _GET.execute_command) .. ' executed successfully!</div>'
				function s_output(str)
					if str ~= nil then
						out =  out ..''.. str
					end
				end
				node.output(s_output, 1)
				node.input(_GET.execute_command)
			elseif act == "activate_live_mode" then
				file.remove("system_status.lua")
				file.open("system_status.lua", "w")
				file.writeline('system_status = { ')
				file.writeline('system_status = 1;')
				file.writeline('} ')
				file.close()
				msg = '<div class="message success">Live mode activated successfully! Please <a href="/?action=restart" target="_blank">restart</a> the TeHyBug</div>'
				
			elseif act == "restart" then
				wifi.setmode(wifi.NULLMODE)
				node.restart()
			end
			
			if string.find(rqFile, ".css") then
				ct = "text/css"
			elseif string.find(rqFile, ".js") then
				ct = "text/javascript"
			else
				ct = "text/html"
			end
			
			if string.find(rqFile, ".gz") then
				c:send("HTTP/1.1 200 OK\r\nContent-Type: "..ct.."\r\nCache-Control: private, max-age=2592000\r\nContent-Encoding: gzip\r\n\r\n")
			else
				c:send("HTTP/1.1 200 OK\r\nContent-Type: "..ct.."\r\n\r\n") 
				
				if rqFile=="sensor_data.html" or rqFile=="sensor_data.json" then
					dofile(system_config.sensor_type.."_test.lc")
					--calibration = nil
				end

			end
		else
			print("[File "..path.." not found]")
			c:send("HTTP/1.1 404 Not Found\r\n\r\n")
			c:close()
			collectgarbage()
		end
		
	end)
	c:on("sent",function(c)

			if rqFile=="configuration_default.html" or rqFile=="configuration_custom_post.html" or rqFile=="configuration_custom_get.html" or rqFile=="configuration_custom_mqtt.html" or rqFile=="configuration_custom_web.html" or rqFile=="configuration_display_only.html" then
				dofile("config.lua")
			elseif rqFile=="configuration_display.html" then
				if file.exists("display_config.lua") then
					dofile("display_config.lua")
				end
			else
				config = nil
				display_config = nil
			end
			
			local prepend = ''
			local append = ''
					
			if rqFile then
				if file.open(rqFile,r) then
					file.seek("set",filePos)
					local partial_data=file.read(512)
					file.close()
					if partial_data then
						filePos=filePos+#partial_data
						--print("["..filePos.." bytes sent]")
						
						if act == "view_file" then
							if(filePos<=512) then
								prepend = '<html><body><pre>'
							else
								prepend = ''
							end
							if string.len(partial_data) < 512 then
								append = '</pre><div style="margin-top: 15px"><a href="/file_list.html">&lt File list</a><br><a href="/">&lt Main Menu</a></div><link rel="stylesheet" href="/style.css.gz"></body></html>'
							end
							
							c:send(prepend..replace_html(partial_data)..append)
						elseif rqFile=="file_list.html" then
							c:send(partial_data,rqFile)
						else
							
							c:send(replace_config(partial_data,rqFile))
						end
						
						if string.len(partial_data)==512 then
							return
						end
						
					else
						
					end
				else
					--print("[Error opening file"..rqFile.."]")
				end
	
			end
			
			--print("[Connection closed]")
			c:close()
			collectgarbage()
	end)
	c:on("disconnection",function(c)
		node.output(nil) -- un-regist the redirect output function, output goes to serial
	end)
	
end)

print("Please connect to: " .. system_config.setup_ssid .. " - IP: " .. wifi.ap.getip())
--end of setup server