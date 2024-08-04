-- https://docs.rs/crate/sgp30/0.3.1/source/src/lib.rs
local M = {
	Command = {
		InitAirQuality = {0x20, 0x03},
		MeasureAirQuality = {0x20, 0x08},
  },
  DEVICE = 0x58
}
function M.initAirQuality(i2c, device)
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0, M.Command.InitAirQuality)
	i2c.stop(0)
	tmr.delay(10*1000)
end

function M.measureAirQuality(i2c, device)
	local msg={}
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0,M.Command.MeasureAirQuality)
	i2c.stop(0)
	tmr.delay(12*1000)
	
	i2c.start(0)
	i2c.address(0, device or M.DEVICE,i2c.RECEIVER)
	c=i2c.read(0,6)
	for x=1,6,1 do
		tc=string.byte(c,x)
		table.insert(msg,tc)
	end
	i2c.stop(0)

	local co2PPM = msg[1]*256 + msg[2]
	local vocPPB = msg[4]*256 + msg[5]
	return co2PPM, vocPPB
end

return M