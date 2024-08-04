
if pcall(function () dofile("system.lua") end) then else end
print((system.status * 1))
if system==nil or (system.status * 1)==0 or not file.exists("config.lua") then
	print("Enter configuration mode")
	if file.exists("setup.lc") then
		dofile("setup.lc")
	end
elseif (system.status * 1)==2 then
	change_systatus(0)
    print("generating file list")
	dofile("generate_filelist.lc")
	node.restart()
elseif (system.status * 1)==3 then
	change_systatus(2)
	print("wget mode")
	dofile("wget.lc")
elseif (system.status * 1)==4 then
	dofile("custom_do.lc")
elseif (system.status * 1)==5 then
	dofile("fw_updater.lc")
elseif pcall(function () dofile("config.lua") end) then
	if(config.request_type == "uart") then
		--print("uart mode")
		dofile("uart.lc")
	elseif(config.request_type == "web") then
		--print("web  mode")
		dofile("web.lc")
	elseif(config.request_type == "ap") then
		--print("ap mode")
		tmr.create():alarm(2000, tmr.ALARM_SINGLE, function ()
			dofile("ap.lc")
		end)
	else
		dofile("wifi_send_request.lc")
	end
else
	print("config.lua failed! Enter configuration mode")
	if file.exists("setup.lc") then
		dofile("setup.lc")
	end
end
