%% -record(player, { username, create_date, login_ip }).
-record(tcp_mm_data, {
	  socket,     % client socket
	  addr,       % client address
	  account,    % ����ʺ�
	  player_sup, % �������ص�ϵͳ��pid����� 
	  is_first_time_login=false, % �Ƿ��һ�ν���Ϸ
	  is_wallow_account=false,   % �Ƿ�������û�
	  eq_pid,
	  pid         % �����̵�pid 
	 }).
