-- dht_test.lua --
local calc_dewpoint(h,t) {
	local k = math.log(h/100) + (17.62 * t) / (243.12 + t);
	return 243.12 * k / (17.62 - k);
}
oled_data = ''
temp = ''
temp_imp = ''
humi = ''
qfe = ''
qfe_imp = ''
dew = ''
dew_imp = ''
--tmr.delay(2*1000000)
gpio.mode(3,gpio.OUTPUT)
gpio.write(3,gpio.LOW) --ground for new asair sensors

tmr.delay(2*1000000)
status, temp, humi, temp_dec, humi_dec = dht.read(4)
temp = string.format("%.2f", temp)
temp_imp = (temp*1.8)+32
temp_imp = string.format("%.2f", temp_imp)

dew = calc_dewpoint(humi,temp)
dew = string.format("%.2f", dew)
dew_imp = (dew*1.8)+32
dew_imp = string.format("%.2f", dew_imp)
print(status, temp, humi, temp_dec, humi_dec)

gpio.mode(3, gpio.INPUT)
gpio.trig(3,"down", debounce(onChange))
generate_sensor_data_page()
