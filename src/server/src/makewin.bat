cd ..\gen_protocal
call makewin.bat

echo "make src"

cd ..\src
del *flymake*
erl -noshell -pz ..\smart_exceptions\ebin -pz ..\ebin -make

cd npc
del *flymake*
erl -noshell -pz ..\smart_exceptions\ebin -pz ..\ebin -make

cd ..\task
del *flymake*
erl -noshell -pz ..\smart_exceptions\ebin -pz ..\ebin -make

pause