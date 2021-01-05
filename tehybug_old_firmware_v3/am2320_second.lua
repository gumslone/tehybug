-- am2320_second.lua --
sda, scl = 4, 3
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once
am2320.setup()


tmr.delay(2*1000000)
local rh, t = am2320.read()
temp = t/10
humi = rh/10

if calibration ~= nil and calibration.temp ~= nil then
	temp = temp + calibration.temp
end
if calibration ~= nil and calibration.humi ~= nil then
	humi = humi + calibration.humi
end
temp = string.format("%.2f", temp)
temp_imp = (temp*1.8)+32
temp_imp = string.format("%.2f", temp_imp)

D = bme280.dewpoint(humi*1000,temp*100)
local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)

dew_imp = (dew*1.8)+32
dew_imp = string.format("%.2f", dew_imp)
--gpio.mode(3,gpio.INPUT)
