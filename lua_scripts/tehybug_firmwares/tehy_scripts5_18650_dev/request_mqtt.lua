sf.data_sent=0
sf.failsafe_datasend()

if file.exists("request_fail_safe.lc") then
	dofile("request_fail_safe.lc")
end
-- init mqtt client without logins, keepalive timer 120s
local m = mqtt.Client(config_system.setup_ssid, 120, config.mqtt_user, config.mqtt_password)
m:on("offline", function(client)
	--print ("offline")
	sf.deep_sleep()
end)

if config.mqtt_secure ~= nil then
	config.mqtt_secure = config.mqtt_secure*1
else
	config.mqtt_secure = 0
end

if config.mqtt_retain ~= nil then
	config.mqtt_retain = config.mqtt_retain*1
else
	config.mqtt_retain = 0
end

if config.topic == "" then
	config.topic = "tehybug"
end
if config.mqtt_port == "" then
	config.mqtt_port = "1883"
end
-- for TLS: m:connect("192.168.11.118", secure-port, 1)
m:connect(config.mqtt_ip, config.mqtt_port * 1, config.mqtt_secure, function(client)
	-- publish a message with data = hello, QoS = 0, retain = 0
	client:publish(config.mqtt_topic, sf.replace_placeholders(config.mqtt_message), 0, config.mqtt_retain, function(client) 
			sf.data_sent = 1
			sf.deep_sleep()
			m:close()
	 end)
end,
function(client, reason)
	print("failed reason: " .. reason)
			sf.deep_sleep()
end)
-- you can call m:connect again