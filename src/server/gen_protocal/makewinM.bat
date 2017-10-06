@echo off
echo "starting..."
svn up
del *flymake*
erl -noshell -pz ../smart_exceptions/ebin -make
cd ebin
erl -noshell -s gen_protocal start -s gen_enum_def start -s gen_protocal_cpp start -s init stop

copy  protocal.erl ..\..\src
copy  packet_def.hrl ..\..\include
copy  enum_def.hrl ..\..\include


cd ..\

echo "finish"
pause