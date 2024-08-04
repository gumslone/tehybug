math.randomseed( node.chipid() )
 local random = math.random
	local function uuid()
		local template ='xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'
		return string.gsub(template, '[xy]', function (c)
			local v = (c == 'x') and random(0, 0xf) or random(8, 0xb)
			return string.format('%x', v)
		end)
	end
	local fd = file.open("config_system.lua", "w+")
	if fd then
		fd:writeline('config_system={bug_key="'..uuid()..'", chip_id="'..node.chipid()..'", setup_ssid="TeHyBug-'..node.chipid()..'", server_url="http://tehybug.com/track/?t=%temp%&h=%humi%&bug_key=%key%", } ')
		fd:close()
		fd = nil
	end