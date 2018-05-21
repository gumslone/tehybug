-- dht_test.lua --
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

D = bme280.dewpoint(humi*1000,temp*100)
local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)

dew = string.format("%.2f", dew)
dew_imp = (dew*1.8)+32
dew_imp = string.format("%.2f", dew_imp)
print(status, temp, humi, dew)

gpio.mode(3, gpio.INPUT)
gpio.trig(3,"down", debounce(onChange))
generate_sensor_data_page()
