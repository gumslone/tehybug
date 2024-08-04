if(config.request_type == "web" or config.request_type == "offline" or config.request_type == "uart") then
	if config.request_type == "uart" then
		print(sf.replace_placeholders(config.uart_string))
	end
else
	sf.pcall('request_'..config.request_type..'.lc')
end
