-- starting webserver and set up the files to be served
local servingFile = nil
msg = ""
act = ""
--setup server
print("Setting up Wifi AP")
wifi.setmode(wifi.STATIONAP)
wifi.nullmodesleep(false)
wifi.setphymode(wifi.PHYMODE_N)
local cfg = {}
cfg.ssid = config_system.setup_ssid .. '-AP-Mode'

if config.ap_password ~= nil and string.len(config.ap_password) > 7 then
	print('AP Password: '..config.ap_password)
	cfg.pwd = config.ap_password
end
wifi.ap.config(cfg)



wifi.ap.setip({ip="192.168.1.1",netmask="255.255.255.0",gateway="192.168.1.1"})
print("Setting up webserver")

local srv=net.createServer(net.TCP)
srv:listen(80,function(c)

	c:on("receive", function(c,rq)
		--print("[New Request]")
		
		out = ""
		msg = ""
		node.output(nil) 

		local _, _, method, path, vars = string.find(rq, "([A-Z]+) (.+)?(.+) HTTP")
		if method == nil then
			_, _, method, path = string.find(rq, "([A-Z]+) (.+) HTTP")
		end

		rq = nil
		if path=="/" or path=="/index.html" then
			rqFile = "ap.html"
		elseif path ~= nil then
			rqFile = string.gsub(path, "/", "")
		else
			rqFile = "ap.html"
		end
		
		if file.exists(rqFile..'.gz') then
			rqFile = rqFile..'.gz'
		end

		if file.exists(rqFile) then
			filePos=0
			print("[Sending file "..rqFile.."]")
			
			-- prevent serving different files
			if servingFile ~= nil then
				--print("[Requesting a different file "..rqFile..", please stand by]")
				return
			else
				servingFile = rqFile
			end
			
			if string.find(rqFile, ".css") then
				ct = "text/css"
			elseif string.find(rqFile, ".txt") then
					ct = "text/plain"
			else
				ct = "text/html"
				rqFile = "ap.html" --override existing files, disable access to config mode files
			end
			
			

			if string.find(rqFile, ".gz") then
				c:send("HTTP/1.1 200 OK\r\nContent-Type: "..ct.."\r\nCache-Control: private, max-age=2592000\r\nContent-Encoding: gzip\r\n\r\n")
			else
				c:send("HTTP/1.1 200 OK\r\nContent-Type: "..ct.."\r\nCache-Control: no-store, no-cache, must-revalidate\r\n\n") 
			end

		else
			print("[File "..path.." not found]")
			c:send("HTTP/1.1 404 Not Found\r\nCache-Control: private, max-age=2592000\r\n\r\n")
		end
	
	end)
	c:on("sent",function(k)
		
		if rqFile then
			if file.open(rqFile,r) then
				file.seek("set",filePos)
				local partial_data=file.read(1024)
				file.close()
				if partial_data then
					filePos=filePos+#partial_data
					print("["..filePos.." bytes sent]")
					k:send((partial_data))
					
					if string.len(partial_data)==1024 then
						partial_data = nil
						return
					end
					
				else
					
				end
			else
				print("[Error opening file"..rqFile.."]")
			end

		end
		
		print("[Connection closed]")
		filePos=nil
		path = nil
		act = nil
		msg = nil
		servingFile = nil
		k:close()
		if rqFile=="ap.html" then
			generate_file = "ap.html"
		end
		rqFile = nil
	end)
	
	c:on("disconnection", function(k) 
		node.output(nil) -- un-regist the redirect output function, output goes to serial
		servingFile = nil
		print("[Disconnected]")
		
		collectgarbage()
	end)

	
end)

print("Please connect to: " .. config_system.setup_ssid .. "-AP-Mode - IP: " .. wifi.ap.getip())


if not tmr.create():alarm(10*1000, tmr.ALARM_AUTO, function()
	if sf == nil then sf = require("sensor_functions") end
	sf.test='_test'
	sf.init()
	sf.first_sensor()
end)
then

end



wifi.eventmon.register(wifi.eventmon.AP_STADISCONNECTED, function(T)
 print("\n\tAP - STATION DISCONNECTED".."\n\tMAC: "..T.MAC.."\n\tAID: "..T.AID)
end)
--end of setup server
