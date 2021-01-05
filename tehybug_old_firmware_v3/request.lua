--if config.data_frequency * 1 > 5 then
--	tmr.alarm(3, 10000, tmr.ALARM_SINGLE, function ()
--		node.dsleep(config.data_frequency*60*1000000) -- deep sleep after 10 seconds timout
--	end)
--end


if config.request_type=='mqtt' then
	-- init mqtt client without logins, keepalive timer 120s
	m = mqtt.Client(system_config.setup_ssid, 120, config.mqtt_user, config.mqtt_password)
	m:on("offline", function(client) 
		print ("offline")
			if config.data_frequency * 1 > 5 then
				wifi.sta.disconnect()
				node.dsleep(config.data_frequency*60*1000000)
			else
				tmr.start(1)
			end
		
	end)
	-- for TLS: m:connect("192.168.11.118", secure-port, 1)
	m:connect(config.mqtt_ip, config.mqtt_port, 0, function(client)
		-- publish a message with data = hello, QoS = 0, retain = 0
		client:publish(config.mqtt_topic, replace_placeholders(config.mqtt_message), 0, 0, function(client) 
			--print("sent")
			if file.exists("display_config.lua") then
				dofile("display_config.lua")
				--print("Display config exists")
				dofile("oled128x64.lc")
			end
			
			if config.data_frequency * 1 > 5 then
				wifi.sta.disconnect()
				node.dsleep(config.data_frequency*60*1000000)
			else
				tmr.start(1)
			end
			
		 end)
	end,
	function(client, reason)
		print("failed reason: " .. reason)
		if file.exists("display_config.lua") then
			dofile("display_config.lua")
			--print("Display config exists")
			dofile("oled128x64.lc")
		end
		
			if config.data_frequency * 1 > 5 then
				wifi.sta.disconnect()
				node.dsleep(config.data_frequency*60*1000000)
			else
				tmr.start(1)
			end
	end)
	m:close();
	-- you can call m:connect again
	
elseif config.request_type=='http_post' then

	http.post(config.server_url,
		'Content-Type: application/json\r\n',
		replace_placeholders(config.post_content),
		function(code, data)
			if (code < 0) then
				print("HTTP request failed")
			else
				print(code, data)
				if data ~= nil then
					oled_data = string.sub(data, 1, 24)
				end
			end
		
			if file.exists("display_config.lua") then
				dofile("display_config.lua")
				--print("Display config exists")
				dofile("oled128x64.lc")
			end
			
			if config.data_frequency * 1 > 5 then
				wifi.sta.disconnect()
				node.dsleep(config.data_frequency*60*1000000)
			else
				tmr.start(1)
			end

		end)

else
	-- http request
	http.get(replace_placeholders(config.server_url), nil, function(code, data)
		if (code < 0) then
		--print("HTTP request failed")
		else
		--	print(code, data)
			if data ~= nil then
				oled_data = string.sub(data, 1, 24)
			end
		end
		
		if file.exists("display_config.lua") then
			dofile("display_config.lua")
			--print("Display config exists")
			dofile("oled128x64.lc")
		end
		
		if config.data_frequency * 1 > 5 then
			wifi.sta.disconnect()
			node.dsleep(config.data_frequency*60*1000000)
		else
			tmr.start(1)
		end

	end)

end