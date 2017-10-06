
del *flymake*

erl -noshell -pz ../../smart_exceptions/ebin -make

cd ebin
erl -noshell -pz ../../../gen_protocal/ebin -s gen_google_proto start -s gen_protocal_erl start -s init stop

protoc.bat

pause



