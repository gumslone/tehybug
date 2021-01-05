print("generating file list")
file.remove("file_list.html")
file.open("file_list.html", "w")
file.writeline('<html><head><title>File List</title></head><body><h3>File List</h3><div>')
for k,v in pairs(file.list()) do
	file.writeline('<a href="'..k..'?action=view_file">'..k..'</a>, size: '..v..'<br>')
end
local remaining, used, total=file.fsinfo()
file.writeline('</div><div style="margin-top: 15px;">File system info:<br>Total : '..total..' (k)Bytes<br>Used : '..used..' (k)Bytes<br>Remain: '..remaining..' (k)Bytes</div>')
file.writeline('<div style="margin-top: 15px;"><a href="/admin.html">&lt; Admin Menu</a></div><div style="margin-top: 15px;"><a href="/">&lt; Main Menu</a></div><link rel="stylesheet" href="/style.css.gz"></body></html>')
file.close()