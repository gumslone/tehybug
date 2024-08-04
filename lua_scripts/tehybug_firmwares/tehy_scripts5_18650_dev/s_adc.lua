-- adc_test.lua --

--tmr.delay(2*1000000)
do
gpio.mode(7,gpio.OUTPUT)
gpio.write(7,gpio.LOW) --ground
local all_vdd = 0
for i=1,1000 do 
	all_vdd = all_vdd + adc.read(0)
	--print(all_vdd) 
end
sensor.xdc = all_vdd/1000

if sf.set ~= nil then
	--sf.do_calibration()
	sf.request()
end
end