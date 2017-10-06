rem 检查模块之间的调用依赖, 编译器不检查模块之间的函数调用是否存在
erl -pz ..\ebin ..\ebin\erlsom ..\ebin\lib\exmpp\ebin\ -pz ..\test\erlymock\ebin -noshell -eval "io:format(\"~p~n\", [xref:d(\"../ebin\")]), c:q().
pause
