clear
#port=$(ls /dev/*USB*)
port=$(ls /dev/tty.usbserial*)
echo "Port: $port"
cd ./tehy_scripts5_18650_min_dev/
rm config_system.txt
system_config=$(nodemcu-uploader --start_baud 115200 --baud 115200 --port $port file print config_system.lua 2>&1)
echo "$system_config" > "config_system.txt"
echo "$system_config"
rm node_info.txt
info=$(nodemcu-uploader --start_baud 115200 --baud 115200 --port $port node info 2>&1 | tail -2)
echo "$info" > "node_info.txt"
echo "$info"
echo "majorV  minorV  devV    chipid  flashid  flashsize  flashmode  flashspeed"
sleep 2

nodemcu-uploader --start_baud 115200 --baud 115200 --port $port file format
sleep 2
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port node restart
sleep 2
echo "Uploading setup files"
cd ./setup/
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.lua --verify=raw --compile
echo "Uploading html files"
cd ../html/
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.html --verify=raw
echo "Uploading compressed html files"
cd ../html_compress/
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.gz --verify=raw
echo "Uploading other compressed files"
cd ../
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.gz --verify=raw


#nodemcu-uploader --start_baud 115200 --baud 115200 --port $port file remove init.lua

echo "Uploading and compiling lua files"
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.lua --verify=raw --compile
cd ./do_not_compile/
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port upload *.lua --verify=raw
nodemcu-uploader --start_baud 115200 --baud 115200 --port $port file do install.lua
cd ../
sleep 3
filelist=$(nodemcu-uploader --start_baud 115200 --baud 115200 --port $port file list 2>&1)
#echo "$filelist"
echo "$filelist" > "filelist.txt"

python filelist_check.py
