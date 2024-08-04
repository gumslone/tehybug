msg=nil
act=nil
mode=nil
out=nil
servingFile = nil
dofile("setup_led.lc")
--setup server
print("Setting up Wifi AP")
wifi.setmode(wifi.STATIONAP)
wifi.nullmodesleep(false)
local cfg={}
cfg.ssid=config_system.setup_ssid
wifi.ap.config(cfg)
wifi.ap.setip({ip="192.168.1.1",netmask="255.255.255.0",gateway="192.168.1.1"})
print("Setting up webserver")

local srv=net.createServer(net.TCP)
srv:listen(80,function(c)

	c:on("receive", function(k,rq)
		--print("Start request "..(math.floor(collectgarbage("count"))))
		--print("[New Request]")
		out=""
		msg=""

		if config ~= nil then for k,v in pairs(config) do config[k]=nil end config=nil end
		if wifi_cnf ~= nil then for k,v in pairs(wifi_cnf) do wifi_cnf[k]=nil end wifi_cnf=nil end
		if calibration ~= nil then for k,v in pairs(calibration) do calibration[k]=nil end calibration=nil  end
		if sf ~= nil then for m,v in pairs(sf) do sf[m]=nil end sf=nil package.loaded['sensor_functions'] = nil end
		
		setf = require("setup_functions")
		
		local _, _, method, path, vars = string.find(rq, "([A-Z]+) (/[^?]*)%??(.*) HTTP")
		if method==nil then
			_, _, method, path=string.find(rq, "([A-Z]+) (.+) HTTP")
		end
		local _GET={}
		if vars ~= nil then
			for k, v in string.gmatch(vars, "([%w_-]+)=([^%&]+)&*") do
				print("["..k.."="..v.."]")
				_GET[k]=setf.unescape(v)
			end
		end
		vars=nil
		act=_GET.action
		mode=_GET.mode
		_GET.action=nil
		
		if path ~= nil then
			print("[Requesting path "..path)
		end
			
		rqFile=setf.rq_file(path)

		-- prevent serving different files
		if servingFile ~= nil then
			--print("[Requesting a different file "..rqFile..", please stand by]")
			return
		else
			servingFile = rqFile
		end

		if act ~= nil or act ~= "" then
			--print("actn")
			local actn = require("setup_actions")
			actn.run(act, _GET, setf, k)
			local stop = actn.stop
			if actn ~= nil then for k,v in pairs(actn) do actn[k]=nil end actn=nil end
			package.loaded['setup_actions'] = nil
			if stop == 1 then
				setf.send_headers(k,'ok')
        		servingFile = nil
        		rqFile = nil
				return
			end
		end

		if rqFile ~= nil and file.exists(rqFile) and method=="GET" then
			filePos = 0
			setf.send_headers(k,rqFile)
		else
			servingFile = nil
			rqFile = nil
			--print("[File "..path.." not found]")
			setf.send_headers(k,nil)
		end
		--k=nil
	end)
	c:on("sent",function(k)
		
		if servingFile then
			local prepend=''
			local append=''
			local fd = file.open(servingFile,r)
			if fd then
				fd:seek("set",filePos)
				local partial_data=fd:read(512)
				fd:close()
				fd = nil
				if partial_data then
					filePos=filePos+#partial_data
					----print("["..filePos.." bytes sent]")
					if act=="view_file" then
						if(filePos<=512) then
							prepend='<html><body><pre>'
						else
							prepend=''
						end
						if string.len(partial_data) < 512 and mode==nil then
							append='</pre><div style="margin-top: 15px"><a href="/file_list.html">&lt; File list</a><br><a href="/admin.html">&lt; Admin Menu</a></div><link rel="stylesheet" href="/style.css.gz"></body></html>'
						end
						k:send(prepend..setf.replace_html(partial_data)..append)
					elseif servingFile=="file_list.html" or not(string.find(servingFile, "%.html")) then
						k:send(partial_data)
					else
						k:send(setf.replace_config(partial_data,servingFile))
					end
					if string.len(partial_data)==512 then
						return
					end
				else
					
				end
			else
				----print("[Error opening file"..rqFile.."]")
			end
		end
		--print("[Connection closed]")
		
		-- create sensor file
		--node.output(nil) -- un-regist the redirect output function, output goes to serial
		if servingFile~= nil and string.find(servingFile, "sensor_data") then
			if not tmr.create():alarm(500, tmr.ALARM_SINGLE, function()
				if setf ~= nil then for k,v in pairs(setf) do setf[k]=nil end setf=nil end
				package.loaded['setup_functions'] = nil
				if sf == nil then sf = require("sensor_functions") end
				sf.test='_test'
				sf.init()
				sf.first_sensor()
			end)
			then
			
			end
			
		end

		filePos=0
		prepend=nil
		append=nil
		partial_data=nil
		node.output(nil)
		k:close()
		k = nil
		servingFile = nil
		--print("END Request "..(math.floor(collectgarbage("count"))))
		--print("HEAP collectgarbage: "..node.heap())
	end)
	c:on("disconnection", function(k) 
		k=nil
		rq=nil
		servingFile = nil
		rqFile = nil
		method=nil
		path=nil
		filePos=0
		if _GET ~= nil then for k,v in pairs(_GET) do _GET[k]=nil end _GET=nil end
		act=nil
		msg=nil
		node.output(nil)
		collectgarbage("collect")
	end)
end)
print("Please connect to: "..config_system.setup_ssid.." - AP IP: "..wifi.ap.getip())
--end of setup server