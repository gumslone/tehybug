    local ipAddr = wifi.sta.getip()
    if ipAddr == nil or ipAddr == "0.0.0.0" then
        wifi.sta.disconnect()
        ipAddr = nil
        sf.pcall("wifi_send_request.lc")
    else
        sf.first_sensor()
    end