--ds18b20 test
do
if sf.current_sensor==1 then
	ds18b20.setup(4)
else
	ds18b20.setup(7)
end
local dstmr = tmr.create()
local http_sent = 0
dstmr:alarm(1000, tmr.ALARM_AUTO, function()
	-- read all sensors and print all measurement results
	ds18b20.read(
		function(ind,rom,res,tem,tdec,par)
			--print(ind,string.format("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",string.match(rom,"(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+)")),res,tem,tdec,par)
			if http_sent == 0 then
				dstmr:unregister()
				sensor.temp = string.format("%.2f", tem)
				
				http_sent = 1
				if sf.set ~= nil then
					sf.do_calibration()
					sf.second_sensor()
					sf.cir()
				end
			end
		end,{});
end)
end