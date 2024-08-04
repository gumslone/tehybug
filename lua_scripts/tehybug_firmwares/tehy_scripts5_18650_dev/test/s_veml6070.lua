-- quick and dirty code for VEML6070 UV-A Light Sensor 320-410nm
-- http://www.vishay.com/docs/84277/veml6070.pdf
-- el@ Datek Wireless AS
-- Doesn't use any bit manipulation so doesn't require bit module, but probably should :)
--sda, scl = 3, 4
--i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once
dev_addr = 0x38
-- Integration time Rset = 270Kohm = 5.265uW/cm2/step
-- Integration Time ("exposure") with 270 kOhm set resistor
-- https://github.com/kriswiner/VEML6070/blob/master/VEML6070.ino
--	itime = 0x02   -- 1/2T  = 90ms
--	itime = 0x06   -- 1T	= 180ms	(default)
--	itime = 0x0A   -- 2T	= 360ms
itime = 0x0E   -- 4T	= 720ms


local function _veml6070()
	i2c.start(0)
	i2c.address(0, dev_addr, i2c.TRANSMITTER)
	i2c.write(0,itime)
	i2c.stop(0)

	tmr.delay(720000)
-- get msb
	i2c.start(0)
	i2c.address(0, dev_addr + 1, i2c.RECEIVER)
	msb = i2c.read(0, 1)
	i2c.stop(0)
--get lsb	
	i2c.start(0)
	i2c.address(0, dev_addr, i2c.RECEIVER)
	lsb = i2c.read(0, 1)
	i2c.stop(0)
	return 256 * string.byte(msb) + string.byte(lsb)
end
local function _veml6070_sleep()
	--sleep	
	i2c.start(0)
	i2c.address(0, dev_addr, i2c.TRANSMITTER)
	i2c.write(0,0x03)
	i2c.stop(0)	
end


--print(uv)
local uv= _veml6070()
sensor.uv = uv
uv = nil
  --print(uv)
  --_veml6070_sleep()