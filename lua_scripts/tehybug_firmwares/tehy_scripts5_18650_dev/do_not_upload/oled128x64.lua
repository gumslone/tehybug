local function round(n)
	return n % 1 >= 0.5 and math.ceil(n) or math.floor(n)
end

local function initI2C()
	local sda, scl = 3, 4
	local sla = 0x3C
	--print('init i2c')
	i2c.stop(0)
	tmr.delay(10000)	
	i2c.setup(0, sda, scl, i2c.SLOW)
	--disp = u8g.ssd1306_128x64_i2c(sla)
	--print('after init i2c')
	if display ~= nil and display.type == "130" then
		print('1.30 inch')
		disp = u8g2.sh1106_i2c_128x64_noname(0, sla)
	else
		print('0.96 inch')
		disp = u8g2.ssd1306_i2c_128x64_noname(0, sla)
	end
	--disp:sleepOff()
	if display ~= nil and display.flip ~= nil then
		
		if display.flip == "90" then
			--disp:setRot90()
			disp:setDisplayRotation(u8g2.R1)
		elseif display.flip == "180" then
			--disp:setRot180()
			disp:setDisplayRotation(u8g2.R2)
		elseif display.flip == "270" then
			--disp:setRot270()
			disp:setDisplayRotation(u8g2.R3)
		end
		
		
	end
end

local function show_display_data()
	local line1_margin_x, line2_margin_x, line3_margin_x = 0,0,0
	--font_10x20,font_6x10,font_6x12,font_helvB14,font_chikita
	--font_chikita_tf,font_6x10_tf,font_6x12_te,font_10x20_te,font_helvB14_te
	--add _tf to font name for u2g
	--disp:setFont(u8g.font_6x12)
	--disp:setFont(u8g.font_10x20)
	disp:setFont(u8g2.font_10x20_te)
	--disp:setColorIndex(1)
	--disp:setScale2x2()
	--disp:drawStr(27, 9, string.char(0xb0)) -- degree symbol
	if sensor.temp ~= nil then
		sensor.temp = round(sensor.temp)
		sensor.temp_imp = round(sensor.temp_imp)
	end
	
	if sensor.qfe ~= nil then
		sensor.qfe = round(sensor.qfe)
		sensor.qfe_imp = string.format("%.2f", sensor.qfe_imp)
	end
	if sensor.qnh ~= nil then
		sensor.qnh = round(sensor.qnh)
		sensor.qnh_imp = string.format("%.2f", sensor.qnh_imp)
	end
	if sensor.air ~= nil then
		sensor.air = string.format("%.2f", sensor.air)
	end
	
	if sensor.humi ~= nil then
		sensor.humi = round(sensor.humi)
	end
	
	if sensor.dew ~= nil then
		sensor.dew = round(sensor.dew)
		sensor.dew_imp = round(sensor.dew_imp)
	end
	
	if display.line1 ~= nil then
		if display.line1_icon ~= nil then
			line1_margin_x = 27
		end
		--disp:drawStr(0+line1_margin_x, 9, sf.replace_placeholders(display.line1))
		disp:drawUTF8(0+line1_margin_x, 16, sf.replace_placeholders(display.line1))
	end
	if display.line2 ~= nil then
		if display.line2_icon ~= nil then
			line2_margin_x = 27
		end
		--disp:drawStr(0+line2_margin_x, 20, sf.replace_placeholders(display.line2))
		disp:drawUTF8(0+line2_margin_x, 39, sf.replace_placeholders(display.line2))
	end
	if display.line3 ~= nil then
		if display.line3_icon ~= nil then
			line3_margin_x = 27
		end
		--disp:drawStr(0+line3_margin_x, 31, sf.replace_placeholders(display.line3))
		disp:drawUTF8(0+line3_margin_x, 62, sf.replace_placeholders(display.line3))
	end
	
	--disp:undoScale()
	
	if display.line1_icon ~= nil and file.exists(display.line1_icon) then
		file.open(display.line1_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 0, 21, 20, xbm_data )
		xbm_data = nil
	end
	if display.line2_icon ~= nil and file.exists(display.line2_icon) then
		file.open(display.line2_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 22, 21, 20, xbm_data )
		xbm_data = nil
	end
	if display.line3_icon ~= nil and file.exists(display.line3_icon) then
		file.open(display.line3_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 44, 21, 20, xbm_data )
		xbm_data = nil
	end
	disp:sendBuffer()
end
initI2C()
show_display_data()
--print("display updated")
