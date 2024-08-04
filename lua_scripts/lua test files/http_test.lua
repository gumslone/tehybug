	http.get('http://tehybug.com/test.php?chipid='..node.chipid(), nil, function(code, data)
		if (code < 0) then
			print("HTTP request failed")
		else
			print(code, data)
			
		end

	end)