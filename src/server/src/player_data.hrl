-record(player_data, {
	  account = "",
	  basic_data = #player_basic_data{},         % 玩家基本信息
	  player_property=#player_property{},        % 玩家属性
	  scene_name,                                % 场景名称
	  mm_pid,                                    % 中间人:网关进程的PID
	  net_process = [],                          % 网络消息处理器
	  curr_pos=#point{x=0.0,y=0.0,z=0.0},        % 玩家当前点坐标
	  dest_pos=#point{x=0.0,y=0.0,z=0.0},        % 玩家目标点坐标
	  walk_plan=undefined,                       % 玩家此次行走的目的(使用家具，与人互动等等)
	  actions = [],                              % 当前动作(编辑房屋，交易等等)       
	  invite_info,                               % 邀请信息
	  trade_info,                                % 交易相关信息
	  work_info,                                 % 打工信息
	  domestic_info,                             % 家政信息
	  body_action_info,                          % 动作表情信息
	  body_state=1,                              % 坐，躺等状态 1表示发呆
	  mail_count = 0,                            % 当前客户端邮件数
	  login_time,                                % 玩家登陆时间
	  is_first_time_login,                       % 是否首次登陆
	  player_coin,                               % 货币
	  used_furni_inst_id = 0,                    % 家具的实例id
	  used_furni_func_id = 0,                    % 家具使用的功能id
	  used_furni_index = 0,                      % 使用的家具位置
	  used_furni_pos = #point{x=0.0,y=0.0,z=0.0},% 使用家具的坐标	  
	  start_edit_house_time,                     % 什么时候开始编辑房屋
	  wallow_info,                               % 防沉迷相关信息

	  flower_count=0,                            % 鲜花数                
	  task,                                      % 任务

	  bag_grids,                                 % 玩家包裹
	  body_grids,                                % 玩家装备
	  depot,                                     % 玩家仓库	  
	  
	  gift,                                      % 玩家礼物
	  gift_service_type,                         % 玩家礼物类型
	  property_timer_ref                         % 计算玩家属性的定时器
	 }).

