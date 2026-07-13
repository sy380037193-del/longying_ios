@echo off
set DIR=%~dp0
set OUTPUT_DIR=%DIR%
set MAKE_LUABINDING="%QUICK_COCOS2DX_ROOT%\bin\compile_luabinding.bat"
call %MAKE_LUABINDING% -E CCOBJECTS=ANodeGrid,Binary,AStar,ANode -d %OUTPUT_DIR% CCAStar_luabinding.tolua
