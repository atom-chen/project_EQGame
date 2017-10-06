%%%%%%%%%%%%%%%%所有在线的场景%%%%%%%%%%%%%%%%%%%%
-record(scene, {scene_name,      	%% 场景名称
		create_date		%% 创建日期
	       }).

-record(house, {house_name,     %% 玩家房屋
		create_date}).  %% 创建日期

%%%%%%%%%%%%%%%%所有在线的玩家%%%%%%%%%%%%%%%%%%%%
-record(online_player, {
	  account,              %% 玩家帐号
	  login_date,           %% 玩家登陆日期
	  login_node            %% 登录的节点
	 }).
