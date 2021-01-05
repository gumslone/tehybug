system_config = {
    server_url = "http://tehybug.com/track/?t=%temp%&h=%humi%&p=%qfe%&a=%air%&chipid=%chipid%&bug_key=%key%&fw=%fw%";
    setup_ssid = "TeHyBug_".. node.chipid();
    order_id = "xxx";
    bug_key = "xxxxxx-2522-44e8-9d5b-f6077d28e8e0";
    chip_id = node.chipid();
    sensor_type = "bme680";
    firmware_version = 30;
}