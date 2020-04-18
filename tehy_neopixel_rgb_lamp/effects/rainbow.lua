--start--
--[static, blink, random_color, rainbow, rainbow_cycle, flicker, fire, fire_soft, halloween, circus_combustus, larson_scanner, color_wipe, random_dot]
--https://nodemcu.readthedocs.io/en/master/modules/ws2812-effects/#ws2812_effectsset_color
ws2812_effects.stop()
ws2812.init(ws2812.MODE_SINGLE)
-- create a buffer, 60 LEDs with 3 color bytes
strip_buffer = ws2812.newBuffer(256, 3)
ws2812_effects.init(strip_buffer)
ws2812_effects.set_brightness(brightness)
-- rainbow cycle with two repetitions
ws2812_effects.set_mode("rainbow_cycle", 2)
--ws2812_effects.set_mode("random_color")
--ws2812_effects.set_mode("circus_combustus")
--ws2812_effects.set_mode("fire")
--ws2812_effects.set_mode("halloween")
--ws2812_effects.set_mode("color_wipe")
--ws2812_effects.set_mode("larson_scanner")
--ws2812_effects.set_color(0,255,0)
--ws2812_effects.set_color(g, r, b, [w])
-- gradient from red to yellow to red
--ws2812_effects.set_mode("gradient", string.char(0,200,0,200,200,0,0,200,0))
-- random dots with fading
--ws2812_effects.set_mode("random_dot",3)
--ws2812_effects.set_mode("cycle",3)
-- init the ws2812 module
ws2812_effects.set_speed(200)
-- rainbow cycle with three repetitions
--ws2812_effects.set_delay(2000)
ws2812_effects.start()
--finish--