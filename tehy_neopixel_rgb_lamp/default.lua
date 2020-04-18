--enum ws2812_effects_type {
--  STATIC,
--  BLINK,
--  GRADIENT,
--  GRADIENT_RGB,
--  RANDOM_COLOR,
--  RAINBOW,
--  RAINBOW_CYCLE,
--  FLICKER,
--  FLICKER,
--  FLICKER_SOFT,
--  FLICKER_INTENSE,
--  HALLOWEEN,
--  CIRCUS_COMBUSTUS,
--  LARSON_SCANNER,
--  CYCLE,
--  COLOR_WIPE,
--  RANDOM_DOT
--};
-- init the ws2812 module
ws2812.init(ws2812.MODE_SINGLE)
-- create a buffer, 60 LEDs with 3 color bytes
strip_buffer = ws2812.newBuffer(256, 3)
ws2812_effects.init(strip_buffer)
ws2812_effects.set_brightness(brightness)
-- init the effects module, set color to red and start blinking
-- rainbow cycle with three repetitions
ws2812_effects.set_mode("gradient", string.char(0,200,0,200,200,0,200,200,200))
-- init the ws2812 module
ws2812_effects.set_speed(1)
ws2812_effects.start()