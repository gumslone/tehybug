-- print the first line of 'init.lua'
if file.open("config_system.lua", "r") then
  print(file.read('\n'))
  file.close()
end