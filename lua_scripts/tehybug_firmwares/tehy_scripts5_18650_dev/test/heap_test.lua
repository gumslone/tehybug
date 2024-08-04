
node.egc.setmode(node.egc.ON_ALLOC_FAILURE)

function unset_table(arr)
    for k,v in pairs(arr) do 
        print(k..' - ')
    end
end


print("HEAP before unset: "..node.heap())
--bme280 = nil
--bme680 = nil

config_system = {
    server_url = "http://tehybug.com/track/?t=%temp%&h=%humi%&p=%qfe%&a=%air%&chipid=%chipid%&bug_key=%key%&fw=%fw%";
    setup_ssid = "TeHyBug-".. node.chipid();
    order_id = "123";
    bug_key = "8c555b43-d8b6-442b-90ff-f595e4e34ded";
    chip_id = node.chipid();
    sensor_type = "bme680";
    firmware_version = 20;
}


print("HEAP data loaded: "..node.heap())

if sf ~= nil then 
 for k,v in pairs(sf) do 
  table.remove(sf,0) 
 end 
 sf = nil 
end
print(type(sf))

collectgarbage()
print("HEAP after unset: "..node.heap())
