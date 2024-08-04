print("aaa")
i2c.setup(0, 3, 4, i2c.SLOW) -- call i2c.setup() only once
max44009.setup(0x40)
print(max44009.read(1600))


