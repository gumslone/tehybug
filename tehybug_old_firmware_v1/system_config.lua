system_config = {
    server_url = "http://tehybug.com/track/?t=%temp%&h=%humi%&p=%qfe%&chipid=%chipid%&sensor=%sensor%&fw=%fw%";
    setup_ssid = "TeHyBug_".. node.chipid() .."";
    order_id = "105677";
    bug_key = "xxxxxx-xxxx-xxxx-xxxx-4xx6cxx8xx63";
    data_frequency = 15;
    chip_id = node.chipid();
    sensor_type = "bme280";
    display_type = "none";
    display_temperature = "C";
    firmware_version = 3;
}