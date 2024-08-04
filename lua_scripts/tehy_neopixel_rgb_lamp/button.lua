--start--
--[static, blink, random_color, rainbow, rainbow_cycle, flicker, fire, fire_soft, halloween, circus_combustus, larson_scanner, color_wipe, random_dot]
--https://nodemcu.readthedocs.io/en/master/modules/ws2812-effects/#ws2812_effectsset_color
local buttonPin = 9 -- this is ESP-01 pin GPIO00
gpio.mode(buttonPin,gpio.INT)
local buttonPressed = 0
local buttonHoldTime = 0
local buttonPressedCounter = 0
local buttonState = 0
local timer_active = 0
local effect = 0

brtimer = tmr.create()
brtimer:register(1000, tmr.ALARM_AUTO, function()
	brightness = brightness + 20
	if brightness > 230 then
		brightness = system_config.brightness
	end
end)



function debounce (func)
	local last = 0
	local delay = 100000 
	-- 1000000 microseconds = 1 second
	return function (...)
		local now = tmr.now()
		if now - last < delay then return end

		last = now
		return func(...)
	end
end

function onChange()
	buttonHoldTime = (tmr.now()-buttonPressed)
	buttonState = gpio.read(buttonPin)
	if buttonState == 1 then
		if buttonHoldTime < 1000000 then
			buttonPressedCounter = buttonPressedCounter + 1
		else
			buttonPressedCounter = 0
		end
		
		if buttonPressedCounter == 2 and timer_active==0 then
			brtimer:start()
		else
			brtimer:stop()
			timer_active = 0
		end
		buttonPressed = tmr.now()
		--dofile("larson.lua")
	else
		
		if buttonHoldTime > 1000000 then
			
			
			if buttonPressedCounter == 1 then
				effect = effect + 1
				if effect > 9 then
					effect = 0
				end				
				if effect == 0 then
					dofile("default.lua")
				elseif effect == 1 then
					dofile("white.lua")
				elseif effect == 2 then
					dofile("lava.lua")
				
				elseif effect == 3 then
					dofile("fire.lua")
				
				elseif effect == 4 then
					dofile("dot.lua")
				elseif effect == 5 then
					dofile("rainbow.lua")
				elseif effect == 6 then
					dofile("circus.lua")
				elseif effect == 7 then
					dofile("random.lua")
				elseif effect == 8 then
					dofile("larson.lua")
				elseif effect == 9 then
					dofile("black.lua")
				end
				
				
				
			end
			if buttonPressedCounter > 5 and buttonHoldTime > 3000000 then
				brightness = 30
				dofile("default.lua")
			end
			if buttonPressedCounter == 0 and buttonHoldTime > 3000000 then
				dofile("black.lua")
				effect = 9
			end
			buttonPressedCounter = 0
		end
		
	end
	
end

gpio.trig(buttonPin, "both", debounce(onChange))
--finish--