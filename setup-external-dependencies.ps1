git submodule update -q --init --recursive

.\libs\curl\projects\generate.bat vc14

Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.6/wxWidgets-3.1.6-headers.7z" -outfile "wxWidgets-3.1.6-headers.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.6/wxMSW-3.1.6_vc14x_x64_Dev.7z" -outfile "wxMSW-3.1.6_vc14x_x64_Dev.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.6/wxMSW-3.1.6_vc14x_x64_ReleaseDLL.7z" -outfile "wxMSW-3.1.6_vc14x_x64_ReleaseDLL.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.6/wxMSW-3.1.6_vc14x_Dev.7z" -outfile "wxMSW-3.1.6_vc14x_Dev.7z"
Invoke-WebRequest "https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.6/wxMSW-3.1.6_vc14x_ReleaseDLL.7z" -outfile "wxMSW-3.1.6_vc14x_ReleaseDLL.7z"

7z.exe x -y ".\wxWidgets-3.1.6-headers.7z" -o".\libs\wxWidgets"
7z.exe x -y ".\wxMSW-3.1.6_vc14x_x64_Dev.7z" -o".\libs\wxWidgets"
7z.exe x -y ".\wxMSW-3.1.6_vc14x_x64_ReleaseDLL.7z" -o".\libs\wxWidgets"
7z.exe x -y ".\wxMSW-3.1.6_vc14x_Dev.7z" -o".\libs\wxWidgets"
7z.exe x -y ".\wxMSW-3.1.6_vc14x_ReleaseDLL.7z" -o".\libs\wxWidgets"

move ".\libs\wxWidgets\lib\vc14x_x64_dll\" ".\libs\wxWidgets\lib\vc_x64_dll\"
move ".\libs\wxWidgets\lib\vc14x_dll\" ".\libs\wxWidgets\lib\vc_dll\"
