svn up
rm -f *flymake*
erl -pz ..\smart_exceptions\ebin -make
erl -noshell -s make_xml_source start -s init stop