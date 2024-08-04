sgp = require("lib_sgp30")
sda, scl = 3, 4
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once
sgp.initAirQuality(i2c, 0x58)

tmr.create():alarm(1000, tmr.ALARM_AUTO, function ()
print(sgp.measureAirQuality(i2c, 0x58))
--print(sgp.measureRawSignals(i2c, 0x58))
--print(sgp.measureTest(i2c, 0x58))
--print(sgp.readVersion(i2c, 0x58))

end)