--start--
--[static, blink, random_color, rainbow, rainbow_cycle, flicker, fire, fire_soft, halloween, circus_combustus, larson_scanner, color_wipe, random_dot]
--https://nodemcu.readthedocs.io/en/master/modules/ws2812-effects/#ws2812_effectsset_color
ws2812_effects.stop()
-- init the ws2812 module
ws2812.init(ws2812.MODE_SINGLE)
-- create a buffer, 60 LEDs with 3 color bytes
strip_buffer = ws2812.newBuffer(256, 3)
-- init the effects module, set color to red and start blinking
ws2812_effects.init(strip_buffer)
ws2812_effects.set_brightness(0)
ws2812_effects.set_color(0,0,0)
ws2812_effects.set_mode("static")
ws2812_effects.start()
--finish--