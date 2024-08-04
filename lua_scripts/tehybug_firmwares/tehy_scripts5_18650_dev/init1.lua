-- boot settings
dofile("boot.lua")

system = {status=1}
sensor = {}

if pcall(function () dofile("config_system.lua") end) then
else
	dofile("make_config_system.lc")
end

if adc.force_init_mode(adc.INIT_ADC) then
		node.restart()
		return -- don't bother continuing, the restart is scheduled
end

gpio.mode(3,gpio.INT,gpio.PULLUP)
function change_systatus(status)
	local fd = file.open("system.lua", "w+")
	if fd then
		print("status: "..status)
		fd:writeline('system={status='..status..'} ')
		fd:close()
		fd = nil
	end
end

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
		if system.status==0 then
			change_systatus(1)
			node.restart()
		else
			change_systatus(0)
			node.restart()
		end
		tmr.delay(500000)
	end
end
gpio.trig(3,"down", debounce(onChange))


tmr.create():alarm(2000, tmr.ALARM_SINGLE, function ()
	dofile("init2.lc")
end)
