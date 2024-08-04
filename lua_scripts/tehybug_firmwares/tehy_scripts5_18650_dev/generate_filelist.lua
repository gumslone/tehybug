print("generating file list")
file.remove("file_list.html")
file.open("file_list.html", "w")
file.writeline('<html><head><title>File List</title></head><body><h3>File List</h3><div>')
local count_files = 0
for k,v in pairs(file.list()) do
	file.writeline('<a href="'..k..'?action=view_file">'..k..'</a>, size: '..v..'<br>')
	count_files = count_files + 1;
end
local remaining, used, total=file.fsinfo()
file.writeline('</div><p class="m15">File system info:<br>'..count_files..' Files<br>Total : '..total..' (k)Bytes<br>Used : '..used..' (k)Bytes<br>Remain: '..remaining..' (k)Bytes</p>')
file.writeline('<p class="m15"><a href="/admin.html">&lt; Admin Menu</a></p><p class="m15"><a href="/">&lt; Main Menu</a></p><link rel="stylesheet" href="/style.css.gz"></body></html>')
file.close()