system_config = {
    device_type = "Neopixel";
    device_hostname = "Neopixel-".. node.chipid();
    wifi_ssid = "wifi";
    wifi_pass = "pw";
    chip_id = node.chipid();
    broker = "192.168.0.1";
    leds = 256;
    brightness = 30;
}