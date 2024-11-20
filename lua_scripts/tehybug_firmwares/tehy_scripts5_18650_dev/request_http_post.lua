sf.data_sent=0
sf.failsafe_datasend()

if file.exists("request_fail_safe.lc") then
	dofile("request_fail_safe.lc")
end
http.post(config.server_url,
	'Content-Type: application/json\r\n',
	sf.replace_placeholders(config.post_content),
	function(code, data)
		if (code < 0) then
			print("HTTP request failed")
		else
			print(code, data)
			sf.data_sent = 1
			if data ~= nil then
				sensor.data = string.sub(data, 1, 24)
			end
		end
		
		code = nil
		data = nil
		sf.deep_sleep()
end)
