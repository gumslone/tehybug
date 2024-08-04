--node.stripdebug(3)
--adc.force_init_mode(adc.INIT_VDD33)
adc.force_init_mode(adc.INIT_ADC)
file.remove('install.lua')
-----------------------------
print("generating file list")
--tmr.delay(2*1000000)
dofile('generate_filelist.lc')
print("make config system")
dofile("make_config_system.lc")
print("make sensor data file")

print("restarting")
node.dsleep(1*1000000)

