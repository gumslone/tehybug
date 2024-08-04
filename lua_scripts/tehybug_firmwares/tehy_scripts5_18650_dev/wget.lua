local function w_get(url,file_name)
    http.get(url, nil, function(code, data)
        if (code < 0) then
            return ("HTTP request failed")
        else
            print(code, data)
            -- open file in 'w' mode
            if file.open('_wget', "w") then
                -- write text to the file
                file.write(data)
                file.flush()
                file.close()
                data = nil
                            
                print('File '..file_name..' is downloaded!')
                file.remove(file_name)
                file.rename('_wget',file_name)
                if wget.compile  ~= nil then
                    node.compile(file_name)
                end
                if wget.do_file  ~= nil then
                    collectgarbage()
                    dofile(file_name)
                end
            end


            print("file downloaded")
            file.remove("config_wget.lua")
            if wget.norestart == nil then
               node.restart()
            end
            wget = nil
            collectgarbage()
            
        end

    end)
    

end

wifi.setmode(wifi.STATION)
wifi.nullmodesleep(false)
wifi.sta.connect()
local timeout = 0
local wifitmr = tmr.create()    
wifitmr:alarm(1000, tmr.ALARM_AUTO, function() 
    ipAddr = wifi.sta.getip()
    if ipAddr == nil or ipAddr == "0.0.0.0" then
        print("IP unavaiable, waiting... " .. timeout)
        timeout = timeout + 1  
    else
        wifitmr:unregister()
        if file.exists("config_wget.lua") then
            dofile("config_wget.lua")
            if wget  ~= nil then
                w_get(wget.file_url,wget.file_name)
            end
            
        end 
    end
end)
