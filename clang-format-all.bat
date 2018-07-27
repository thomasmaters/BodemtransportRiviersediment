for /r %%i in (*.cpp) do start clang-format -i -style=file "%%~dpnxi"
for /r %%i in (*.hpp) do start clang-format -i -style=file "%%~dpnxi"
for /r %%i in (*.inc) do start clang-format -i -style=file "%%~dpnxi"