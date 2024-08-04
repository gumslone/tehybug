--content
local actn = {}
actn.stop = 0
function actn.run(act,_GET,setf,k)
    if act=="file_append" then
        if _GET.file_line ~= nil and file.open(_GET.file_name, "a+") then
            file.write(_GET.file_line.."\n")
            file.close()
        end
        _GET.file_line = nil
        _GET.file_name = nil
        actn.stop = 1
    elseif act=="restart" then
        node.restart()
        actn.stop = 1
    elseif act=="save_configuration" then
    
        if _GET.configuration_name=="wget" then
            change_systatus(3)
        elseif _GET.configuration_name=="wifi_cnf" then
            wifi.sta.config(_GET)
            wifi.sta.connect()
            _GET.pwd = nil
        end
        
        local fd  = file.open(_GET.configuration_file, "w+")
        if fd then
            fd:writeline(_GET.configuration_name..'={')
            if(_GET.configuration_name=='config' and _GET.server_url==nil) then
                fd:writeline('server_url="'..setf.trim(config_system.server_url)..'",')
            end
            _GET.configuration_name=nil
            _GET.configuration_file=nil
            for k,v in pairs(_GET) do
                fd:writeline(k..'='..string.format("%q", v)..',')
                _GET[k] = nil
            end
            fd:writeline('}')
            
            fd:close()
            fd = nil
        end
        
        actn.stop = 1
    elseif act=="remove_calibration" or act=="remove_scenario" or act=="reset_config" then
        msg='<div class="msg success">Configuration removed successfully!</div>'
        if act=="reset_config" then
            wifi.sta.clearconfig()
            fd = file.open("config.lua", "w+")
            if fd then
                fd:writeline("config={\ndata_frequency=30,\nserver_url=\""..setf.trim(config_system.server_url).."\"\n}")
                fd:close()
                fd = nil
                file.remove("config_wifi.lua")
            end
        end
        
        if act=="remove_calibration" or act=="reset_config" then
            file.remove("config_calibration.lua")
        end
        if act=="remove_scenario" or act=="reset_config" then
            file.remove("config_scenario1.lua")
            file.remove("config_scenario2.lua")
            file.remove("config_scenario3.lua")
        end
        
    elseif act=="execute" then
        msg='<div class="msg info">Command '.. string.format("%q", _GET.execute_command)..' executed successfully!</div>'
        function s_output(str)
            k:send(str)
        end
        node.output(s_output, 1)
        node.input(_GET.execute_command)
        actn.stop = 1
    elseif act=="activate_live_mode" then
        change_systatus(1)
        msg='<div class="msg success">Live mode activated successfully! Please <a href="/?action=restart" target="_blank">restart</a> the TeHyBug</div>'
        
    end
end

return actn