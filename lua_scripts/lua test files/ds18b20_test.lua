--ds18b20 test
local http_sent = 0
oled_data = ''
temp = ''
temp_imp = ''
humi = ''
qfe = ''
qfe_imp = ''
dew = ''
dew_imp = ''

ds18b20.setup(4)
tmr.alarm(5, 10000, 1, function() 
	-- read all sensors and print all measurement results
	ds18b20.read(
		function(ind,rom,res,temp,tdec,par)
			print(ind,string.format("%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",string.match(rom,"(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+):(%d+)")),res,temp,tdec,par)
			if http_sent == 0 then
				tmr.unregister(5)
				temp = string.format("%.2f", temp)
				temp_imp = (temp*1.8)+32
				temp_imp = string.format("%.2f", temp_imp)
				http_sent = 1
				
			end
		end,{});
end)
