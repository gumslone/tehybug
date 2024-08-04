i2c.setup(0, 3, 4, i2c.SLOW) 
veml6070.setup(0x0E)
print(veml6070.read(720))
veml6070.sleep()
