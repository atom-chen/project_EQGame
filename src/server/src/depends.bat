rem ���ģ��֮��ĵ�������, �����������ģ��֮��ĺ��������Ƿ����
erl -pz ..\ebin ..\ebin\erlsom ..\ebin\lib\exmpp\ebin\ -pz ..\test\erlymock\ebin -noshell -eval "io:format(\"~p~n\", [xref:d(\"../ebin\")]), c:q().
pause
