#!/bin/sh
ERL=/usr/local/bin/erl
cd `dirname $0`
exec $ERL -name 'eq@10.35.12.224' -setcookie abcd -mnesia dump_log_write_threshold 50000 -mnesia dc_dump_limit 40 +P 100000 +K true -env ERL_MAX_PORTS 10000 -pz log -pz erlsom  -boot start_sasl -config elog -s logger -s guid start_link -s db_connection open localhost -s gamesvr -run gate start tcp_server -detached 
