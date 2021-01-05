find -E . -regex "".*\(css\|js\)$"" -exec bash -c 'echo Compressing "{}" && gzip -c --best "{}" > "{}.gz"' \;
