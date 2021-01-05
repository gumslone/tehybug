dofile("system_config.lua")
dofile("system_status.lua")

gpio.mode(3,gpio.INT,gpio.PULLUP)


function debounce (func)
	local last = 0
	local delay = 200000

	return function (...)
		local now = tmr.now()
		if now - last < delay then return end

		last = now
		return func(...)
	end
end

function onChange()
	if gpio.read(3) == 0 then
		print("Button Pressed!")
		if(system_status.system_status==1) then
			file.remove("system_status.lua")
			file.open("system_status.lua", "w")
			file.writeline('system_status = { ')
			file.writeline('system_status = 0;')
			file.writeline('} ')
			file.close()
			node.restart()
		else
			file.remove("system_status.lua")
			file.open("system_status.lua", "w")
			file.writeline('system_status = { ')
			file.writeline('system_status = 1;')
			file.writeline('} ')
			file.close()
			node.restart()
		end
		tmr.delay(500000)
	end
end
gpio.trig(3,"down", debounce(onChange))


if (system_status.system_status * 1)==0 or not file.exists("config.lua") then
	print("Enter configuration mode")
	dofile("setup.lc")
elseif (system_status.system_status * 1)==2 then
	system_status = nil
	system_config = nil
	file.remove("system_status.lua")
	file.open("system_status.lua", "w")
	file.writeline('system_status = { ')
	file.writeline('system_status = 0;')
	file.writeline('} ')
	file.close()
	dofile("generate_filelist.lua")
	node.restart()
elseif (system_status.system_status * 1)==3 then
	system_status = nil
	system_config = nil
	file.remove("system_status.lua")
	file.open("system_status.lua", "w")
	file.writeline('system_status = { ')
	file.writeline('system_status = 2;')
	file.writeline('} ')
	file.close()
	dofile("wget.lc")
elseif (system_status.system_status * 1)==4 then
	dofile("custom_do.lc")
else
	dofile("config.lua")
	if(config.request_type == "display") then
		--print("display only mode")
		dofile("display_only.lc")
	elseif(config.request_type == "web") then
		--print("web  mode")
		dofile("web.lc")
	else
		dofile("wifi_send_request.lc")
	end
end
