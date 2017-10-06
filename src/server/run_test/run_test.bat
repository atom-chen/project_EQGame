@echo off
rm ../test/*flymake*
cd ..\ebin
..\run_test\run_test.py -pz log -pz erlsom -pz ../test -run db_update start db_update.cfg -run gate start log4erl.conf tcp_server_test -dir ../ -logdir ../run_test/log
pause