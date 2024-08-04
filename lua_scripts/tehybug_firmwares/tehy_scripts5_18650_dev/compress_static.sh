find -E . -regex "".*\(css\|js\)$"" -exec bash -c 'echo Compressing "{}" && gzip -c --best "{}" > "{}.gz"' \;
cd html_compress 
find -E . -regex "".*\(html\)$"" -exec bash -c 'echo Compressing "{}" && gzip -c --best "{}" > "{}.gz"' \;
