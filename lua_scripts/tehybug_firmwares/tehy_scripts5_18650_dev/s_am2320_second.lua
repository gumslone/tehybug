-- am2320_second.lua --
-- local sda, scl = 3, 4
do
i2c.setup(0, 4, 3, i2c.SLOW) -- call i2c.setup() only once
am2320.setup()

tmr.delay(2*1000000)
local rh, t = am2320.read()
i2c.stop(0)
sensor.temp = t/10
sensor.humi = rh/10

sensor.temp = string.format("%.2f", sensor.temp)

local D = bme280.dewpoint(sensor.humi*1000,sensor.temp*100)
local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
sensor.dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
--gpio.mode(3,gpio.INPUT)
end