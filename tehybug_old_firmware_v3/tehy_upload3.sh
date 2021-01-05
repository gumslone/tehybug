clear
cd ~
cd ./tehy_scripts3
rm system_config.txt
output2=$(nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 file print system_config.lua 2>&1)
echo "$output2" > "system_config.txt"
echo "$output2"
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 file format
#rm system_config.lua
rm node_info.txt
output=$(nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 node info 2>&1 | tail -2)
echo "$output" > "node_info.txt"
echo "$output"
echo "majorV  minorV  devV    chipid  flashid  flashsize  flashmode  flashspeed"
sleep 2
python system_config_generator.py
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 file format
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 node restart
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 upload *.lua --verify=raw
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 upload *.html --verify=raw
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 upload *.gz --verify=raw
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 upload *.MONO --verify=raw
#nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 file remove init.lua
nodemcu-uploader --start_baud 115200 --baud 115200 --port /dev/cu.wchusbserialfa130 file do install.lua


