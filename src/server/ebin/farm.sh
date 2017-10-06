#!/bin/sh
ERL=/usr/local/bin/erl
cd `dirname $0`
exec $ERL -name 'farm@10.35.12.224' -setcookie abcd -pz ..\lib\exmpp\ebin -pz log -pz erlsom -boot start_sasl -config elog -s logger -s db_connection open eq@10.35.12.224 -s farmsvr -run gate start tcp_farm -detached
