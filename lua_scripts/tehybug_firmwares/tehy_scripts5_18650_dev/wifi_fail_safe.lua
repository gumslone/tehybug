-------- Station modes -------
    local STAMODE = {
    STATION_IDLE			= 0,
    STATION_CONNECTING		= 1,
    STATION_WRONG_PASSWORD	= 2,
    STATION_NO_AP_FOUND		= 3,
    STATION_CONNECT_FAIL	= 4,
    STATION_GOT_IP			= 5
    }
    ------------------------------
    if wifi.sta.status() == STAMODE.STATION_IDLE then
        print("Station: idling")
        --Get default Station configuration (NEW FORMAT)
        --local wifi_config = wifi.sta.getdefaultconfig(true)
        --for k,v in pairs(wifi_config) do 
        --    print(k..' = '..tostring(v))
        --end
        wifi_config = nil
    elseif wifi.sta.status() == STAMODE.STATION_CONNECTING then
        print("Station: connecting")
    elseif wifi.sta.status() == STAMODE.STATION_WRONG_PASSWORD then
        print("Station: wrong password") 
        print("Enter configuration mode")
        change_systatus(0)
        node.restart()
    elseif wifi.sta.status() == STAMODE.STATION_NO_AP_FOUND then
        print("Station: AP not found")
    elseif wifi.sta.status() == STAMODE.STATION_CONNECT_FAIL then
        print("Station: connection failed")
    end
    dofile("do_sleep.lc") 