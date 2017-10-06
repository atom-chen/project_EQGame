1. 如何编译服务器程序: 执行src目录下的make.bat即可
2. 如何运行服务器程序: 执行ebin目录下的run.bat即可, 退出服务器,请在控制台里执行q(). 注意要输入"."
3. 问题: 如果出现版本不匹配, 请更新服务端的代码, 然后运行src目录下的make.bat即可
4. 设置远程数据库，需把本地数据库清空, 并修改run.bat。 例如想连接224数据库，需进行如下修改
   原先 db_connection open localhost
   改为 db_connection open eq@10.35.12.224 
