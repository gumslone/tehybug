-- https://docs.rs/crate/sgp30/0.3.1/source/src/lib.rs
local M = {
	Command = {
		GetSerial = {0x36, 0x82},
		SelfTest = {0x20, 0x32},
		InitAirQuality = {0x20, 0x03},
		MeasureAirQuality = {0x20, 0x08},
		MeasureRawSignals = {0x20, 0x50},
		GetBaseline = {0x20, 0x15},
		SetBaseline = {0x20, 0x1E},
		SetHumidity = {0x20, 0x61},
		GetFeatureSet = {0x20, 0x2F}
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

function M.measureRawSignals(i2c, device)
	local msg={}
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0,M.Command.MeasureRawSignals)
	i2c.stop(0)
	tmr.delay(25*1000)

	i2c.start(0)
	i2c.address(0, device or M.DEVICE,i2c.RECEIVER)
	c=i2c.read(0,6)
	for x=1,6,1 do
		tc=string.byte(c,x)
		table.insert(msg,tc)
	end
	i2c.stop(0)

	local sout_H2 = msg[1]*256 + msg[2]
	local sout_EthOH = msg[4]*256 + msg[5]
	return sout_H2, sout_EthOH
end

function M.measureTest(i2c, device)
	local msg={}
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0,M.Command.SelfTest)
	i2c.stop(0)
	
	tmr.delay(220*1000)
	
  	i2c.start(0)
	i2c.address(0, device or M.DEVICE,i2c.RECEIVER)
	c=i2c.read(0,3)
	for x=1,3,1 do
		tc=string.byte(c,x)
		table.insert(msg,tc)
	end
	i2c.stop(0)
	local result = msg[1]*256 + msg[2]
	return result == 0xd400
end

-- humi must be absolute humidity
function M.setHumidity(i2c, device, humi)
	local msg={}
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0,M.Command.SetHumidity)
	i2c.write(0,humi)
	i2c.stop(0)
	
	tmr.delay(220*1000)
end

function M.readVersion(i2c, device)
	local msg={} 
	i2c.start(0)
	i2c.address(0, device or M.DEVICE, i2c.TRANSMITTER)
	i2c.write(0,M.Command.GetFeatureSet)
	i2c.stop(0)
	tmr.delay(2*1000)
	i2c.start(0)
	i2c.address(0, device or M.DEVICE,i2c.RECEIVER)
	c=i2c.read(0,3)
	for x=1,3,1 do
		tc=string.byte(c,x)
		table.insert(msg,tc)
	end
	i2c.stop(0)
	
  local lsb = msg[2]
  return lsb
end


return M