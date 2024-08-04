-- dht.lua --

if sf.set ~= nil then
	sf.pcall("s_dht_second.lc")
	sf.do_calibration()
	sf.pcall("s_dht_second.lc")
	sf.cir()
end
