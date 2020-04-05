led_status = 0

tmr.alarm(4, 500, 1, function() 
	if led_status == 0 then
		led_status = 1
		dofile("gpio4_low.lua")
	else
		led_status = 0
		dofile("gpio4_high.lua")
	end

end)