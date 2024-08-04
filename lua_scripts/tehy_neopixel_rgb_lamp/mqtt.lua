

-- init mqtt client without logins, keepalive timer 120s
timeout = 0
connected = 0
chipid = node.chipid()
topic = "/neopixel"
nodename = system_config.device_hostname
broker = system_config.broker
port = 1883


-- init mqtt client without logins, keepalive timer 120s
m = mqtt.Client(nodename, 120)

-- init mqtt client with logins, keepalive timer 120sec

-- setup Last Will and Testament (optional)
-- Broker will publish a message with qos = 0, retain = 0, data = "offline" 
-- to topic "/lwt" if client don't send keepalive packet
m:lwt("/neopixel", "OFFLINE "..nodename, 0, 0)

m:on("connect", function(client)
	print ("connected")
end)
m:on("offline", function(client) 
	connected = 0
	print ("offline")
	print("Offline reconnect... ")
	do_mqtt_connect()
end)

-- on publish message receive event
m:on("message", function(client, topic, data) 
	print(topic .. ":" ) 
	if data ~= nil then
		--print(data)
		if data ~= nil then
			print(data)
			if data == "check" then
				client:publish(topic, nodename, 0, 0, function(client) print("sent") end)
			elseif data == "reboot "..nodename or data == "restart "..nodename then
				client:publish(topic, nodename.." going to reboot", 0, 0, function(client) print("sent") end)
				node.restart()
			elseif data == nodename.." off" then
				ws2812_effects.stop()
				-- init the ws2812 module
				ws2812.init(ws2812.MODE_SINGLE)
				-- create a buffer, 60 LEDs with 3 color bytes
				buffer = ws2812.newBuffer(leds, 4)
				buffer:fill(0, 0, 0, 0)
				ws2812.write(buffer)
			else
				s = split(data, " ")
				if s[1] == nodename then
					client:publish(topic, "RE: "..s[1].." "..s[2], 0, 0, function(client) print("sent") end)
					if s[2] == "wget" then
						local file_name = s[3]:match( "([^/]+)$" )
						w_get(s[3],file_name,nil,nil,1)
					elseif s[2]=="compile" then
						node.compile(s[3])
					elseif s[2]=="dofile" then
						dofile(s[3])
					elseif s[2]=="remove" then
						file.remove(s[3])
					elseif s[2]=="set" then
						local pixels = s[3]
						local leds = s[4]
						local brightness = s[5]
						ws2812_effects.stop()
						-- init the ws2812 module
						ws2812.init(ws2812.MODE_SINGLE)
						-- create a buffer, 60 LEDs with 3 color bytes
						buffer = ws2812.newBuffer(leds, 3)

						for i=1,leds*1 do 
							local incr = (i*6)-6+1
							s = string.sub(pixels, incr, incr-1+6)
							local r,g,b = HEXtoRGB(s)
							local w=0
							if (r==255 and g==255 and b==255) then
								r=0
								b=0
								g=0
								w=255
							else
								w = 0
							end
							
							buffer:set(i, {g, r, b, w}) -- set the first led green for a RGB strip
							ws2812.write(buffer)
						end
						buffer:mix(brightness*1, buffer)
						ws2812.write(buffer)
					
					elseif s[2]=="pixelset" then
						local pixel = s[3]
						local color = s[4]
						local brightness = s[5]
						ws2812_effects.stop()
						-- init the ws2812 module
						ws2812.init(ws2812.MODE_SINGLE)
						-- create a buffer, 60 LEDs with 3 color bytes
						buffer = ws2812.newBuffer(leds, 3)

						local r,g,b = HEXtoRGB(color)
						local w=0
						if (r==255 and g==255 and b==255) then
							r=0
							b=0
							g=0
							w=255
						else
							w = 0
						end
						buffer:set(pixel, {g, r, b, w}) -- set the first led green for a RGB strip
						ws2812.write(buffer)
						buffer:mix(brightness*1, buffer)
						ws2812.write(buffer)
					end
				end
			end
		end
	end
end)

function change_status(status)
	local fd = file.open("status.lua", "w+")
	if fd then
		print("rollo status: "..status)
		fd:writeline('rollo={status="'..status..'"} ')
		fd:close()
		fd = nil
	end
end

function handle_mqtt_error(client, reason)
	print("failed reason: " .. reason)
	tmr.create():alarm(10 * 1000, tmr.ALARM_SINGLE, do_mqtt_connect)
end

function do_mqtt_connect()
	m:connect(broker, port, 0, function(client)
		client:subscribe(topic, 0, function(client) 
			connected = 1
			print("subscribe success") 
		end)
	end, handle_mqtt_error)
end

function mqtt_pub(message)
	m:publish(topic, message, 0, 0, function(client) print("sent") end)
end

function trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function w_get(url,file_name,compile,do_file,norestart)
	wget = nil
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

function split(s, delimiter)
	result = {};
	for match in (s..delimiter):gmatch("(.-)"..delimiter) do
		table.insert(result, match);
	end
	return result;
end


function HEXtoRGB(hexArg)

	hexArg = hexArg:gsub('#','')

	if(string.len(hexArg) == 3) then
		return tonumber('0x'..hexArg:sub(1,1)) * 17, tonumber('0x'..hexArg:sub(2,2)) * 17, tonumber('0x'..hexArg:sub(3,3)) * 17
	elseif(string.len(hexArg) == 6) then
		return tonumber('0x'..hexArg:sub(1,2)), tonumber('0x'..hexArg:sub(3,4)), tonumber('0x'..hexArg:sub(5,6))
	else
		return 0, 0, 0
	end

end

-- for TLS: m:connect("192.168.11.118", secure-port, 1)

iptimer = tmr.create()
iptimer:register(5000, tmr.ALARM_AUTO, function()
	ipAddr = wifi.sta.getip()
	if ipAddr == nil or ipAddr == "0.0.0.0" then
		m:close()
		wifi.sta.disconnect()
		wifi.sta.connect()
		timeout = timeout + 1
	end
	
end)
iptimer:start()


wifi.eventmon.register(wifi.eventmon.STA_GOT_IP, function(T)
	timeout = 0
	print("\n\tSTA - GOT IP".."\n\tStation IP: "..T.IP.."\n\tSubnet mask: "..
 T.netmask.."\n\tGateway IP: "..T.gateway)
 	do_mqtt_connect()
 
end)

do_mqtt_connect()
--m:close();
-- you can call m:connect again
