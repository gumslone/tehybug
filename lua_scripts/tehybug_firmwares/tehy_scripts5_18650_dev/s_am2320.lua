-- am2320_test.lua --


if sf.set ~= nil then
	sf.do_calibration()
	sf.pcall("s_am2320_second.lc")
	sf.pcall("s_am2320_second.lc")
	sf.cir()
end