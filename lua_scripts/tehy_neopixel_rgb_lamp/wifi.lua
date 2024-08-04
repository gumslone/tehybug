-------- Station modes -------
local STAMODE = {
STATION_IDLE            = 0,
STATION_CONNECTING      = 1,
STATION_WRONG_PASSWORD  = 2,
STATION_NO_AP_FOUND     = 3,
STATION_CONNECT_FAIL    = 4,
STATION_GOT_IP          = 5
}
------------------------------
local timeout=0
wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)
--connect to Access Point (DO NOT save config to flash)
station_cfg={}
station_cfg.ssid = system_config.wifi_ssid
station_cfg.pwd = system_config.wifi_pass
station_cfg.save=false
station_cfg.auto=true
wifi.sta.config(station_cfg)
wifi.sta.sethostname(system_config.device_hostname) 
--wifi.sta.connect()
--wifi.sta.autoconnect(1)
mytimer = tmr.create()
mytimer:register(1000, tmr.ALARM_AUTO, function()
    ipAddr = wifi.sta.getip()
    if ipAddr == nil or ipAddr == "0.0.0.0" then
        print("IP unavaiable, waiting... " .. timeout)
        timeout = timeout + 1
        if timeout >= 99 then
            
            print("Timeout!")
            if wifi.sta.status() == STAMODE.STATION_IDLE then
                print("Station: idling")

            elseif wifi.sta.status() == STAMODE.STATION_CONNECTING then
                print("Station: connecting")

            elseif wifi.sta.status() == STAMODE.STATION_WRONG_PASSWORD then
                
            elseif wifi.sta.status() == STAMODE.STATION_NO_AP_FOUND then
                print("Station: AP not found")

            elseif wifi.sta.status() == STAMODE.STATION_CONNECT_FAIL then
                print("Station: connection failed")

            else

            end
            --tmr.unregister(1)
            node.restart();
        end
        
    else
        
        print("IP avaiable, connected... ")
        mytimer:stop()
        timeout = 0

        collectgarbage()
        
        dofile("mqtt.lua")
   

    end
end)
mytimer:start()