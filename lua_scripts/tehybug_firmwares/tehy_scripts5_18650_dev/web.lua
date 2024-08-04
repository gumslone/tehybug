-- starting webserver and set up the files to be served
--print("Wireless client")
wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)

if wifi.sta.sethostname("tehybug"..config_system.chip_id) == true then
	print("hostname was successfully changed to: tehybug"..config_system.chip_id)
end
wifi.sta.connect()

if sf == nil then sf = require("sensor_functions") end

local timeout=0
local first_time=1
tmr.create():alarm(1000, tmr.ALARM_AUTO, function() 
		
		timeout = timeout + 1
		if (first_time == 1 and timeout >= 10) or (first_time == 0 and timeout >= config.data_frequency*60) then
			timeout = 0
			first_time = 0
			sf.init()
			sf.first_sensor()
		end
end)

local srv = nil
srv=net.createServer(net.TCP)
srv:listen(80,function(c)

	c:on("receive", function(c,rq)
		print("[New Request]")
		
		local timeout = 0
		local _, _, method, path, vars = string.find(rq, "([A-Z]+) (.+)?(.+) HTTP")
		if method == nil then
			_, _, method, path = string.find(rq, "([A-Z]+) (.+) HTTP")
		end
		local _GET = {}
		
		c:send("HTTP/1.1 200 OK\r\nAllow-Control-Allow-Origin: *\r\nCache-Control: no-store, no-cache, must-revalidate\r\n\n") 
	
	end)
	c:on("sent",function(k)
		
		local buf = ""
		if config.web_output ~= nil then
			buf = sf.replace_placeholders(config.web_output)
		else
			buf = "web output is not defined"
		end
		k:send(buf)
		print("[Connection closed]")
		k:close()
	end)
	
	c:on("disconnection", function(k) 
		node.output(nil) -- un-regist the redirect output function, output goes to serial
		
		print("[Disconnected]")
		
		collectgarbage()
	end)
	
end)


local iptimer = tmr.create()
iptimer:register(5000, tmr.ALARM_AUTO, function()
	ipAddr = wifi.sta.getip()
	if ipAddr == nil or ipAddr == "0.0.0.0" then
		wifi.sta.disconnect()
		wifi.sta.connect()
	else
		print("Open in your browser http://"..tostring(wifi.sta.getip()).."/") 
		mdns.register("tehybug"..config_system.chip_id, { description="TeHyBug environmental data logger", service="http", port=80, location='Local network' })
	end
	
end)
iptimer:start()



--end of setup server