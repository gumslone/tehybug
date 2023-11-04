namespace Config
{

    bool shouldSaveConfig = false;
    
    void saveConfigCallback() {
        shouldSaveConfig = true;
    }
    void saveConfig() {
        // save the custom parameters to FS
        if (shouldSaveConfig) {
            DynamicJsonDocument json(4096);

            json["mqttActive"] = serveData.mqtt.active;
            json["mqttRetained"] = serveData.mqtt.retained;
            json["mqttUser"] = serveData.mqtt.user;
            json["mqttPassword"] = serveData.mqtt.password;
            json["mqttServer"] = serveData.mqtt.server;
            json["mqttMasterTopic"] = serveData.mqtt.topic;
            json["mqttMessage"] = serveData.mqtt.message;
            json["mqttPort"] = serveData.mqtt.port;
            json["mqttFrequency"] = serveData.mqtt.frequency;

            json["httpGetURL"] = serveData.get.url;
            json["httpGetActive"] = serveData.get.active;
            json["httpGetFrequency"] = serveData.get.frequency;

            json["httpPostURL"] = serveData.post.url;
            json["httpPostActive"] = serveData.post.active;
            json["httpPostFrequency"] = serveData.post.frequency;
            json["httpPostJson"] = serveData.post.message;

            json["calibrationActive"] = calibration.active;
            json["calibrationTemp"] = calibration.temp;
            json["calibrationHumi"] = calibration.humi;
            json["calibrationQfe"] = calibration.qfe;

            json["configModeActive"] = configModeActive;

            json["sleepModeActive"] = sleepModeActive;

            json["key"] = sensorData["key"];
            json["dht_sensor"] = sensor.dht;
            json["second_dht_sensor"] = sensor.dht_2;

            json["ds18b20_sensor"] = sensor.ds18b20;
            json["second_ds18b20_sensor"] = sensor.ds18b20_2;
            json["adc_sensor"] = sensor.adc;

            json["scenario1_active"] = scenarios.scenario1.active;
            json["scenario1_type"] = scenarios.scenario1.type;
            json["scenario1_url"] = scenarios.scenario1.url;
            json["scenario1_data"] = scenarios.scenario1.data;
            json["scenario1_condition"] = scenarios.scenario1.condition;
            json["scenario1_value"] = scenarios.scenario1.value;
            json["scenario1_message"] = scenarios.scenario1.message;

            json["scenario2_active"] = scenarios.scenario2.active;
            json["scenario2_type"] = scenarios.scenario2.type;
            json["scenario2_url"] = scenarios.scenario2.url;
            json["scenario2_data"] = scenarios.scenario2.data;
            json["scenario2_condition"] = scenarios.scenario2.condition;
            json["scenario2_value"] = scenarios.scenario2.value;
            json["scenario2_message"] = scenarios.scenario2.message;

            json["scenario3_active"] = scenarios.scenario3.active;
            json["scenario3_type"] = scenarios.scenario3.type;
            json["scenario3_url"] = scenarios.scenario3.url;
            json["scenario3_data"] = scenarios.scenario3.data;
            json["scenario3_condition"] = scenarios.scenario3.condition;
            json["scenario3_value"] = scenarios.scenario3.value;
            json["scenario3_message"] = scenarios.scenario3.message;

            File configFile = SPIFFS.open("/config.json", "w");
            serializeJson(json, configFile);
            configFile.close();
            Log("SaveConfig", "Saved");
            // end save
        }
    }

    void setConfigParameters(JsonObject &json) {
        D_println("Config:");
        if (DEBUG)
        {
            for (JsonPair kv : json) {
            D_print(kv.key().c_str());
            D_print(" = ");
            D_println(kv.value().as<String>());
            }
        }
        D_println();
        if (json.containsKey("mqttActive")) {
            serveData.mqtt.active = json["mqttActive"].as<bool>();
        }
        if (json.containsKey("mqttRetained")) {
            serveData.mqtt.retained = json["mqttRetained"].as<bool>();
        }
        if (json.containsKey("mqttUser")) {
            serveData.mqtt.user = json["mqttUser"].as<String>();
        }
        if (json.containsKey("mqttPassword")) {
            serveData.mqtt.password = json["mqttPassword"].as<String>();
        }
        if (json.containsKey("mqttServer")) {
            serveData.mqtt.server = json["mqttServer"].as<String>();
        }
        if (json.containsKey("mqttMasterTopic")) {
            serveData.mqtt.topic = json["mqttMasterTopic"].as<String>();
        }
        if (json.containsKey("mqttMessage")) {
            serveData.mqtt.message = json["mqttMessage"].as<String>();
        }
        if (json.containsKey("mqttPort")) {
            serveData.mqtt.port = json["mqttPort"].as<int>();
        }
        if (json.containsKey("mqttFrequency")) {
            serveData.mqtt.frequency = json["mqttFrequency"].as<int>();
            validateDataFrequency(serveData.mqtt.frequency);
        }

        // http
        if (json.containsKey("httpGetURL")) {
            serveData.get.url = json["httpGetURL"].as<String>();
        }
        if (json.containsKey("httpGetActive")) {
            serveData.get.active = json["httpGetActive"].as<bool>();
        }
        if (json.containsKey("httpGetFrequency")) {
            serveData.get.frequency = json["httpGetFrequency"].as<int>();
            validateDataFrequency(serveData.get.frequency);
        }

        if (json.containsKey("httpPostURL")) {
            serveData.post.url = json["httpPostURL"].as<String>();
        }
        if (json.containsKey("httpPostActive")) {
            serveData.post.active = json["httpPostActive"].as<bool>();
        }
        if (json.containsKey("httpPostFrequency")) {
            serveData.post.frequency = json["httpPostFrequency"].as<int>();
            validateDataFrequency(serveData.post.frequency);
        }
        if (json.containsKey("httpPostJson")) {
            serveData.post.message = json["httpPostJson"].as<String>();
        }

        if (json.containsKey("configModeActive")) {
            configModeActive = json["configModeActive"].as<bool>();
        }
        if (json.containsKey("calibrationActive")) {
            calibration.active = json["calibrationActive"].as<bool>();
        }
        if (json.containsKey("calibrationTemp")) {
            calibration.temp = json["calibrationTemp"].as<float>();
        }
        if (json.containsKey("calibrationHumi")) {
            calibration.humi = json["calibrationHumi"].as<float>();
        }
        if (json.containsKey("calibrationQfe")) {
            calibration.qfe = json["calibrationQfe"].as<float>();
        }
        if (json.containsKey("sleepModeActive")) {
            sleepModeActive = json["sleepModeActive"].as<bool>();
        }
        if (json.containsKey("dht_sensor")) {
            sensor.dht = json["dht_sensor"].as<bool>();
        }
        if (json.containsKey("second_dht_sensor")) {
            sensor.dht_2 = json["second_dht_sensor"].as<bool>();
        }
        if (json.containsKey("ds18b20_sensor")) {
            sensor.ds18b20 = json["ds18b20_sensor"].as<bool>();
        }
        if (json.containsKey("second_ds18b20_sensor")) {
            sensor.ds18b20_2 = json["second_ds18b20_sensor"].as<bool>();
        }
        if (json.containsKey("adc_sensor")) {
            sensor.adc = json["adc_sensor"].as<bool>();
        }

        // scenarios
        if (json.containsKey("scenario1_active")) {
            scenarios.scenario1.active = json["scenario1_active"].as<bool>();
        }
        if (json.containsKey("scenario1_type")) {
            scenarios.scenario1.type = json["scenario1_type"].as<String>();
        }
        if (json.containsKey("scenario1_url")) {
            scenarios.scenario1.url = json["scenario1_url"].as<String>();
        }
        if (json.containsKey("scenario1_data")) {
            scenarios.scenario1.data = json["scenario1_data"].as<String>();
        }
        if (json.containsKey("scenario1_condition")) {
            scenarios.scenario1.condition = json["scenario1_condition"].as<String>();
        }
        if (json.containsKey("scenario1_value")) {
            scenarios.scenario1.value = json["scenario1_value"].as<float>();
        }
        if (json.containsKey("scenario1_message")) {
            scenarios.scenario1.message = json["scenario1_message"].as<String>();
        }

        if (json.containsKey("scenario2_active")) {
            scenarios.scenario2.active = json["scenario2_active"].as<bool>();
        }
        if (json.containsKey("scenario2_type")) {
            scenarios.scenario2.type = json["scenario2_type"].as<String>();
        }
        if (json.containsKey("scenario2_url")) {
            scenarios.scenario2.url = json["scenario2_url"].as<String>();
        }
        if (json.containsKey("scenario2_data")) {
            scenarios.scenario2.data = json["scenario2_data"].as<String>();
        }
        if (json.containsKey("scenario2_condition")) {
            scenarios.scenario2.condition = json["scenario2_condition"].as<String>();
        }
        if (json.containsKey("scenario2_value")) {
            scenarios.scenario2.value = json["scenario2_value"].as<float>();
        }
        if (json.containsKey("scenario2_message")) {
            scenarios.scenario2.message = json["scenario2_message"].as<String>();
        }

        if (json.containsKey("scenario3_active")) {
            scenarios.scenario3.active = json["scenario3_active"].as<bool>();
        }
        if (json.containsKey("scenario3_type")) {
            scenarios.scenario3.type = json["scenario3_type"].as<String>();
        }
        if (json.containsKey("scenario3_url")) {
            scenarios.scenario3.url = json["scenario3_url"].as<String>();
        }
        if (json.containsKey("scenario3_data")) {
            scenarios.scenario3.data = json["scenario3_data"].as<String>();
        }
        if (json.containsKey("scenario3_condition")) {
            scenarios.scenario3.condition = json["scenario3_condition"].as<String>();
        }
        if (json.containsKey("scenario3_value")) {
            scenarios.scenario3.value = json["scenario3_value"].as<float>();
        }
        if (json.containsKey("scenario3_message")) {
            scenarios.scenario3.message = json["scenario3_message"].as<String>();
        }
    }

    void loadConfig() {
        if (SPIFFS.exists("/config.json")) {
            // file exists, reading and loading
            File configFile = SPIFFS.open("/config.json", "r");

            if (configFile) {
            Log("LoadConfig", "opened config file");

            DynamicJsonDocument json(4096);
            auto error = deserializeJson(json, configFile);

            if (!error) {
                JsonObject documentRoot = json.as<JsonObject>();
                setConfigParameters(documentRoot);

                Log("LoadConfig", "Loaded");
            } else {
                switch (error.code()) {
                case DeserializationError::Ok:
                    D_println(F("Deserialization succeeded"));
                    break;
                case DeserializationError::InvalidInput:
                    D_println(F("Invalid input!"));
                    break;
                case DeserializationError::NoMemory:
                    D_println(F("Not enough memory"));
                    break;
                default:
                    D_println(F("Deserialization failed"));
                    break;
                }
            }
            }
        } else {
            Log("LoadConfig", "No configfile found, create a new file");
            saveConfigCallback();
            saveConfig();
        }
    }

    void setConfig(JsonObject &json) {
        setConfigParameters(json);
        saveConfigCallback();
        saveConfig();

        if (json.containsKey("reboot")) {
            if (json["reboot"]) {
            delay(1000);
            ESP.restart();
            }
        }
    }

    String getConfig() {
        File configFile = SPIFFS.open("/config.json", "r");

        if (configFile) {
            const size_t size = configFile.size();
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);
            DynamicJsonDocument root(4096);

            if (DeserializationError::Ok == deserializeJson(root, buf.get())) {
            }
            String json;
            serializeJson(root, json);
            return json;
        }
    }

}