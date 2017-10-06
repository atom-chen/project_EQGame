#!/bin/sh
ERL=/usr/local/bin/erl
cd `dirname $0`
exec $ERL -name 'web@10.35.12.224' -setcookie abcd -pa $PWD/ebin $PWD/deps/*/ebin -boot start_sasl -s eqweb -detached
