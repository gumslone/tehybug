l = file.list();
for k,v in pairs(l) do
  --print("name:"..k..", size:"..v)
  if string.find(k, "%.lua") then
    collectgarbage("collect")
    print("compiling "..k.."...\n")
    node.compile(k)
    print("removing "..k.."...")
    file.remove(k)
  end
end