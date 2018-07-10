-- am2320_test.lua --
oled_data = ''
temp = ''
temp_imp = ''
humi = ''
qfe = ''
qfe_imp = ''
dew = ''
dew_imp = ''
uv = ''
--tmr.delay(2*1000000)
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

dew = string.format("%.2f", dew)
dew_imp = (dew*1.8)+32
dew_imp = string.format("%.2f", dew_imp)
print(temp, humi, dew)


oled_data = nil
temp = nil
temp_imp = nil
humi = nil
qfe = nil
qfe_imp = nil
dew = nil
dew_imp = nil
uv = nil
calibration = nil
status = nil
temp_dec = nil
humi_dec = nil
