
-- dns-liar1.lua
--Thanks to Thomas Shaddack for optimizations - 20150707 ARe
dns_ip=wifi.ap.getip()
local i1,i2,i3,i4=dns_ip:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")
x00=string.char(0)
x01=string.char(1)
-- constants required for a proper DNS reply
dns_str1=string.char(128)..x00..x00..x01..x00..x01..x00..x00..x00..x00
dns_str2=x00..x01..x00..x01..string.char(192)..string.char(12)..x00..x01..x00..x01..x00..x00..string.char(3)..x00..x00..string.char(4)
dns_strIP=string.char(i1)..string.char(i2)..string.char(i3)..string.char(i4)
--% code in this section was modified to amend the deprecated APIs
--% see for details https://nodemcu.readthedocs.io/en/master/modules/net/#netudpsocket-module 
udpSocket=net.createUDPSocket()
udpSocket:on("receive", function(s, data, port, ip)
    --print(string.format("received '%s' from %s:%d", data, ip, port)) -- DEBUG
    --s:send(port, ip, "echo: " .. data) -- from the docs example replaced below
    decodedns(data)
    s:send(port, ip, dns_tr..dns_str1..dns_q..dns_str2..dns_strIP)    
    collectgarbage("collect")    
end)
-------------------------------------
--old callback code - deprecated
--svr:on("receive",function(svr,dns_pl)
--  print(dns_pl)
--  print(dns_tr..dns_str1..dns_q..dns_str2..dns_strIP)
--  svr:send(dns_tr..dns_str1..dns_q..dns_str2..dns_strIP)
--  collectgarbage("collect")
--end)
-------------------------------------
udpSocket:listen(53) -- DNS requests are sent to the gateway IP : 53
--%
function decodedns(dns_pl)
  local a=string.len(dns_pl)
  dns_tr = string.sub(dns_pl, 1, 2) -- get the first 2B of the request -> request ID
  local bte=""
  dns_q=""
  local i=13
  local bte2=""
  while bte2 ~= "0" do
    bte = string.byte(dns_pl,i)
    bte2 = string.format("%x", bte )
    dns_q = dns_q .. string.char(bte)
    i=i+1
  end
  --print(string.format("DNS request: '%s' for %s",dns_tr,dns_q)) -- DEBUG
end
print("DNS Server is now listening. Free Heap:", node.heap())