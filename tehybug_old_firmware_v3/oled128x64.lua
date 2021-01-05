function round(n)
	return n % 1 >= 0.5 and math.ceil(n) or math.floor(n)
end

function initI2C()
	local sda = 4 -- GPIO2
	local scl = 3 -- GPIO0
	local sla = 0x3C
	i2c.setup(0, sda, scl, i2c.SLOW)
	--disp = u8g.ssd1306_128x64_i2c(sla)

	disp = u8g2.ssd1306_i2c_128x64_noname(0, sla)
	--disp:sleepOff()
	if display_config ~= nil and display_config.flip ~= nil then
		
		if display_config.flip == "90" then
			--disp:setRot90()
			disp:setDisplayRotation(u8g2.R1)
		elseif display_config.flip == "180" then
			--disp:setRot180()
			disp:setDisplayRotation(u8g2.R2)
		elseif display_config.flip == "270" then
			--disp:setRot270()
			disp:setDisplayRotation(u8g2.R3)
		end
		
		
	end
end

function show_display_data()
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
	if temp ~= nil and temp ~= '' then
		temp = round(temp)
		temp_imp = round(temp_imp)
	end
	
	if qfe ~= nil and qfe ~= '' then
		qfe = round(qfe)
		--qfe_imp = round(qfe_imp)
		qfe_imp = string.format("%.2f", qfe_imp)
	end
	
	if air ~= nil and air ~= '' then
		air = string.format("%.2f", air)
	end
	
	if humi ~= nil and humi ~= '' then
		humi = round(humi)
	end
	
	if dew ~= nil and dew ~= '' then
		dew = round(dew)
		dew_imp = round(dew_imp)
	end
	
	if display_config.line1 ~= nil then
		if display_config.line1_icon ~= nil then
			line1_margin_x = 27
		end
		--disp:drawStr(0+line1_margin_x, 9, replace_placeholders(display_config.line1))
		disp:drawStr(0+line1_margin_x, 16, replace_placeholders(display_config.line1))
	end
	if display_config.line2 ~= nil then
		if display_config.line2_icon ~= nil then
			line2_margin_x = 27
		end
		--disp:drawStr(0+line2_margin_x, 20, replace_placeholders(display_config.line2))
		disp:drawStr(0+line2_margin_x, 39, replace_placeholders(display_config.line2))
	end
	if display_config.line3 ~= nil then
		if display_config.line3_icon ~= nil then
			line3_margin_x = 27
		end
		--disp:drawStr(0+line3_margin_x, 31, replace_placeholders(display_config.line3))
		disp:drawStr(0+line3_margin_x, 62, replace_placeholders(display_config.line3))
	end
	
	--disp:undoScale()
	
	if display_config.line1_icon ~= nil and file.exists(display_config.line1_icon) then
		file.open(display_config.line1_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 0, 21, 20, xbm_data )
	end
	if display_config.line2_icon ~= nil and file.exists(display_config.line2_icon) then
		file.open(display_config.line2_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 22, 21, 20, xbm_data )
	end
	if display_config.line3_icon ~= nil and file.exists(display_config.line3_icon) then
		file.open(display_config.line3_icon, "r")
		xbm_data = file.read()
		file.close()
		disp:drawXBM( 0, 44, 21, 20, xbm_data )
	end
	disp:sendBuffer()
end

function ticker()
	--disp:firstPage()
	--repeat
	  show_display_data()  
	--until disp:nextPage() == false
end

initI2C()
ticker()
--print("display updated")
