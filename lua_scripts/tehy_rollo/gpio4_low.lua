-- set pin index 1 to GPIO mode, and set the pin to high.
pin=4
gpio.mode(pin, gpio.OUTPUT)
gpio.write(pin, gpio.LOW)
print("LOW")
