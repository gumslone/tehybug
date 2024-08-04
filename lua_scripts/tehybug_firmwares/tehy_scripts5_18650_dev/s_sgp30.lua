do
local sgp = require("lib_sgp30")

i2c.setup(0, 3, 4, i2c.SLOW) -- call i2c.setup() only once
sgp.initAirQuality(i2c, 0x58)

tmr.create():alarm(1000, tmr.ALARM_AUTO, function ()
	local co2PPM, vocPPB = sgp.measureAirQuality(i2c, 0x58)
end)

if sgp ~= nil then for k,v in pairs(sgp) do sgp[k]=nil end sgp=nil package.loaded['lib_sgp30'] = nil end
end