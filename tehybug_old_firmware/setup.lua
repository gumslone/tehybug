
gpio.mode(10, gpio.OUTPUT) -- tx pin
gpio.write(10, gpio.LOW)



local unescape = function (s)
   s = string.gsub(s, "+", " ")
   s = string.gsub(s, "%%(%x%x)", function (h)
         return string.char(tonumber(h, 16))
      end)
   return s
end

function setup_server(aps)
   print("Setting up Wifi AP")
   wifi.setmode(wifi.SOFTAP)
   cfg = {}
   cfg.ssid = system_config.setup_ssid
   
   wifi.ap.config(cfg)      
   wifi.ap.setip({ip="192.168.1.1",netmask="255.255.255.0",gateway="192.168.1.1"})
   print("Setting up webserver")
   srv = nil
   srv=net.createServer(net.TCP)
   srv:listen(80,function(conn)
       conn:on("receive", function(client,request)
           
           local buf = ""
           local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
           if(method == nil)then
               _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP")
           end
           local _GET = {}
           if (vars ~= nil)then
               for k, v in string.gmatch(vars, "(%w+)=([^%&]+)&*") do
                   _GET[k] = unescape(v)
               end
           end
           
           
           if(_GET.firmware_update == 1) then
	            file.remove("system_status.lua")
                file.open("system_status.lua", "w")
                file.writeline('system_status = { ')
                file.writeline('system_status = -1;')
                file.writeline('} ')
                file.close()
           elseif (_GET.ssid ~= nil and _GET.pass ~= nil and _GET.freq ~= nil) then
                client:send("Saving data..")
                client:on("sent",function(conn)
                    print("Closing connection")
                    client:close()
                end)
                collectgarbage()
                --file.remove("config.lc")
                file.remove("config.lua")
                file.open("config.lua", "w")
                file.writeline('config = { ')
                file.writeline('wifi_router_ssid = "' .. _GET.ssid .. '";')
                file.writeline('wifi_router_password = "' .. _GET.pass .. '";')
                file.writeline('data_frequency = "' .. _GET.freq .. '";')
                if(_GET.url ~= nil) then
                    file.writeline('server_url = "' .. _GET.url .. '";')
                else
                    file.writeline('server_url = "' .. system_config.server_url .. '";')
                end
                file.writeline('} ')
                file.close()
                file.remove("system_status.lua")
                file.open("system_status.lua", "w")
                file.writeline('system_status = { ')
                file.writeline('system_status = 1;')
                file.writeline('} ')
                file.close()
                --node.compile("config.lua")
                tmr.delay(5000000)
                node.restart()
            elseif(_GET.mode=='firmware_update') then
                buf = '<html><head><meta http-equiv="Cache-Control" content="no-cache"/><title>'..system_config.setup_ssid..' Firmware update</title></head>'
                
                buf = buf .. '<body><h3>TeHy-Bug '.. system_config.setup_ssid ..' Firmware Update</h3>'
                buf = buf .. '<form method="get" action="http://' .. wifi.ap.getip() ..'"><table>'
                buf = buf .. '<tr><td class="tt"></td><td><input type="hidden" name="firmware_update" value="1" /></td></tr>'
                
                buf = buf .. '<tr><td colspan="2"><button type="submit" class="tt">Update Firmware</button></td></tr>'
                buf = buf .. '</table></form><div><button onclick="window.history.back()">Back</button></div></body></html>'
                client:send(buf)
                client:on("sent",function(conn)
                    print("Closing connection")
                    client:close()
                end)
                --client:close()
                collectgarbage()
                print("Setting HTML")
            elseif(_GET.mode=='default') then
                buf = '<html><head><meta http-equiv="Cache-Control" content="no-cache"/><title>'..system_config.setup_ssid..' Config</title></head>'
                buf = buf .. '<style>.tt{position:relative;display:inline-block;}.tt .ttt{visibility:hidden;width:120px;background-color:#000;color:#fff;border-radius:6px;padding:3px;font-size:small;position:absolute;z-index:1}.tt:hover .ttt{visibility:visible}</style>'
                buf = buf .. '<body><h3>TeHyBug key: '.. system_config.bug_key ..'</h3>'
                buf = buf .. '<form method="get" action="http://' .. wifi.ap.getip() ..'"><table>'
                buf = buf .. '<tr><td class="tt">Wifi SSID:</td><td><input type="text" name="ssid" value="'.. config.wifi_router_ssid ..'" /></td></tr>'
                buf = buf .. '<tr><td class="tt">Wifi password:</td><td><input type="password" name="pass" value="'.. config.wifi_router_password ..'" /></td></tr>'
                buf = buf .. '<tr><td class="tt">Data update frequency (in minutes):<span class="ttt">How often to send data to the TeHy server (max 60).</span></td><td><input type="text" name="freq" value="'.. config.data_frequency ..'" pattern="[0-9]{1,2}" /></td></tr>'
                buf = buf .. '<tr><td colspan="2"><button type="submit" class="tt">Save settings<span class="ttt">TeHyBug will restart and should begin with sending data.</span></button></td></tr>'
                buf = buf .. '</table></form><div><button onclick="window.history.back()">Back</button></div></body></html>'
                client:send(buf)
                client:on("sent",function(conn)
                    print("Closing connection")
                    client:close()
                end)
                --client:close()
                collectgarbage()
                print("Setting HTML")
            elseif(_GET.mode=='custom') then
                buf = '<html><head><meta http-equiv="Cache-Control" content="no-cache"/><title>'..system_config.setup_ssid..' Config</title></head>'
                buf = buf .. '<style>.tt{position:relative;display:inline-block;}.tt .ttt{visibility:hidden;width:120px;background-color:#000;color:#fff;border-radius:6px;padding:3px;font-size:small;position:absolute;z-index:1}.tt:hover .ttt{visibility:visible}</style>'
                buf = buf .. '<body><h3>TeHyBug key: '.. system_config.bug_key ..'</h3>'
                buf = buf .. '<form method="get" action="http://' .. wifi.ap.getip() ..'"><table>'
                buf = buf .. '<tr><td class="tt">Wifi SSID:</td><td><input type="text" name="ssid" value="'.. config.wifi_router_ssid ..'" /></td></tr>'
                buf = buf .. '<tr><td class="tt">Wifi password:</td><td><input type="password" name="pass" value="'.. config.wifi_router_password ..'" /></td></tr>'
                buf = buf .. '<tr><td class="tt">Data update frequency (in minutes):<span class="ttt">How often to send data to the TeHy server</span></td><td><input type="text" name="freq" value="'.. config.data_frequency ..'" pattern="[0-9]{1,2}" /></td></tr>'
                buf = buf .. '<tr><td class="tt">Server url with placeholders:<span class="ttt">%temp% for temperature, %humi% for humidity. https is not supported</span></td><td><input type="url" name="url" value="'.. config.server_url ..'" pattern="https?://.+" /></td></tr>'
                buf = buf .. '<tr><td colspan="2"><button type="submit" class="tt">Save settings<span class="ttt">TeHy-Bug will restart and should begin with sending data.</span></button></td></tr>'
                buf = buf .. '</table></form><div><button onclick="window.history.back()">Back</button></div></body></html>'
                client:send(buf)
                client:on("sent",function(conn)
                    print("Closing connection")
                    client:close()
                end)
                --client:close()
                collectgarbage()
                print("Setting HTML")
           else
                buf = '<html><head><meta http-equiv="Cache-Control" content="no-cache"/><title>'..system_config.setup_ssid..' Config</title></head>'
                buf = buf .. '<style>.tt{position:relative;display:inline-block;}.tt .ttt{visibility:hidden;width:120px;background-color:#000;color:#fff;border-radius:6px;padding:3px;font-size:small;position:absolute;z-index:1}.tt:hover .ttt{visibility:visible}</style>'
                buf = buf .. '<body><h3>TeHyBug key: '.. system_config.bug_key ..'</h3><br/><p>Please select a mode below</p>'
                buf = buf .. '<a href="http://' .. wifi.ap.getip() ..'/?mode=default" class="tt">Default mode<span class="ttt">Use TeHyBug server for data tracking</span></a><br/><br/> '
                buf = buf .. '<a href="http://' .. wifi.ap.getip() ..'/?mode=custom" class="tt">Custom mode<span class="ttt">Use your own server for data tracking</span></a>'
                buf = buf .. '</body></html>'
                client:send(buf)
                client:on("sent",function(client)
                    print("Closing connection")
                    client:close()
                end)
                --client:close()
                collectgarbage()
                print("Setting HTML")


                
           
           end
           
       end)
   end)
   print("Please connect to: " .. system_config.setup_ssid .. " - IP: " .. wifi.ap.getip())
end

function trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end


setup_server('')
