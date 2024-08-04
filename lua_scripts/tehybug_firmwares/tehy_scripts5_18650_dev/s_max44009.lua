do
local dev_addr = 0x4A

-- read regs for len number of bytes
-- return table with data
local function read_reg(reg_addr, len)
	local ret={}
	local c
	local x
	i2c.start(0)
	i2c.address(0, dev_addr ,i2c.TRANSMITTER)
	i2c.write(0,reg_addr)
	i2c.stop(0)
	i2c.start(0)
	i2c.address(0, dev_addr,i2c.RECEIVER)
	c=i2c.read(0,len)
	for x=1,len,1 do
		tc=string.byte(c,x)
		table.insert(ret,tc)
	end
	i2c.stop(0)
	return ret
end

local function init_max4409()	
	i2c.start(0)
	i2c.address(0, dev_addr, i2c.TRANSMITTER)
	-- Select configuration register
	i2c.write(0, 0x02)
	-- Continuous mode, Integration time = 800 ms
	i2c.write(0, 0x40)
	i2c.stop(0)
	
end

init_max4409()
tmr.delay(820)

local lux_high = read_reg(0x03,2)
local lux_low = read_reg(0x04,2)
local exponent = bit.rshift(bit.band(lux_high[1],0xF0),4)
--print(exponent)
local mantissa = bit.bor(bit.lshift(bit.band(lux_high[1], 0x0F), 4) , bit.band(lux_low[1] , 0x0F));
--print(mantissa)
local lux = (2 ^ exponent) * mantissa * 0.045;

sensor.lux=string.format("%.2f",lux)
end