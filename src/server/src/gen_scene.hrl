-include("packet_def.hrl").
%% 移动的数据
-record(move_data, {
	  pos=#point{x=0.0,y=0.0,z=0.0},    %%玩家坐标
	  path,                             %%行走路径
	  speed,                            %%移动速度
	  pos_index                         %%当前行走到的点的索引
	 }).


%%场景数据 
-record(scene_data,{
	  scene_name,            %% 场景名称
	  players = dict:new(),  %% 玩家列表, 一个dict
	  enter_pos=#point{x=0,y=0,z=0}, %% 进入点
	  module,                %% 模块名
	  custom_data            %% 房屋或公共场景的自定数据
	 }).
