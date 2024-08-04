system_config = {
    device_type = "ROLLO";
    device_hostname = "ROLLO-".. node.chipid();
    wifi_ssid = "MyWifi";
    wifi_pass = "mypw";
    chip_id = node.chipid();
    broker = "192.168.0.111";
}