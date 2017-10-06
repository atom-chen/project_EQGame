echo "========copy boost========"

xcopy %__boost__%\boost ..\include\boost /s /e /i /q /y

xcopy %__boost__%\stage\lib\libboost_date_time-vc90-mt-?_??.lib ..\lib\ /y
xcopy %__boost__%\stage\lib\libboost_date_time-vc90-mt-gd-?_??.lib ..\lib\ /y
xcopy %__boost__%\stage\lib\libboost_thread-vc90-mt-?_??.lib ..\lib\ /y
xcopy %__boost__%\stage\lib\libboost_thread-vc90-mt-gd-?_??.lib ..\lib\ /y
