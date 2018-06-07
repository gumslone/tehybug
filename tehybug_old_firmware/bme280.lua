--alt = 320
tmr.delay(2*1000000)
sda, scl = 3, 4
i2c.setup(0, sda, scl, i2c.SLOW) -- call i2c.setup() only once

bme280.init(sda, scl)
tmr.delay(3*1000000)
--bme280.setup()
--T, P, H, QNH = bme280.read(alt)
--print(config.server_url)
temp = ''
humi = ''
qfe = ''
dew = ''
T, P, H, QNH = bme280.read()
--print(T, P, H, QNH)

if T ~= nil then
	--print(T, P, H, QNH)
	
	local Tsgn = (T < 0 and -1 or 1); T = Tsgn*T
	temp = string.format("%s%d.%02d", Tsgn<0 and "-" or "", T/100, T%100)
	qfe = string.format("%d.%02d", P/1000, P%1000)
end
if H ~= nil then
    humi = string.format("%d.%02d", H/1000, H%1000)

  
    D = bme280.dewpoint(H, T)
    local Dsgn = (D < 0 and -1 or 1); D = Dsgn*D
    dew = string.format("dew_point=%s%d.%02d", Dsgn<0 and "-" or "", D/100, D%100)
end


if system_config.display_type~=nil and system_config.display_type~='none' then
	dofile("oled_128x64.lua")
end


config.server_url = string.gsub(config.server_url, "%%temp%%", temp)
config.server_url = string.gsub(config.server_url, "%%humi%%", humi)
config.server_url = string.gsub(config.server_url, "%%qfe%%", qfe)
config.server_url = string.gsub(config.server_url, "%%dew%%", dew)
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
