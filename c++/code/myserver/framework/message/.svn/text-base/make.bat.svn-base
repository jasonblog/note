del *.pb.*
cd proto\
for /r %%i in (*.proto) do (
	..\protoc.exe %%~ni%%~xi --cpp_out=..\
)
cd ..