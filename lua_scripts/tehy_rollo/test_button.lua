buttonPin = 3 -- this is ESP-01 pin GPIO00
gpio.mode(buttonPin,gpio.INT,gpio.PULLUP)

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
	if gpio.read(buttonPin) == 0 then
		dofile("activate.lua")
	else
		dofile("deactivate.lua")
	end
end

gpio.trig(buttonPin,"both", debounce(onChange))