g++ -w -DUNICODE -DWIN32 -s -static -static-libgcc -static-libstdc++ *.cpp  -o console.exe -I./../../../lizzz_framework -lws2_32 -lcrypt32

call "console.exe"

pause