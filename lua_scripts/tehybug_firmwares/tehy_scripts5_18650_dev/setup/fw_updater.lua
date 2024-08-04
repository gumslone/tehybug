--node.egc.setmode(node.egc.ON_ALLOC_FAILURE)
msg=nil
act=nil
mode=nil
out=nil
local servingFile = nil
--setup server
--print("Setting up Wifi AP")
wifi.setmode(wifi.STATIONAP)
wifi.nullmodesleep(false)
cfg={}
cfg.ssid=config_system.setup_ssid
wifi.ap.config(cfg)

wifi.ap.setip({ip="192.168.1.1",netmask="255.255.255.0",gateway="192.168.1.1"})

wifi.sta.sethostname(cfg.ssid) 
wifi.sta.connect()

for k,v in pairs(cfg) do cfg[k]=nil end cfg=nil
--print("Setting up webserver")
local srv=nil
srv=net.createServer(net.TCP)
srv:listen(80,function(c)

	c:on("receive", function(k,rq)
		--print("[New Request]")
		out=""
		msg=""
		
		local function trim(s)
			return (s:gsub("^%s*(.-)%s*$", "%1"))
		end
		local function unescape(s)
			s = string.gsub(s, "+", " ")
			s = string.gsub(s, "%%(%x%x)", function (h)
				return string.char(tonumber(h, 16))
			end)
			return trim(s)
		end
		

		local _, _, method, path, vars = string.find(rq, "([A-Z]+) (/[^?]*)%??(.*) HTTP")
		if method==nil then
			_, _, method, path=string.find(rq, "([A-Z]+) (.+) HTTP")
		end
		local _GET={}
		if vars ~= nil then
			for k, v in string.gmatch(vars, "([%w_-]+)=([^%&]+)&*") do
				print("["..k.."="..v.."]")
				_GET[k]=unescape(v)
			end
		end
		vars=nil
		act=_GET.action
		mode=_GET.mode
		
		if path ~= nil or path ~= "/" then
			rqFile=string.gsub(path, "/", "")
		else
			rqFile=nil
		end	
		
		-- prevent serving different files
		if servingFile ~= nil then
			--print("[Requesting a different file "..rqFile..", please stand by]")
			return
		else
			servingFile = rqFile
		end
		
		
		if(act ~= "execute") then
			node.output(nil)
		end
		--content
		if act=="file_append" then
			if _GET.file_line ~= nil and file.open(_GET.file_name, "a+") then
				file.write(_GET.file_line.."\n")
				file.close()
			end
			_GET.file_line = nil
			_GET.file_name = nil
			k:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n")
		elseif act=="activate_live_mode" then
			change_systatus(1)
			k:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n")
		elseif act=="execute" then
			msg='Command '.. string.format("%q", _GET.execute_command)..' executed successfully!<br>'
			function s_output(str)
				msg = msg..'<code>'..str..'</code>'
			end
			node.output(s_output, 1)
			node.input(_GET.execute_command)
			k:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/html\r\n\r\n")
		elseif act=="restart" then
			node.restart()
			return
		else
			filePos=0
			
			print("[rqFile "..rqFile.."]")
			if(rqFile==nil or rqFile == "") then
				msg = '<h1>'..wifi.sta.getip()..'</h1>'
			end
			k:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/html\r\n\r\n")
		end
		k=nil
	end)
	c:on("sent",function(k)

		
		
		if servingFile and rqFile then
			local prepend=''
			local append=''
			fd = file.open(rqFile,r)
			if fd then
				fd:seek("set",filePos)
				local partial_data=fd:read(512)
				fd:close()
				fd = nil
				if partial_data then
					filePos=filePos+#partial_data
					--print("["..filePos.." bytes sent]")
					k:send(partial_data)
					
					if string.len(partial_data)==512 then
						return
					end
				else
					
				end
			else
				if msg  ~= nil and msg ~= "" then
					k:send(" "..msg)
				end
				--print("[Error opening file"..rqFile.."]")
			end
		end
		--print("[Connection closed]")
		servingFile = nil
		method=nil
		path=nil
		rq=nil
		if _GET ~= nil then for k,v in pairs(_GET) do _GET[k]=nil end _GET=nil end
		act=nil
		prepend=nil
		append=nil
		partial_data=nil
		k:close()
		k = nil
		rqFile=nil

	end)
	c:on("disconnection", function(k) 
		servingFile = nil
	end)
end)


tmr.create():alarm(500, tmr.ALARM_AUTO, function()
	ipAddr = wifi.sta.getip()
	--print(tostring(ipAddr))
end)
--end of setup server