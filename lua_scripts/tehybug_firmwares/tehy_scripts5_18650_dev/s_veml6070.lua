
veml6070.setup(0x0E)
--print(uv)
sensor.uv = veml6070.read(750)
veml6070.sleep()
--print("UV steps: "..uv)
