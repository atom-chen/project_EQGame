%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  excel映射文件的结构体定义, 定义完结构体后, 程序会生成相应的资源映射文件供excel使用
%%% @end

%%% Created : 22 Mar 2010 by  <>

%% 类型的定义包括了: int, float, string, list_int, list_float, list_string
%% 其中以list_ 打头的都是列表的形式, 模板表里填写的格式如下 1, 2, 3 要以逗号为分隔符
%% 其中以range 表示区间,  模板表里填写的格式如下 1~2,  最终生成的数据为元组{1, 2}。 也可直接填2, 表示{2, 2} 

-define(tplt_list,(
	  [{house_tplt,     % 房屋模板表
	    [{int, id},     % id
	     {string, name},% 名称
	     {int, level},  % 等级
	     {int, type},   % 类型
	     {string, scene_id},        % 场景id，客户端用
%%	     {string, res},  % 对应的房屋模型资源
	     {string, house_furniture}, % 对应的房屋附属家具
	     {int, born_id}, %出生点Id
	     {int, max_players},              % 最大人数
	     {int, preview_born} %预览出生点
	    ]},
	   {test_tplt,        % 测试用的模板表
	    [{int, id},       
	     {string, name}   
	    ]},
	   {gm_friend_tplt,       % gm好友(默认自动加到玩家身上)
	    [{string, account},   % 账号    
	     {string, name},      % 中文名字
	     {int, house_id},     % 对应的房屋模板id
	     {int, first_time_magic_box}, % 首次宝箱
	     {int, first_time_garbage_box} % 首次垃圾
	    ]},
	   {house_furniture_tplt, % 房屋附属家具(给房屋的时候的默认家具)
	    [{int, id},           % id
	     {int, furniture_id}, % 家具ID
	     {int, item_temp_id}, % 对应的物品模板ID(用于把家具转换成物品)
	     {int, x},      % x坐标
	     {int, z},      % y坐标
	     {int, height}, % 离地高度
	     {int, flr},      % 楼层
	     {int, face}      % 朝向
	    ]},
	   {item_tplt,        % 物品模板表
	    [{int, id},       % 物品的ID
	     {string, name},  % 物品的名称
	     {int, type},     % 物品归属的类型
	     {string, imageset}, % icon所在的图片集
	     {string, icon},  % 物品的图标
	     {int, intro_id}, % 物品的说明ID
	     {int, overlap},  % 最大堆叠数
	     {int, bind},     % 是否绑定(绑定的物品不能丢弃, 交易等)
	     {int, use_del},  % 物品使用后是否从包裹中删除
	     {int, sell_price},% 售价(0表示不可卖出)
	     {int, sub_id},    % 关联的子表ID
	     {int, effect_time_id}, % 时效id
	     {int, use_id},
	     {int, voiceid},   % 音效相关
	     {int, dvoiceid},  % 音效相关
	     {int, uvoiceid},   % 音效相关
	     {int, decoration}  % 家装值
	    ]},
	   {item_dress_tplt,  % 物品服装表
	    [{int, id},       % ID
	     {string, name},  % 名称
	     {int, sex},      % 性别
	     {int, equip_pos},% 装备的位置
	     {list_int, equip_placeholder}, % 占位符
	     {int, equip_disp_id}, %装备后显示ID. 对应EquipDisplay.xml(装备表现表)的ID
	     {int, place},    % 存放方式
	     {string, action},% 动作
	     {int, type1},    % 类型1
	     {int, type2}     % 类型2
	    ]},
	   {house_comp,       % 房屋部件表
	    [{int, id},       % 部件的ID
	     {string, name},  % 名称
	     {int, prop_id},  % 
	     {int, dis_id},   % 
	     {string, pos},   % 坐标
	     {string, rot},   % 翻转
	     {string, scale}  % 缩放
	    ]},
	   {common_scene,     % 公共场景
	    [{int, id},       % 场景id
	     {string, name},  % 中文名称
	     {string, file_res},% 文件资源
	     %% {float, x},
	     %% {float, y},
	     %% {float, z},
	     {int, born_id}, %出生点Id
	     {int, max_players},            %%允许的最大玩家数
	     {int, busy_players},           %%繁忙状态玩家数
	     {int, loose_players}           %%宽松状态玩家数
	    ]},	   
	   {scene_copy,                              %% 产品副本表
	    [          
	     {int, id},
	     {int, scene_id},                        %% 场景ID      
	     {int, copy_id},                         %% 副本ID
 	     {string, name}                          %% 场景名字  
	    ]},
	   {mutex_actions,    % 动作互斥
	    [{int, id},       % id  
	     {string, name},  % 中文名称
	     {list_int, allow_actions} % 允许的动作描述
	    ]},	   
	   {domestic_service_tplt,    % 家政服务
	    [{int, id},               % id  
	     {string, name},
	     {int, house_clean},      % 房屋清洁度
	     %% {int, property_id},      % 影响玩家属性id
	     {int, money_rate},       % 打折率(单位:百分之一)
	     {int, discount_rate},     % 打折触发机率(单位:百分之一)
	     {string, image}
	    ]},	   	   
	   {domestic_price_tplt,   % 家政服务价格
	    [{int, house_level},   % 房屋等级  
	     {int, money}          % 每点清洁度 消耗的金钱 
	    ]},	   	   
	   {work_tplt,             % 打工
	    [{int, id}, 
	     {int, grade},         % 等级
	     {int, money},         % 可获得的金钱数
	     {int, need_clean},             % 人物清洁
	     {int, need_health},            % 人物健康
	     {int, need_charm},             % 人物魅力
	     {int, need_active},            % 活跃值
	     {int, need_disease},           % 疾病值
	     {int, property_id},            % 加减玩家属性
	     {int, event_rate},             % 触发事件总机率           
	     {list_int, event_ids},         % 触发事件列表           
	     {list_int, event_rates},       % 各个事件触发机率
	     {string, workImage},
	     {string, typeImageSet},
	     {string, typeImage}
	    ]},
	   {house_magic_box_tplt,          % 房屋宝箱
	    [{int, house_level},             % 房屋等级 
	     {list_int, box_ids},            % 可出现的宝箱id
	     {list_int, box_rates},          % 宝箱出现机率
	     {int, max_box_count},           % 最大宝箱数
	     {range, player_box_range},      % 每个玩家来该房屋，每次能产生的宝箱数量
	     {int, max_create_box_times}     % 每天宝箱的最大次数
	    ]},
	   {magic_box_tplt,                 % 宝箱
	    [{int, id},                      % 宝箱id 
	     {range, money_range},           % 随机金钱
	     {list_int, item_ids},           % 物品
	     {list_int, item_rates}          % 物品出现机率
	    ]},
	   {player_level_magic_box_tplt,     % 玩家宝箱表
	    [{int, player_level},            % 玩家等级 
	     {int, max_box_per_day},         % 每天能产生的最大宝箱次数
	     {int, reward_times}             % 正常奖励次数
	    ]},
	   
	   {house_garbage_tplt,            % 房屋垃圾
	    [{int, house_level},             % 房屋等级 
	     {list_int, garbage_ids},        % 可出现的垃圾id
	     {list_int, garbage_rates},      % 垃圾出现机率
	     {int, max_count}                % 最大垃圾数
	    ]},
	   {garbage_tplt,                 % 垃圾
	    [{int, id},                      % 垃圾id 
	     {range, money_range},           % 随机金钱
	     {list_int, item_ids},           % 物品
	     {list_int, item_rates}          % 物品出现机率
	    ]},	
   
	   {random_garbage_tplt,          % 随机垃圾
	    [{int, id},                      % 垃圾id 
	     {list_int, interval},           % 间隔分钟数
	     {list_int, garbage_count}       % 物品出现个数
	    ]},	   
	   
	   {work_event_tplt,                % 打工触发的事件
	    [{int, id},                     % 事件id
	    {int, reward_money},            % 金钱奖励
	    {list_int, reward_items},       % 物品奖励
	    {int, item_rate},               % 物品出现机率
	    {string, image}
	    ]},
	   {change_looks_tplt,             % 美容
	    [
	     {int, id},          % 性别 * 10000 + 改变(1.发型, 2.发色 3.脸形 4.肤色 5.胡子) * 1000 + 改变后的新值(新发型，新颜色)
	     {int, money_type},  % 费用类型(1为游戏金币，2为点卡点数)
	     {int, money}        % 费用
	   ]},
	   {teleport_tplt,      % 传送点
	    [
	     {int, id},           % 流水号
	     {int, src_scene_id}, % 出发场景id
	     {int, born_id}, %出生点Id
	     {float, src_x},        % 传送点
	     {float, src_y},
	     {float, src_z},
	     {float, src_radius},  % 传送点半径
	     {int, dest_scene_id}, % 目标场景id
	     %% {float, dest_x},      % 目标点
	     %% {float, dest_y},
	     %% {float, dest_z},
	     {int, dest_dir}      % 到达后人物朝向
	   ]},
	   {create_random_dress,     % 创建人物服装随机表
	    [{int, id},       % id
	     {int, sex},      % 性别
	     {int, fashion1}, % 套装1
	     {int, fashion2}, % 套装2
	     {int, fashion3}, % 套装3
	     {int, fashion4}, % 套装4
	     {int, fashion5}  % 套装5
	    ]},
	   {npc_tplt,             % npc
	    [{int, npc_id},      % npc主键
	     {int, body},        % 身体
	     {int, head},        % 头
	     {int, hair},        % 头发
	     {int, equip1},      % 初始装备1
	     {int, equip2},      % 初始装备2
	     {int, skeleton},    % 骨骼
	     {string, npc_name}  % npc名称
	    ]},
	   {npc_map_mapping_tplt,% 地图对应npc关系
	    [{int, id},         % 地图主键
	     {int, npc_id},     % 关联的npc
	     {string, npc_name},% npc名称
	     %% {float, x},        % x轴坐标
	     %% {float, y},        % y轴坐标
	     %% {float, z},        % z轴坐标
	     {int, born_id}, %出生点Id
	     {int, script_id},  % 执行脚本编号
	     {int, action},     % 自动播放动作
	     {int, towards}     % 朝向
	    ]},
	   {player_level_tplt,    % 玩家等级与时长对照关系
	    [{int, level},        % 玩家等级
	     {int, online_time} % 玩家在线时长
	    ]},
	   {player_property_tplt,     %% 系统时间与玩家属性的关系
	    [{int, property_id},                %% 关联的Id
	     {int, player_clean},        %% 人物清洁
	     {int, player_health},       %% 人物健康
	     {int, player_charm},        %% 人物魅力
	     {int, active_value}         %% 活跃值
	    ]},
	   {player_property_by_stime_tplt,
	    [{int, min_property},          %% 时间范围(最小时间)
	     {int, max_property},          %% 时间范围(最大时间)
	     {int, ref_id}                 %% 关联的Id
	    ]},
	   {sys_shop_tplt,            %% 系统商城
	    [{int, goods_id},         %% 商品ID
	     {int, item_id},          %% 物品ID
	     {int, type},             %% 分类
	     {int, sex},              %% 性别
	     {int, money_type},       %% 货币类型
	     {int, price},            %% 价格
	     {int, intro},            %% 介绍
	     {int, commend},          %% 推荐
	     {int, broadcast_id}      %% 系统公告id
	    ]},
	   {npc_shop_tplt,            %% 系统商城
	    [{int, goods_id},         %% 商品ID
	     {int, item_id},          %% 物品ID
	     {int, type},             %% 分类
	     {int, sex},              %% 性别
	     {int, price},            %% 玩家买进物品的价格
	     {int, intro},            %% 介绍
	     {int, sale_type}         %% 销售类型
	    ]},
	   {furni_tplt,               %% 家具模板表
	    [{int, furniture_id},     %% 家具Id
	     {int, max_use_player},   %% 最大使用人数
	     {int, use_sex},          %% 使用的性别 
	     {int, permission},       %% 使用权限
	     {list_int, function_list},%% 功能列表
	     {int, use_type},          %% 使用类型
	     {int, status_qty}        %% 状态数量
	    ]},
	   {item_effect_time_tplt,      %% 物品时效
	    [{int, id},                % id
     	     {int, type},              % 类型(1表示倒计时时效。2表示时间段时候。)
     	     {int, effect_time},       % 有效时间(天)
	     {int, trade_cut_time},    % 交易扣除时间(天)
	     {int, del_time}           % 删除时间(格式YYYYMMDD), 00:00开始删除
	    ]},
	   {furni_interact_tplt,
	    [{int, function_id},      %% 功能id
	     {int, property_id},      %% 玩家属性Id
	     {int, status},           %% 修改的家具状态
	     {int, action_type}       %% 动作类型(0:未分类, 3:洗手 11:洗澡)
	    ]},
	   {common_scene_furniture,
	    [{int, id},        %% id (到时当实例id来用)
	     {int, object_id}  %% 家具id
	    ]},
	   {player_task_action_tplt,
	    [{int, id},        %% id (拜访好友，捡宝箱之类的)
	     {string, desc}    %% 描述 (你访问了1/3好友)
	    ]},
	   {player_task_tplt,                       %% 玩家任务模板表
	    [{int, task_id},                        %% 任务Id
	     {string, title},
	     {string, intention},
	     {int, step_count},                     %% 任务所需要的步骤数量
	     {list_int, props},                     %% 接受任务获得的道具
	     {list_int, props_count},               %% 接受任务获得的道具数量
	     {list_int, require_items},             %% 需要的物品Id
	     {list_int, require_items_count},       %% 需要的物品个数
	     {string, describe},
	     {list_int, select_reward_items},       %% 选择奖励的物品
	     {list_int, select_reward_items_count}, %% 选择建立的物品个数
	     {list_int, fixed_reward_items},        %% 固定奖励的物品
	     {list_int, fixed_reward_items_count},  %% 固定奖励的物品个数
	     {int, reward_game_coin},               %% 奖励的游戏币
	     {int, reward_eq_coin},                 %% 奖励的EQ币
	     {int, time_limit},                     %% 时间期限(以秒为单位)
	     {int, property_id},                    %% 对应的玩家属性Id
	     {int, is_repeat},                      %% 是否可重复接
	     {int, is_show},                        %% 是否显示任务
	     {int, is_give_up},                     %% 是否是新手任务
	     {int, sub_id},                         %% 
	     {list_int, require_actions},           %% 需要的动作Id(比如说拜访好友之类)
	     {list_int, require_actions_times},     %% 需要的动作数量
	     {int, kind}                            %% 分类: 2 每日任务,1 新手任务
	    ]},
	   {farm_task_tplt,
	    [{int, task_id},                        %% 任务Id
	     {string, title},                       %% 任务标题
	     {string, intention},                   %% 任务目的
	     {string, describe},                    %% 任务描述
	     {string, imageset},                    %% 任务显示的图片
	     {string, image},                       %% 
	     {list_int, fixed_reward_items},        %% 固定奖励的物品
	     {list_int, fixed_reward_items_count},  %% 固定奖励的物品个数
	     {int, reward_game_coin},               %% 奖励的游戏币
	     {int, reward_eq_coin},                 %% 奖励的EQ币
	     {int, exp},
	     {int, time_limit},                     %% 时间期限(以秒为单位)
	     {int, is_repeat}                       %% 是否可重复接
	    ]},
	   {chain_task_tplt,                         %% 链式任务
	    [{int, task_id},                         %% 链式任务Id
	     {list_int, tasks},                      %% 任务列表
	     {int, is_repeat},                       %% 是否可重复接
	     {int, is_restart}                       %% 是否下线重新开始
	    ]},
	   {farm_seed_tplt,                         %% 种子
	    [{int, id},                             %% Id
	     {string, name},                        %% 名字
	     {int, expect_output},                  %% 预期产量
	     {int, fruit_id},                       %% 果实ID
	     {int, sprout_time},                    %% 发芽的时间
	     {int, s_leaf_time},                    %% 小叶子的时间
	     {int, b_leaf_time},                    %% 大叶子的时间
	     {int, bloom_time},                     %% 开花的时间
	     {int, fruit_time},                     %% 结果时间
	     {int, pick_time},                      %% 采摘时间
	     {int, drought_id},                     %% 旱灾的id
	     {int, pest_id},                        %% 虫灾的id
	     {int, grass_id},                       %% 杂草的id
	     {int, use_level},                      %% 使用等级(种植等级)
	     {int, pick_exp},                       %% 采摘能够获得的经验
	     {int, redland}                         %% 是否能种在红土地上(1--在普通土地和红土地上种植；
	                                            %% 2--为红土地种子，只能在红土地上种植)
	    ]},
	   {farm_damage_tplt,                       %% 农场灾害
	    [{int, id},                             %% 灾害Id
	     {int, sprout_ratio},                   %% 发芽阶段的灾害几率
	     {int, s_leaf_ratio},                   %% 小叶子阶段的灾害几率
	     {int, b_leaf_ratio},                   %% 大叶子阶段的灾害几率 
	     {int, bloom_ratio},                    %% 开花阶段的灾害几率
	     {int, fruit_ratio},                    %% 结果阶段的灾害几率
	     {int, interval},                       %% 灾害产生的间隔时间
	     {int, reduce_health}                   %% 影响健康度
	    ]},
	   {farm_pet_tplt,                          %% 农场宠物
	    [{int, pet_id},                         %% 宠物Id
	     {int, rate},                           %% 发现几率
	     {int, min_coin},                       %% 如果发现，随机扣除的最少金币
	     {int, max_coin}                        %% 如果发现，随机扣除的最多金币
	    ]},
	   {common_define_tplt,                     %% 公共定义模板表
	   [{string, key},                          
	    {string, value}
	   ]},
	   %% {farm_announcement_tplt,                 %% 公告
	   %%  [{int, id},                             %% 唯一Id
	   %%   {string, announcement}                    %% 公告Id
	   %%  ]},
	   {farm_shop_tplt,           %% 农场商城
	    [{int, item_id},          %% 物品ID
	     {int, gameprice},        %% 需要使用游戏币购买
	     {int, eqprice},          %% 需要使用eq点卡购买
	     {string, intro},         %% 介绍
	     {string, special},       %% 特殊用途
	     {int, level},            %% 等级需求
	     {int, repeat_buy},       %% 是否允许重复购买
	     {int, add_exp},          %% 购买该物品后增加的经验
	     {list_string, rewards}   %% 奖励列表
	    ]},
	   {farm_assart_tplt,         %% 农场开垦
	    [{int, plot_index},       %% 开垦的地块编号(第一块地, 第二块地, ...)
	     {int, level},            %% 农场等级
	     {int, money}             %% 需要消耗的游戏币
	    ]},
	   {farm_level_tplt,          %% 农场开垦
	    [{int, level},            %% 等级
	     {int, next_exp},         %% 下一级所需的经验
	     {int, sow_exp},          %% 播种获得的经验
	     {int, water_exp},        %% 浇水获得的经验
	     {int, weed_exp},         %% 除草获得的经验
	     {int, worm_exp},         %% 除虫获得的经验
	     {int, hoeing_exp},       %% 锄地获得的经验
	     {string, title}          %% 称号
	    ]},
	   {farm_decoration_tplt,     %% 农场装饰表
	    [{int, id},               %% 物品表Id
	     {int, type},             %% 装饰类型
	     {int, static_model},  %% 静态模型
	     {int, dynamic_model},    %% 动态模型
	     {string, scene_name}
	    ]},
	   {farm_decoration_init_tplt,
	    [{int, background},       %% 背景
	     {int, house},            %% 房屋
	     {int, doghouse},         %% 狗窝
	     {int, signpost},         %% 路标
	     {int, fence},            %% 篱笆 
	     {int, scarecrow},        %% 稻草人
	     {int, billboard},        %% 告示牌
	     {int, billboard_content} %% 告示牌内容
	    ]},
	   {farm_setting_tplt,        %% 农场设置
	    [{string, water},         %% 浇水
	     {string, weed},          %% 除草
	     {string, worm},          %% 除虫
	     {string, put_grass},     %% 放草
	     {string, put_pest}       %% 防虫
	    ]},
	   {player_disease_probability_tplt,  %% 玩家疾病触发几率表
	    [{int, player_health},          %% 人物清洁度
	     {int, probability}               %% 触发几率
	    ]},
	   {player_disease_type_tplt,         %% 疾病类型模板表
	   [{int, type},                      %% 疾病类型
	    {int, rate},                      %% 得不同疾病的比例
	    {int, upgrate_probability},       %% 提高触发几率
	    {int, level},                     %% 疾病的等级
	    {int, treatment_costs},           %% 治疗费用
	    {int, min},                       %% 最小浮动值
	    {int, max},                       %% 最大浮动值
	    {list_int, special_event_id},     %% 特殊事件Id
	    {list_int, event_id_probability}, %% 特殊事件Id的几率
	    {int, special_event_probability}  %% 特殊事件几率
	   ]},
	   {player_disease_special_event_tplt,%% 疾病特殊事件模板表
	     [{int, special_event_id},          %% 特殊事件Id
	     {int, property_id},               %% 影响的属性Id
	     {string, image}                   %% 图片地址
	    ]},
	   {farm_prop_tplt,                    %% 农场道具
	    [{int, id},                        %% 物品Id
	     {int, type},                      %% 类型
	     {int, effect_range},              %% 影响范围
	     {int, effect_time},               %% 影响时间
	     {int, effect_output},             %% 影响产量
	     {list_int, effect_stage},         %% 影响阶段
	     {int, reduce_time}                %% 减少时间
	    ]},
	   {house_transaction_tplt,           %% 房屋交易表
	   [{int, id},                        %% 主键Id
	    {string, name},                   %% 名称
	    {int, type},                      %% 类型
	    {int, recommend},                 %% 推荐
	    {int, level},                     %% 等级
	    {int, pay_type},                  %% 支付类型
	    {int, price},                     %% 价格
	    {int, cost_flower},               %% 消耗鲜花值
	    {int, need_decoration},           %% 需要家装值
	    {int, house_id},                  %% 对应的房屋模板Id
	    {string, intro},                  %% 简介
	    {int, broadcast_id}               %% 系统公告id
	   ]},
	   {sys_broadcast_tplt,               %% 系统公告
	   [{int, id},                        %% 主键Id
	    {string, content},                %% 内容
	    {int, show_seconds}               %% 显示时间
	   ]},
	   {player_property_interaction_tplt,
	    [{int, id},
	     {int, monitor_player_clean},           %% 人物清洁
	     {int, monitor_player_health},          %% 人物健康
	     {int, monitor_player_charm},           %% 人物魅力
	     {int, monitor_active_value},           %% 活跃值
	     {int, ref_id}                          %% 关联的属性Id
	    ]},
	   {farmland_upgrade_tplt,                  %% 农场红土地开垦表
	    [{int, land_num},                       %% 系统约定红土地块
	     {int, level},                          %% 需要的等级
	     {int, money}                           %% 需要的金币
	    ]},
	   {red_farmland_harvest_tplt,              %% 红土地收获奖励表
	    [{int, id},                             %% 不重复的id, 没有实际用途
	     {int, ratio},                          %% 几率
	     {int, type},                           %% 类型
	     {int, data},                           %% 和类型匹配的数据段
	     {int, count}                           %% 和data匹配的数量
	    ]},
	   {farmfruit_aberrance_tplt,               %% 农场果实变异
	    [{int, seed_id},                        %% 种子ID参见farm_seed_tplt的id
	     {int, ratio1},
	     {int, fruit_id1},
	     {int, ratio2},
	     {int, fruit_id2},
	     {int, ratio3},
	     {int, fruit_id3},
	     {int, ratio4},
	     {int, fruit_id4},
	     {int, ratio5},
	     {int, fruit_id5},
	     {int, ratio6},
	     {int, fruit_id6},
	     {int, ratio7},
  	     {int, fruit_id7},
	     {int, ratio8},
	     {int, fruit_id8},
	     {int, ratio9},
	     {int, fruit_id9},
	     {int, ratio10},
	     {int, fruit_id10},
	     {int, ratio11},
	     {int, fruit_id11},
	     {int, ratio12},
	     {int, fruit_id12}
	    ]},
	   {gift_box_tplt,                            %% 礼物盒模板表
	    [{int, id},                               %% 礼物Id
	     {int, type},                             %% 礼物类型
	     {string, name},                          %% 礼物名称
	     {int, game_coin},                        %% 需要的游戏币
	     {int, eq_coin},                          %% 需要的EQ币
	     {int, model_id},
	     {string, imageset},
	     {string, image},
	     {string, desc}                           %% 盒子介绍
	    ]},
	   {gift_card_tplt,                           %% 礼物卡模板表
	    [{int, id},                               %% 礼物卡Id
	     {int, type},                             %% 礼物卡类型
	     {string, name},                          %% 礼物卡名称
	     {int, game_coin},                        %% 需要的游戏币
	     {int, eq_coin},                          %% 需要的EQ币
	     {string, imageset},
	     {string, image},
	     {string, desc},                          %% 礼物卡介绍
	     {string, url}
	    ]},	   
	   {day_night_config_tplt,                    %% 白天黑夜
	    [{list_int, hh_mm_ss},                    %% 时间
	     {int, is_day}                            %% 是否白天
	    ]},	   
	   {farmhoeing_awards_tplt,                 %% 农场翻地奖励表
	    [{int, level},                          %% 农场等级
	     {int, ratio},                          %% 几率
	     {list_int, item_id},                   %% 物品id(数组)
	     {int, count}                           %% 物品的数量
	    ]},
	   {born,                              %% 出生点表
	    [{int, id},                            
	     {list_space_int, pos},
	     {int, dir},
	     {string, lookat},
	     {int, pitch},
	     {int, yaw},
	     {int, zoom}
	    ]
	   },
	   {                               %%丘比特之箭的tag
	     cupid_tag,
	     [{string, tag}]
	   }
	  ])
       ).
