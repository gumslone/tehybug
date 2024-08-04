local setf = {}
setf.set = 1
function setf.unescape(s)
	s = string.gsub(s, "+", " ")
	s = string.gsub(s, "%%(%x%x)", function (h)
		return string.char(tonumber(h, 16))
	end)
	return setf.trim(s)
end

function setf.trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

function setf.replace_config(s,f)
	
	for k,v in pairs(config_system) do
		v = string.gsub(v, '%%', '%%%%')
		s = string.gsub(s, "config_system%."..k, v, 1)
	end
	
	if msg ~= nil and msg ~= "" then
		s = string.gsub(s, '<%!%-%-message%-%->', msg,1)
	end
	
	if f == 'node_info.html' then
		local majorVer, minorVer, devVer, chipid, flashid, flashsize, flashmode, flashspeed = node.info()
		s = string.gsub(s, "node_info_majorVer", majorVer,1)
		s = string.gsub(s, "node_info_minorVer", minorVer,1)
		s = string.gsub(s, "node_info_devVer", devVer,1)
		s = string.gsub(s, "node_info_chipid", chipid,1)
		s = string.gsub(s, "node_info_flashid", flashid,1)
		s = string.gsub(s, "node_info_flashsize", flashsize,1)
		s = string.gsub(s, "node_info_flashmode", flashmode,1)
		s = string.gsub(s, "node_info_flashspeed", flashspeed,1)
		s = string.gsub(s, "node_dsleepmax", ((node.dsleepMax()/1000000)/60),1)
		
	end
	local ipAddr = wifi.sta.getip()
	if ipAddr == nil or ipAddr == "0.0.0.0" then
		s = string.gsub(s, "node_ip", '',1)
	else
		s = string.gsub(s, "node_ip", ipAddr,1)
	end
	return(s)
end

function setf.replace_html(s)
	s = string.gsub(s, "<", "&lt")
	s = string.gsub(s, ">", "&gt")
	return(s)
end

function setf.rq_file(s)
	local f
	if s ~= nil and s~="/" then
		f=string.gsub(s, "/", "")
	else
		f="index.html"
	end
	if file.exists(f..'.gz') then
		f = f..'.gz'
	end
	return(f)
end

function setf.send_headers(k,f_name)

	if f_name==nil then
		k:send("HTTP/1.1 404 Not Found\r\nCache-Control: private, max-age=2592000\r\n\r\n")
	elseif f_name=='ok' then
		k:send("HTTP/1.1 200 OK\r\n\r\n")
	else
		local ct = ''
		if string.find(f_name, "%.css") then
			ct="text/css"
		elseif string.find(f_name, "%.js") then
			ct="text/javascript"
		else
			ct="text/html"
		end
		
		if string.find(f_name, "%.gz") then
			k:send("HTTP/1.1 200 OK\r\nContent-Type: "..ct.."\r\nCache-Control: private, max-age=2592000\r\nContent-Encoding: gzip\r\n\r\n")
		else
			k:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: "..ct.."; charset=\"utf-8\"\r\n\r\n")
		end
	end
end

return setf