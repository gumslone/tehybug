



-- init mqtt client without logins, keepalive timer 120s
timeout = 0
connected = 0
chipid = node.chipid()
topic = "/rollo"
nodename = system_config.device_hostname
broker = system_config.broker
port = 1883




-- init mqtt client without logins, keepalive timer 120s
m = mqtt.Client(nodename, 120)

-- init mqtt client with logins, keepalive timer 120sec

-- setup Last Will and Testament (optional)
-- Broker will publish a message with qos = 0, retain = 0, data = "offline" 
-- to topic "/lwt" if client don't send keepalive packet
m:lwt("/rollo", "OFFLINE "..nodename, 0, 0)

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
			if data == "down "..nodename then
				dofile("down.lua")
				change_rollo_systatus("down")
				client:publish(topic, nodename.." going down", 0, 0, function(client) print("sent") end)
			elseif data == "up "..nodename then
				dofile("up.lua")
				change_rollo_systatus("up")
				client:publish(topic, nodename.." going up", 0, 0, function(client) print("sent") end)
			elseif data == "status "..nodename then
				dofile("rollo_status.lua")
				client:publish(topic, nodename.." status is : "..rollo.status, 0, 0, function(client) print("sent") end)
			elseif data == "check" then
				client:publish(topic, nodename, 0, 0, function(client) print("sent") end)
			elseif data == "reboot "..nodename or data == "restart "..nodename then
				client:publish(topic, nodename.." going to reboot", 0, 0, function(client) print("sent") end)
				node.restart()
			else
				s = split(data, " ")
				print(s[1])
			    if s[1] == "wget" then
					local file_name = s[2]:match( "([^/]+)$" )
					w_get(s[2],file_name,nil,nil)
				elseif s[1]=="compile" then
					node.compile(s[2])
				elseif key=="dofile" then
					dofile(s[2])
				elseif s[1]=="remove" then
					file.remove(s[2])
				end
				
			
			end
		end
	end
end)



function change_rollo_systatus(status)
	local fd = file.open("rollo_status.lua", "w+")
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
					--print('File '..file_name..' is valid!')
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
					--print ('File '..file_name..' is NOT valid! Removing....')
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
