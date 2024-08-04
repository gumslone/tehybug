sf.data_sent=0
sf.failsafe_datasend()
for k, v in pairs(sensor) do
	

end
if file.exists("request_fail_safe.lc") then
	dofile("request_fail_safe.lc")
end
-- init mqtt client without logins, keepalive timer 120s
local m = mqtt.Client(config_system.setup_ssid, 120, config.mqtt_user, config.mqtt_password)
m:on(
	"offline",
	function(client)
		--print("offline")
		--if config.data_frequency * 1 > 5 then
			sf.deep_sleep()
		--else
		--end
	end
)

if config.mqtt_secure ~= nil then
	config.mqtt_secure = config.mqtt_secure * 1
else
	config.mqtt_secure = 0
end

config.mqtt_retain = 1


-- for TLS: m:connect("192.168.11.118", secure-port, 1)
MQTT_TOPIC_AVAILABILITY = "TeHyBug/" .. config_system.setup_ssid .. "/status"
MQTT_TOPIC_STATE = "TeHyBug/" .. config_system.setup_ssid .. "/state"
MQTT_TOPIC_AUTOCONF_SENSOR = "homeassistant/sensor/TeHyBug/sensor_%sensor%/config"

device_json =
	'{"name":"' ..
	config_system.setup_ssid .. '","identifiers":["' .. config_system.setup_ssid .. '"],"manufacturer":"TeHyBug"}'
autoconf =
	'{"device":' ..
	device_json ..
		',"state_topic":"' ..
			MQTT_TOPIC_STATE ..
				'","name":"' .. config_system.setup_ssid .. ' %sensor%", "value_template":"{{value_json.sensor.%sensor%}}",'
autoconf = autoconf .. '"unique_id":"' .. config_system.setup_ssid .. '_sensor_%sensor%"}'

state = '{"sensor":{'
for k, v in pairs(sensor) do
	state = state .. '"' .. k .. '": ' .. v .. ", "
end
state = state .. '"dummy": 0}}'

m:connect(
	config.mqtt_ip,
	config.mqtt_port,
	config.mqtt_secure,
	function(client)
		-- publish a message with data = hello, QoS = 0, retain = 0

		for k, v in pairs(sensor) do
			print(k.."="..v)
			local new_t = string.gsub(MQTT_TOPIC_AUTOCONF_SENSOR, "%%sensor%%", k, 1)
			local new_message = string.gsub(autoconf, "%%sensor%%", k)
			client:publish(
				new_t,
				new_message,
				0,
				config.mqtt_retain,
				function(client)
					print("sent")
				end
			)
		end

		client:publish(
			MQTT_TOPIC_STATE,
			sf.replace_placeholders(state),
			0,
			config.mqtt_retain,
			function(client)
				
				print("sent")
				--m:close();
				--sf.deep_sleep()
			end
		)

		tmr.create():alarm(
			1 * 1000,
			tmr.ALARM_SINGLE,
			function()
				sf.data_sent=1
				print("timer")
				m:close()
				m=nil
				client=nil
				sf.deep_sleep()
			end
		)
	end,
	function(client, reason)
		print("failed reason: " .. reason)

		--if config.data_frequency * 1 > 5 then
		sf.deep_sleep()
		--else
		--end
	end
)
-- you can call m:connect again
