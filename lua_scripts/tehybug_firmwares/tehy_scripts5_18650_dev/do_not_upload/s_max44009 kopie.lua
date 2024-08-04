max44009.setup(0x4A)
local fct, lux = max44009.read(820)
sensor.lux = string.format("%.2f", lux)
fct,lux = nil, nil