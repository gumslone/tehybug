-- dht_second.lua --
--tmr.delay(2*1000000)
do
gpio.mode(3,gpio.OUTPUT)
gpio.write(3,gpio.LOW) --ground for new asair sensors

tmr.delay(2*1000000)
local status, temp, humi, temp_dec, humi_dec = dht.read(4)
sensor.humi = humi
sensor.temp = string.format("%.2f", temp)

local D = bme280.dewpoint(sensor.humi*1000,sensor.temp*100)
local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
sensor.dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
end
