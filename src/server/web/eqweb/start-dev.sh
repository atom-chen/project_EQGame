#!/bin/sh
ERL=/usr/local/bin/erl
DIR=/root/server/web/eqweb
cd `dirname $0`
make
exec erl -pa $DIR/ebin $DIR/deps/*/ebin -boot start_sasl -s reloader -s eqweb -name 'web@10.35.12.216' -setcookie abcd
