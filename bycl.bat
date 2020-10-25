mkdir -p bin
cl /O2 /GL /Wall /EHsc /std:c++14 /Febin\mscl src\run.cpp
bin\mscl
