git submodule update -q --init --recursive

.\libs\curl\projects\generate.bat vc14

Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0-headers.7z" -outfile "wxWidgets-3.1.0-headers.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxMSW-3.1.0_vc140_x64_Dev.7z" -outfile "wxMSW-3.1.0_vc140_x64_Dev.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxMSW-3.1.0_vc140_Dev.7z" -outfile "wxMSW-3.1.0_vc140_Dev.7z"

7z.exe x ".\wxWidgets-3.1.0-headers.7z" -o".\libs\wxWidgets"
7z.exe x ".\wxMSW-3.1.0_vc140_x64_Dev.7z" -o".\libs\wxWidgets"
7z.exe x ".\wxMSW-3.1.0_vc140_Dev.7z" -o".\libs\wxWidgets"

move ".\libs\wxWidgets\lib\vc140_x64_dll\" ".\libs\wxWidgets\lib\vc_x64_dll\"
move ".\libs\wxWidgets\lib\vc140_dll\" ".\libs\wxWidgets\lib\vc_dll\"
