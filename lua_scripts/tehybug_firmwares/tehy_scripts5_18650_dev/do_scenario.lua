        local send = 0

		if scenario.condition=='eq' and sensor[scenario.parameter] ~= nil and sensor[scenario.parameter] * 1 == scenario.value * 1 then
			send = 1
		elseif scenario.condition=='gt' and sensor[scenario.parameter] ~= nil and sensor[scenario.parameter] * 1 > scenario.value * 1 then
			send = 1
		elseif scenario.condition=='lt' and sensor[scenario.parameter] ~= nil and sensor[scenario.parameter] * 1 < scenario.value * 1 then
			send = 1
		end

		if send == 1 then
			http.get(sf.replace_placeholders(scenario.url), nil, function(code, data) 
				code = nil
				data = nil
			end)
		end
		for k,v in pairs(scenario) do scenario[k]=nil end
		scenario = nil
		send=nil