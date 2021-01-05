-- dht.lua --
tmr.delay(2*1000000)
gpio.mode(3,gpio.OUTPUT,gpio.PULLUP)
gpio.write(3,gpio.LOW) --ground for new asair sensors

tmr.delay(1*1000000)
status, temp, humi, temp_dec, humi_dec = dht.read(4)
print(status, temp, humi, temp_dec, humi_dec)


if system_config.display_type~=nil and system_config.display_type~='none' then
	dofile("oled_128x64.lua")
end

config.server_url = string.gsub(config.server_url, "%%temp%%", temp)
config.server_url = string.gsub(config.server_url, "%%humi%%", humi)
config.server_url = string.gsub(config.server_url, "%%temp_dec%%", temp_dec)
config.server_url = string.gsub(config.server_url, "%%humi_dec%%", humi_dec)
config.server_url = string.gsub(config.server_url, "%%sensor%%", system_config.bug_key)
config.server_url = string.gsub(config.server_url, "%%chipid%%", system_config.chip_id)
config.server_url = string.gsub(config.server_url, "%%fw%%", system_config.firmware_version)
--print(config.server_url)
http.get(config.server_url, nil, function(code, data)
    if (code < 0) then
      print("HTTP request failed")
    else
      print(code, data)
    end
    node.dsleep(config.data_frequency*60*1000000)
end)


-- rtctime.dsleep(2*60*1000000)
-- tmr.delay(10*1000000)
-- node.dsleep(2*60*1000000)
