


function round(n)

    return n % 1 >= 0.5 and math.ceil(n) or math.floor(n)

end

function initI2C()
   local sda = 4 -- GPIO2
   local scl = 3 -- GPIO0
   local sla = 0x3C
   i2c.setup(0, sda, scl, i2c.SLOW)
   disp = u8g.ssd1306_128x64_i2c(sla)
   disp:sleepOff()
end 

function initDisplay()
   disp:setFont(u8g.font_6x10)  
   --  disp:setDefaultForegroundColor()   
end

function alphabet()
    --disp:setFont(u8g.font_unifont)
    --disp:setFont(u8g.font_6x10)
    --disp:setFont(u8g.font_chikita)
    --disp:setFont(u8g.font_10x20)
    disp:setColorIndex(1)
    disp:setScale2x2()
    disp:drawStr(0, 7, "T:")
    
    if system_config ~= nil and system_config.display_temperature=='F' then
    	temp_f = (temp*1.8)+32
    	disp:drawStr(15, 7, round(temp_f).."F")
    else
		disp:drawStr(15, 7, round(temp).."C")
    end
    if humi ~= nil and humi ~= '' then
        disp:drawStr(0, 19, "H:")
        disp:drawStr(15, 19, round(humi).."50%")
    end
    if qfe ~= nil and qfe ~= '' then
	    disp:drawStr(0, 31, "P:")
	    --pinHg = 0.02952998751 � phPa
	    if system_config ~= nil and system_config.display_temperature=='F' then
	    	qfe_f = 0.02952998751 * qfe
	    	disp:drawStr(15, 31, round(qfe_f).."inHg")
	    else
	    	disp:drawStr(15, 31, round(qfe).."hPa")
	    end
    end
    disp:undoScale()
    --disp:sleepOn() 
end

function ticker()
   disp:firstPage()
   repeat
      alphabet()  
   until disp:nextPage() == false         
end


initI2C()  
initDisplay()
tmr.alarm(0, 5000, 1, function()
   ticker()
   tmr.stop(0)

     
end)
