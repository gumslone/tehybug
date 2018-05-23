--bme280 test
oled_data = ''
temp = ''
temp_imp = ''
humi = ''
qfe = ''
qfe_imp = ''
dew = ''
dew_imp = ''

--alt = 320
tmr.delay(2*1000000)
sda, scl = 3, 4
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once

--bme280.init(sda, scl)
bme280.setup(nil, nil, nil, 1)

tmr.delay(2*1000000)
--bme280.setup()
--T, P, H, QNH = bme280.read(alt)
--print(config.server_url)
T, P, H, QNH = bme280.read()
print(T, P, H, QNH)

if T ~= nil then
	--print(T, P, H, QNH)
	local Tsgn = (T < 0 and -1 or 1); T = Tsgn*T
	temp = string.format("%s%d.%02d", Tsgn<0 and "-" or "", T/100, T%100)
	temp_imp = (temp*1.8)+32
	temp_imp = string.format("%.2f", temp_imp)
	qfe = string.format("%d.%02d", P/1000, P%1000)
	qfe_imp = 0.02952998751 * qfe
	qfe_imp = string.format("%.3f", qfe_imp)
end
if H ~= nil then
	humi = string.format("%d.%02d", H/1000, H%1000)

  
	D = bme280.dewpoint(H, T)
	local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
	dew = string.format("%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
	dew_imp = (dew*1.8)+32
	dew_imp = string.format("%.2f", dew_imp)
end

i2c.stop(0)
