%% -record(player, { username, create_date, login_ip }).
-record(tcp_mm_data, {
	  socket,     % client socket
	  addr,       % client address
	  account,    % 玩家帐号
	  player_sup, % 和玩家相关的系统的pid监控树 
	  is_first_time_login=false, % 是否第一次进游戏
	  is_wallow_account=false,   % 是否防沉迷用户
	  eq_pid,
	  pid         % 主进程的pid 
	 }).
