@ECHO OFF
cd C:\Program Files\nginx-1.17.6
taskkill /f /IM nginx.exe
start nginx
