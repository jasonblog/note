@echo off
rem The code is portable, you can choose your favouriate compiler to compile the code.
rem Here I compile it use cl.exe

echo "Invoking Visual C++ compilier..."
cl /nologo /GX test_trace.cpp
