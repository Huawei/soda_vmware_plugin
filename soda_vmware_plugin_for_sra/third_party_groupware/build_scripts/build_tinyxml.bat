@echo off
SET ProjectPath=%cd%\..\OpenSDS_Storage_Plugins\tinyxml\source

::copy
xCOPY "%ProjectPath%\tinyxml.h" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y
xCOPY "%ProjectPath%\tinystr.h" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y

xCOPY "%ProjectPath%\tinyxmlparser.cpp" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y
xCOPY "%ProjectPath%\tinyxmlerror.cpp" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y
xCOPY "%ProjectPath%\tinystr.cpp" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y
xCOPY "%ProjectPath%\tinyxml.cpp" "%ProjectPath%\..\..\..\..\source\common\xmlserial\" /y

:Exit