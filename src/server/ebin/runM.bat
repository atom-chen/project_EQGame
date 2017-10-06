erl -name eqg -setcookie abcd -pz lib\exmpp\ebin -pz log -pz erlsom -boot start_sasl -config elog -s logger -s guid start_link -s db_connection open localhost -s gamesvr -run gate start tcp_server 
