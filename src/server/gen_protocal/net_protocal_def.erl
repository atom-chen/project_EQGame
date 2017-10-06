% Author: NoteBook
%% Created: 2009-9-9
%% Description: 网络协议模块的定义
-module(net_protocal_def).


-export([get_struct_def/0, get_enum_def/0, get_version/0]).

%% 获得消息结构体的定义
get_struct_def() -> 
    [
     [req_ver_validate,                                   % 请求版本验证
      {int, ver}                                          % 版本编号        
     ],
     [notify_ver_validate,                                % 返回验证信息
      {ver_validate_result, result}
     ],

     %%------------------------公共数据---------------------------
     % 坐标数据, 现在这个只作为结构体用，不作为消息
     [stime, 
      {int, year},
      {int, month},
      {int, day},
      {int, hour},
      {int, minute},
      {int, second}
     ],
     [point,
      {float, x},
      {float, y},
      {float, z}
     ],

     [grid_pos,  %% 格子坐标点
      {int, x},
      {int, y},
      {int, z}
     ],
     
     [item,                                               % 物品
      {uint64, instance_id},                              % 物品实例ID
      {int,    template_id},                              % 物品模板ID
      {stime,  del_time},                                 % 删除时间
      {int, decoration_val}                               % 家装值
     ],

     [player_basic_data,                                  % 玩家基础数据
      {string, account},                                  % 用户帐号
      {string, username},                                 % 用户名称
      {int, sex},                                         % 性别
      {int, skin_color},                                  % 肤色(身体)
      {int, hair},                                        % 发型
      {int, face},                                        % 脸型
      {int, beard},                                       % 胡子
      {int, age},                                         % 年龄
      {int, level},                                       % 等级
      {float, online_time},                               % 在线时长
      {disease_type, health_status},                      % 健康状况
      {string, personality_sign},                         % 个性签名
      {string, personality_describe},                     % 个性说明
      {int, hair_color},                                  % 发色
      {stime, last_login_time},                           % 最后登录时间
      {gm_level, gm_enabled},                             % 用户是否可以使用GM命令
      {stime, logout_time}                           % 最后登出时间
     ],

     [player_property,                                    % 玩家属性
      {string, account},
      {int, player_clean},                              % 人物清洁
      {int, player_health},                             % 人物健康
      {int, player_charm},                              % 人物魅力
      {int, active_value}                               % 活跃值
     ],

     [pack_grid,                                          % 包裹格
      {int, count},                                       % 格子里面存放的物品的数量
      {int, lock},                                        % 格子是否被锁住
      {item, item_data}                                   % 物品数据
     ],

     [player_body,                                        % 角色的身体, 主要是装备的容器
      {string, owner},                                    % 用户帐号
      {array, pack_grid, grid_vec}                        % 装备(包括:帽子, 衣服, 裤子, 鞋子, 手持物, 披风)
     ],

     [player_pack,                                        % 角色的包裹
      {string, owner},                                    % 用户帐号
      {array, pack_grid, grid_vec}                        % 装备(包括:帽子, 衣服, 裤子, 鞋子, 手持物, 披风)
     ],

     [player_info,
      {player_basic_data, basic_data},                    % 玩家基本信息
      {string, scenename}                                 % 玩家所在场景名称
     ],  

     [friend_info,
      {player_basic_data, basic_data},                    % 玩家基本信息
      {int, is_online}                                    % 玩家是否在线(1: 在线, 2: 不在线)
     ],
     
     [mail_info,                               %% 邮件信息
      {uint64, id},                               %% 邮件ID 
      {int, type},                                %% 类型(个人, GM, 系统, 退信)
      {int, read_status},                         %% 是否已读取(1:未看过，2:看过标题，3:看过内容)
      {int, remain_time},                         %% 剩余时间(秒)
      {string, sender_name},                      %% 发送者名字
      {string, sender_account},                   %% 发送者账号
      {string, title},                            %% 标题      
      {array, pack_grid, items}                   %% 物品   
     ],

     [npc_info,
      {int, npc_id},                                      % Npc主键
      {int, body},                                        % 身体
      {int, head},                                        % 头
      {int, hair},                                        % 头发
      {int, equip1},                                      % 初始装备1
      {int, equip2},                                      % 初始装备2
      {int, skeleton},                                    % 骨骼
      {string, npc_name}                                  % npc名称(注释用)
     ],

     [npc_map_mapping_info,
      {int, id},
      {int, npc_id},                                      % 关联的npc主键
      {string, npc_name},                                 % npc名称(显示在地图上)
      {int, born_id},                                       % npc坐标
      {int, script_id},                                   % 执行的脚本编号
      {int, action},                                      % 自动播放的动作
      {string, npc_key},                                  % npc在地图中的唯一标识
      {int, towards}
     ],

     [npc_command,
      {int, template_id},
      {uint64, unique_id},
      {int, page_index},
      {int, command_index}
     ],

     [furniture_position,                                 % 家具使用位置
      {int, position_index},                              % 索引位置
      {int, is_used},                                     % 是否被使用
      {string, used_account},                             % 使用帐号
      {int, status},                                      % 家具状态
      {int, func_id},                                     % 功能Id
      {stime, use_time}                                   % 使用时间 
     ],

     [player_task_action,                                 % 任务动作(访问好友，捡垃圾之类的)
      {task_action_type, type},                           % 什么动作 
      {int, finish_amount}                                % 完成次数 
     ],

     [player_task,                                        % 玩家任务
      {uint64, id},                                       % 主键id
      {string, account},                                  % 玩家帐号
      {int, task_id},                                     % 任务Id
      {int, step_index},                                  % 步骤索引
      {int, track},                                       % 跟踪任务
      {int, status},                                      % 任务状态
      {stime, create_date},                               % 创建日期
      {array, player_task_action, action_list}            % 完成任务所需的动作列表(拜访好友，捡垃圾之类的)
     ],

     [eq_user_info, % EQ用户信息
      {string, full_name},   % 全名
      {string, nick_name},   % 昵称
      {string, birthday},    % 生日
      {string, phone},       % 电话
      {string, url},         % 个人主页
      {string, email},       % 邮箱
      {string, country},     % 国
      {string, region},      % 省
      {string, city},        % 市
      {string, street},      % 街
      {string, org_name},    % 公司
      {string, org_unit},    % 部门
      {string, title},       % 职位
      {string, role},        % 角色
      {string, desc},        % 个人说明
      {string, photo_type},  % 头像图片类型(JEPG, PNG)
      {string, photo_data}   % 头像图片数据
     ],

     [offline_invite_record,        % 离线邀请
      {int, invite_type},    % 邀请类型
      {string, invitor},     % 邀请方   
      {string, invitor_name},
      {stime, date}          % 时间
     ],

     [friend_setting_record,                                     % 好友相关设置
      {accept_invite_type, accept_all_invite}                    % 允许他人的好友邀请 
     ],

     [magic_box,          % 宝箱信息
      {magic_box_type, type},          % 类型 mbt_magic_box: 宝箱, mbt_garbage: 垃圾
      {uint64, inst_id},    % 实例id
      {grid_pos, pos},      % 格子位置
      {int, graphic_id}     % 图形表现id
     ],

     [gift_box,                                          % 礼物盒
      {uint64, inst_id},                                 % 礼物盒实例Id
      {string, sender},                                  % 发送者
      {string, sender_name},                             % 发送者名称
      {string, receiver},                                % 接受者
      {string, receiver_name},                           % 接收者名称
      {string, name},                                    % 礼物名称
      {array, pack_grid, grid_vec},                      % 包裹
      {int, box_id},                                     % 礼物盒
      {int, card_id},                                    % 礼物卡
      {string, comment},                                 % 赠言
      {grid_pos, pos},                                   % 礼物位置
      {stime, date},                                     % 记录日期
      {anonymous_type, is_anonymous}                     % 是否匿名
     ],

     [flower_log,                  %% 鲜花记录
       {stime, time},
       {string, who}
      ],

      [egg_log,                    %% 鸡蛋记录
       {stime, time},
       {string, who}
      ],
     %-----------------------登录流程---------------------------

     [req_login,                                          % 请求登录
      {string, account},                                  % 用户帐号
      {string, password}                                  % 用户密码
     ],

     [notify_login_result,                                %返回登录结果
      {login_result, result}
     ],

     [notify_repeat_login,                                % 通知客户端重复登陆
      {string, account}                                   % 用户帐号
     ],

     [req_get_roles                                       % 请求获取角色列表
     ],

     [notify_get_roles_result,                            % 返回角色列表
      {array, player_info, player},                       % 玩家数组
      {string, nick_name}
     ],

     [req_create_role,                                    % 请求创建角色
      {player_basic_data, basic_data},                    % 玩家信息
      {array, item, equips}                               % 玩家身体,用于盛放装备(时装)
     ],

     [notify_create_role_result,                          % 返回角色创建结果
      {create_role_result, result}
     ],

     [req_enter_scene                                     % 请求进入场景
     ],

     [notify_enter_scene_result,                          % 返回登录信息
      {enter_scene_result, result}                        % 登录结果
     ],
  
     [notify_body_data,                                   % 发送玩家身体数据给客户端
      {array, pack_grid, grid_vec}                        % 玩家的身体数据
     ],

     %-----------------------切换场景----------------------------
     [req_enter_player_house,    % 请求进入目标玩家房屋
      {string, account}          % 目标玩家帐号
     ],

     [req_enter_common_scene,    % 进入公共场景
      {int, scene_id},           % 场景ID
      {int, copy_id}             % 场景副本ID,默认值为1
     ],     
     [notify_enter_common_scene, % 自已进入公共场景
      {int, scene_id},
      {int, born_id}            % 进入点
     ],

     [notify_enter_player_house, % 自已进入玩家场景(进入自已家 或 进入朋友家都会收到此消息)
      {string, account},         % 进入自已家时account为自已
      {int, born_id},            % 进入点
      {int, house_tplt_id}       % 房屋的模板ID
     ],
     
     [req_teleport,              % 请求传送
      {int, id},                 % 传送点id
      {int, copy_id}             % 场景副本id,默认值应该为1
     ],

     [req_open_change_scene_guide        % 开启切换切景路标
     ],
     
     [notify_open_change_scene_guide
     ],

     [req_close_change_scene_guide       % 结束切换切景路标
     ],

     [notify_close_change_scene_guide
     ],

     [notify_finish_enter_scene          % 进入场景完成
     ],

     [req_kick_guest,        % 踢人
      {string, target_player} % 被踢玩家
     ],

     [notify_other_player_data,         % 新玩家登陆后发送相关信息给用户
      {player_info, player},            % 玩家的基本信息
      {player_property, property},      % 玩家属性      
      {point, curr_pos},                % 玩家的当前点坐标
      {point, dest_pos},                % 玩家的目标点坐标
      {uint64, used_furni_inst_id},     % 使用的家具实例Id
      {int, used_furni_func_id},        % 使用的家具功能Id
      {int, used_furni_index}           % 使用的家具的索引
     ],
     
     [notify_other_player_info,         % 其它玩家信息
      {player_info, player},            % 玩家的基本信息
      {player_property, property},      % 玩家属性      
      {array, pack_grid, body}          % 装备信息 
     ],

     [req_other_player_info,            % 请求查看其它玩家信息
      {string, account}                 % 玩家账号
     ],

     [notify_player_enter_scene,    % 通知有玩家进入场景
      {string, account}             % 玩家帐号
     ],
     [notify_player_leave_scene,    % 通知有玩家离开场景
      {string, account}             % 玩家帐号
     ],
     %-----------------------------------------------------------		
     [req_logout % 请求退出
     ],

     [notify_player_data, 	 % 发送玩家的详细信息
      {player_basic_data, basic_data} %玩家的基本信息
     ],

     [req_walk,                  % 请求行走
      {array, point, path}       % 关键点数据的数组，参见req_pos
     ],
     %-------------------------------------------------------------
     % 行走相关
     [req_start_walk,    % 请求开始行走
      {point, curr_pos}, % 当前位置
      {point, dest_pos}, % 目标位置
      {int, move_type}   % 移动类型
     ],
     [notify_start_walk, % 通知周边玩家开始行走
      {string, account}, % 帐号
      {point, curr_pos}, % 当前位置
      {point, dest_pos}, % 目标位置   
      {int, move_type}   % 移动类型
     ], 

     [req_stop_walk,     % 请求结束行走
      {point, pos}       % 玩家当前位置
     ],
     [notify_stop_walk,  % 通知周边玩家停止行走
      {string, account}, % 帐号
      {point, pos}
     ],
 
     [req_sync_position, % 请求同步坐标
      {point, pos}
     ],

     [req_walk_for_use_furniture,  % 行走计划(使用家具
      {point, curr_pos}, % 当前位置
      {point, dest_pos},  % 目标位置
      {uint64, instance_id},    % 家具id 
      {int, function_id},       % 功能Id
      {int, move_type}   % 移动类型
     ],

     [req_walk_for_body_action, % 行走计划(人与人的互动动作)
      {point, curr_pos}, % 当前位置
      {point, dest_pos},  % 目标位置
      {string, my_action},  % 我的动作
      {string, you_action}, % 你的动作
      {string, you_account}, % 你的账号
      {point, you_pos},     % 你的位置
      {int, move_type}   % 移动类型
     ],

     %-------------------------------------------------------------
     % 好友相关
     [req_add_friend,       % 增加好友
      {string, friend_account}
     ],
     [req_remove_friend,     % 删除好友
      {string, friend_account}
     ],
     [notify_add_friend,     % 通知增加好友
      {string, friend_account},
      {friend_info, friend_infos}
      ],
     [notify_remove_friend,  % 通知删除好友
      {string, friend_account}
     ],
     [req_friend_list        % 取好友列表
     ],
     [notify_friend_list,    % 更新好友列表
      {array, friend_info, friend_vec}     % 好友列表
     ],
     [notify_friend_online, % 通知好友上下线
      {string, friend_account}
     ],
     [notify_friend_offline, % 通知好友上下线
      {string, friend_account}
     ],

     [req_eq_users_info,        % 请求EQ用户信息(批量请求)
      {array, string, account_list}  % 账号列表
     ],

     [notify_eq_user_info, % EQ用户信息
      {string, account},     % 账号
      {eq_user_info, info}
     ],

     [notify_eq_user_presence, % EQ用户个性签名
      {string, account},       % 账号
      {string, presence}
     ],

     [notify_offline_invites, % 离线邀请
      {array, offline_invite_record, inv_list}
     ],

     [req_friend_setting                     % 请求好友设置列表
     ],

     [notify_friend_setting,                              
      {friend_setting_record, setting}
     ], 

     [req_friend_setting_accept_all,          % 请求好友设置
      {accept_invite_type, accept_all}                                     
     ],

     [req_friend_setting_black_someone,       % 不接收某人的请求
      {string, who}                                     
     ],

     [req_search_player_by_nick,       % 昵称搜索
      {string, nick}
     ],

     [req_search_player_by_mail,       % 邮件搜索
      {string, mail}
     ],

     [notify_search_player,            % 通知搜索结果
      {array, string, players}
     ],
     %-------------------------------------------------------------
     % 聊天相关
     [req_chat_around,   % 说话, 周围的人可以听到
      {string, content}  % 说话内容
     ],

     [notify_chat_around, % 通知周围的人玩家说了一句话
      {string, account},  % 说话的玩家账号
      {string, player_name}, %% 玩家姓名
      {string, content}   % 说话内容
     ],

     [req_chat_tell,     % 密聊 
      {string, target_player}, % 目标玩家
      {string, content}  % 说话内容
     ],
     
     [notify_chat_tell, % 通知玩家收到密聊信息
      {string, speaker}, % 说话的玩家账号
      {string, speaker_name}, % 说话的玩家姓名
      {string, content} % 说话内容
     ],     
     [req_chat_big_expression,   % 大表情
      {int, exp_id}   
     ],

     [notify_chat_big_expression, % 通知周围的人玩家
      {string, account},  % 说话的玩家账号
      {string, player_name}, %% 玩家姓名
      {int, exp_id}   % 
     ],
     %-----------------------房屋相关--------------------------------
     [house_furniture,                                    % 房屋家具
      {uint64, instance_id},                              % 物件实例ID
      {int, template_id},                                 % 物件的模板ID
      {int, x},                                           % 坐标
      {int, z},                                           % 坐标
      {int, height},                                      % 离地高度
      {int, floor},                                       % 楼层
      {int, face},                                        % 朝向
      {int, item_tempid},                                 % 家具对应的物品模板表ID
      {int, max_use_player},                              % 最大可使用的玩家数量
      {array, furniture_position, position_list},         % 可以使用的位置
      {array, int, function_list},                        % 功能列表
      {int, use_sex},                                     % 使用性别
      {int, permission},                                  % 家具使用权限
      {int, use_type},                                    % 使用类型
      {int, status_qty},                                  % 状态数量
      {stime, del_time}                                   % 删除时间
     ],

     [room_tex,         % 房间墙纸, 地板
      {int, room_id},   % 房间ID
      {int, type},      % 类型(地板，墙)
      {string, tex},    % 纹理
      {int, floor_id}   % 楼层ID 
     ],

     [house_component_tex,   % 房屋内的组件的材质, 比如地砖,墙纸
      {string, component_name},% 需要被替换材质的物体的ID
      {string, new_tex}      % 新的材质
     ],

     [house_component_mesh,  % 房屋的固定物件, 比如窗户,门,可以认为是属于建筑物的一部分
      {string, component_name},% 被替换的固定物件的ID
      {string, new_mesh}     % 新的固定物件的mesh文件名
     ],

     [house_visit_log,                                 % 房屋访问记录
      {house_visit_type, type},
      {stime, datetime},
      {string, account},
      {string, username},
      {int, item_id}                                   % 物品类型id
     ],

     [house_data,                                         % 房屋数据
      {string, owner},                                    % 房屋的主人
      {uint64, instance_id},                              % 房屋实例ID
      {int, template_id},                                 % 房屋的模板ID
      {furniture_permission_type, furniture_permission},  % 使用家具权限
      {array, house_furniture, furniture_vec},            % 房屋内的家具集合
      {array, room_tex, room_tex_vec},                    % 房屋墙纸的集合
      {array, house_component_tex, component_tex_vec},    % 房屋内被替换的材质的集合
      {array, house_component_mesh, component_mesh_vec},  % 房屋内被替换的固定物件的集合
      {string, welcome_words},                            % 欢迎语
      {house_permission_type, house_permission},          % 房屋权限
      {array, house_visit_log, visit_log},                % 房屋访问记录
      {stime, buy_date},                                  % 购买日期
      {int, level},                                       % 等级  
      {array, gift_box, gift_box_vec},                    % 礼物集合
      {array, magic_box, magic_box_vec},                  % 宝箱(垃圾)集合
      {int, house_clean}                                  % 房屋清洁度
     ],

     [notify_house_clean,                                 % 通知玩家房屋清洁度变化
      {int, house_clean}                                  % 房屋清洁度
     ],

     [notify_house_data, % 发送房屋的数据
      {house_data, data} % 房屋数据
     ],

     [notify_house_gift_box,
      {array, gift_box, gift_box_vec}
     ],

     [req_placed_furniture, % 请求摆放家具
      {int, grid_index},    % 背包格子的索引
      {int, x},           % x坐标
      {int, z},           % z坐标
      {int, height},      % 离地高度
      {int, floor},         % 楼层
      {int, face}           % 朝向
     ],

     [req_start_edit_house % 请求进入家装模式
     ],
     
     [notify_start_edit_house % 通知玩家进入家装模式
     ],

     [req_end_edit_house   % 请求结束家装模式      
     ],

     [notify_end_edit_house % 通知玩家结束家装模式
     ],

     [req_recovery_furniture, % 请求回收家具
      {uint64, furniture_id}  % 家具的ID
     ],

     [req_replace_component_mesh, % 请求替换组件的mesh
      {house_component_mesh, component_mesh}
     ],

     [req_replace_component_tex, % 请求替换组件的texture
      {house_component_tex, component_tex}
     ],

     [req_replace_room_tex,
      {room_tex,room_tex_info},
      {int, goods_id}
     ],

     [notify_replace_room_tex, % 改变房间纹理
      {room_tex, room_tex_info} % 房间纹理信息
     ],
     [req_move_furniture,       % 请求移动家具
      {uint64, instance_id},    % 家具的实例ID
      {int, x},                 % x
      {int, z},                 % z
      {int, height},            % 离地高度
      {int, floor},             % 楼层
      {int, face}               % 朝向
     ],
     [notify_add_furniture,     % 通知客户端添加了一个新的家具到房间里
      {uint64, instance_id},    % 家具的实例ID
      {int,    template_id},    % 家具的模板ID
      {int, x},                 % x
      {int, z},                 % z
      {int, height},            % 离地高度
      {int, floor},             % 楼层
      {int, face},              % 朝向
      {stime, del_time}         % 删除时间
     ],
     [notify_delete_furniture,  % 通知客户端删除家具
      {uint64, instance_id}     % 家具实例ID
     ],

     [req_house_basic_data                                % 请求房屋数据
     ],

     [notify_house_basic_data,                            % 返回房屋数据
      {house_data, data}                                  % 房屋数据
     ],

     [req_set_house_welcome_words,                        % 请求设置欢迎语
      {string, words}
     ],

     [notify_set_house_welcome_words,                     % 返回设置欢迎语
      {set_house_welcome_words_result, result}
     ],

     [req_set_house_permission,                           % 请求设置房屋权限
      {house_permission_type, house_permission},
      {furniture_permission_type, furniture_permission}
     ],

     [notify_set_house_permission,                        % 返回设置房屋权限
      {set_house_permission_result, result}
     ],

     [req_clear_house_log                                 % 请求清空房屋日志
     ],

     [notify_clear_house_log,                             % 返回清空房屋日志结果
      {clear_house_log_result, result}
     ],

     [req_change_house,                                   % 请求切换房屋
      {buy_house_replace_type, type},                     % 切换房屋是否保存之前房屋的设置
      {uint64, inst_id}                                   % 房屋的实例Id
     ],

     [notify_change_house,                                % 通知切换房屋成功
      {uint64, inst_id}
     ],

     [req_house_list,                                     % 请求房屋列表
      {string, account}
     ],
     
     [house_plot_info,                                    % 房屋信息
      {uint64, instance_id},                              % 房屋实例Id
      {int, template_id},                                 % 房屋模板Id
      {stime, buy_date},                                  % 房屋购买日期
      {uint64, plot_id}                                   % 房屋所在地块
     ],

     [notify_house_list,                                  % 通知房屋列表
      {string, account},
      {array, house_plot_info, house_vec}                 % 房屋列表
     ],
     %----------------------玩家属性-------------------------------------
     [notify_player_property_changed,                     % 通知玩家属性被改变
      {int, player_clean},                              % 玩家清洁度
      {int, player_health},                             % 玩家健康度
      {int, player_charm},                              % 玩家魅力值
      {int, active_value}                               % 活跃度
     ],
     %----------------------家具相关-------------------------------------
     [control_furni_info,
      {uint64, instance_id},    % 家具id 
      {int, function_id}       % 功能Id
     ],

     [mutli_furni_info,
      {string, account},                                  % 玩家帐号
      {int, position_index},                              % 允许的索引值
      {uint64, instance_id},                              % 家具id
      {int, status},
      {int, function_id}                                  % 功能id
     ],

     [req_use_mutli_furni,
      {array, control_furni_info, furni_vec}
     ],

     [notify_use_mutli_furni,
      {array, mutli_furni_info, furni_vec}
     ],

     [notify_use_furniture_result,                        % 返回是否允许使用家具
      {string, account},                                  % 玩家帐号
      {int, position_index},                              % 允许的索引值
      {uint64, instance_id},                              % 家具id
      {int, status},
      {int, function_id}                                  % 功能id
     ],
     
     [req_stop_using_furniture,                           % 请求停止使用家具
      {string, account}                                   % 玩家帐号
     ],

     [notify_stop_using_furniture_result,                 % 通知停止使用家具
      {string, account},                                  % 玩家帐号
      {int, position_index},                              % 允许的索引值
      {uint64, instance_id},                              % 实例Id
      {int, function_id},                                 % 功能Id
      {int, pre_status},
      {int, status},
      {point, pos}                                        % 玩家坐标
     ],

     [notify_open_all_light,                              % 开关所有的灯
      {int, turn_on}                                      % 0 表示关 1 表示开
     ],
     %--------------------物品,包裹相关-----------------------------------
     [notify_package,        % 通知玩家包裹的数据
      {string, account},     % 玩家帐号
      {int, type},           % 包裹类型
      {array, pack_grid, grid_vec}
     ],
     [req_swap_item,             % 请求包裹内部的物品交换
      {int, type},               % 包裹类型,参看package_type的定义
      {int, index1},             % 格子1的索引
      {int, index2}              % 格子2的索引
     ],
     [req_use_item,              % 请求使用物品
      {int, index}               % 包裹中的索引
     ],
     [req_move_item,             % 请求在包裹之间移动物品
      {int, pack1_type},         % 包裹1的类型
      {int, index1},             % 包裹1中的物品格子的索引
      {int, pack2_type},         % 包裹2的类型
      {int, index2}              % 包裹2中的物品格子的索引
     ],
     [req_delete_item,           % 请求删除物品
      {int, grid_index}          % 格子索引
     ],
     [req_split_item,            % 请求拆分物品
      {int, type},               % 包裹类型, 具体见package_type
      {int, src_gindex},         % 格子索引
      {int, target_gindex},      % 格子索引
      {int, count}               % 拆分出来的数量
     ],
     [req_resize_player_pack    % 请求对玩家包裹扩容
     ],
     [req_lock_bag_item,         % 请求锁定背包物品
      {uint64, item_inst_id}     % 物品实例ID
     ],
     [req_unlock_bag_item,       % 请求解除锁定背包物品
      {uint64, item_inst_id}     % 物品实例ID
     ],
     [req_move_money,            % 请求在包裹之间移动金钱
      {int, pack1_type},         % 包裹1的类型
      {int, pack2_type},         % 包裹2的类型
      {int, money}               % 包裹1中的物品格子的索引
     ],
     [req_switch_item,           % 请求在在游戏间交换物品
      {int, pack1_type},         % 包裹1的类型
      {uint64, instance_id},     % 物品实例ID(包裹1中的物品)
      {int, pack2_type}          % 包裹2的类型
     ],
     %----------------------系统消息相关----------------------------------
     [notify_sys_msg,            % 发送系统消息给客户端
      %%{int, type},               % 系统类型, 有时候不同的系统, 表现的形式会不一样,参见common_def.hrl
      {int, code},               % 系统消息代码, 参见sys_msg.hrl
      {array, string, params}    % 参数
     ],

     [notify_sys_broadcast,      % 系统公告, 参考sys_broadcast_type
      {uint64, id},         % 公告id
      {int, type},         % 公告类型
      {string, content},   % 播放内容 
      {int, play_times},   % 播放次数
      {int, priority},     % 优先级
      {int, show_seconds}, % 显示秒数
      {stime, start_time}  % 公告开始时间
     ],

     [req_fixed_broadcast,  % 请求固定的公告
      {int, type}           % 公告类型(系统自动显示公告或农场公告)
     ],

     [notify_del_broadcast,  % 通知删除公告
      {int, type},               % 公告类型
      {uint64, id}               % 公告id
     ],

     [req_gm_command,            % 请求使用GM指令
      {int, type},               % GM指令类型
      {array, string, params}    % 指令后面跟随的参数
     ],
     %----------------------NPC相关----------------------------------
     [notify_npc_list,                                    % 通知客户端npc列表
      {array, npc_map_mapping_info, npc_map_mapping_list} % npc与地图的对应表
     ],

     [notify_npc_close_dialog                             % 请求关闭对话框
     ],

     [notify_npc_show_dialog_by_option,                   % 通知客户端显示对话框(加选项)
      {int, template_id},                                 % 对话框内容
      {array, npc_command, command_list}                  % 对应对话框中指令信息
     ],

     [notify_npc_show_dialog_by_message,                  % 通知客户端显示对话框(只显示消息)
      {int, template_id},                                 % 对话框内容
      {uint64, unique_id},                                % 唯一标识
      {int, page_index},                                  % 对话框对应的页码
      {int, command_index}                                % 对应的指令信息
     ],

     [notify_npc_show_dialog_by_item,                     % 通知客户端显示对话框(显示物品)
      {int, template_id},                                 % 任务Id
      {array, npc_command, item_list}                     % 对话框对应的页码
     ],

     [req_npc_command,                                    % 请求npc指令
      {string, npc_key},                                  % npc主键
      {uint64, unique_id},                                % 唯一标识
      {int, page_index},                                  % 当前页码索引
      {int, command_index}                                % 当前命令索引
     ],
     %----------------------玩家相关----------------------------------
     [req_player_basic_data                               % 请求玩家基本资料
     ],

     [notify_player_basic_data,                           % 通知玩家基本资料
      {player_basic_data, basic_data},                    % 玩家基本信息
      {player_property, property}                         % 玩家属性
     ],

     [req_start_body_action,                              % 请求开始自身动作或二人动作
      {string, target_account},                           % 动作目标玩家(自身动作填空字串)
      {string, action}                                    % 动作名称
     ],

     [notify_start_body_action,                           % 通知开始自身动作或二人动作
      {string, player_account},                           % 玩家账号
      {string, target_account},                           % 动作目标玩家(自身动作填空字串)
      {string, action}                                    % 动作名称
     ],     

     [req_play_animation,                             % 请求播放动画
      {string, target_account},                           % 动作目标玩家(自身动作填空字串)
      {int, loop},                                        % 循环播放
      {string, ani},                                      % 动画名称
      {string, action}                                    % 动作名称
     ],

     [notify_play_animation,                           % 通知播放动画
      {string, player_account},                           % 玩家账号
      {string, target_account},                           % 动作目标玩家(自身动作填空字串)
      {int, loop},                                        % 循环播放
      {string, ani},                                      % 动画名称
      {string, action}                                    % 动作名称
     ],     

     [req_end_body_action                                 % 请求结束自身动作或二人动作      
     ],

     [notify_end_body_action,                             % 通知结束自身动作或二人动作
      {string, player_account},                           % 玩家账号
      {string, target_account}                            % 动作目标玩家(自身动作填空字串)
    ], 

     [req_sync_body_state,                                % 同步身体状态
      {int, body_state}                                   % 身体动作状态
     ],

     [notify_other_body_state,                            % 同步其它人身体状态
      {string, other_account},                            % 账号
      {int, body_state}                                   % 身体动作状态 
     ],

     [req_start_change_looks                              % 请求开始美容
     ],
     [notify_start_change_looks,                          % 通知开始
      {int, type}                                         % 费用类型(1为游戏金币，2为点卡点数) 
     ],
     [req_cancel_change_looks                             % 取消美容
     ],
     [req_end_change_looks,                               % 结束美容(改变个人形象)
      {int, new_hair},                                    % 新发型
      {int, new_hair_color},                              % 新发色
      {int, new_face},                                    % 新脸型
      {int, new_skin_color},                              % 新肤色
      {int, new_beard}                                    % 新胡子
     ],
     [notify_change_looks,                                % 通知容貌改变(自已，周围玩家都会收到)
      {string, account},                                  % 玩家账号
      {int, new_hair},                                    % 新发型
      {int, new_hair_color},                              % 新发色
      {int, new_face},                                    % 新脸型
      {int, new_skin_color},                              % 新肤色
      {int, new_beard}                                    % 新胡子
     ],
     [notify_end_change_looks                             % 通知结束美容(改变个人形象)
     ],
     %----------------------换装相关----------------------------------
     [notify_around_change_dress,    % 通知周边的玩家有玩家穿上服装
      {string, account},            % 指定的玩家穿上服装
      {int, pos},                   % 位置(衣服, 裤子, 鞋子等)
      {int, template_id}            % 指定的物品模板ID
     ],
     %----------------------邀请----------------------------------------
     [req_invite_someone,          % 请求邀请某人
      {string, invitee},           % 被邀请人的帐号
      {int, type}                  % 邀请的类型(交易, 交好友等)
     ],
     [notify_invitation,           % 通知被邀请人, 有人发送了一个邀请
      {string, invitor},           % 邀请人帐号
      {string, invitor_name},      % 邀请人姓名
      {int, type}                  % 邀请的类型(交易, 交好友等)
     ],
     [req_agree_invitation,        % 同意邀请
      {string, invitor},           % 邀请人帐号
      {int, type}                  % 邀请的类型(交易, 交好友等)
     ],
     [notify_agree_invitation,     % 同意邀请
      {string, agreer},            % 同意人帐号
      {int, type}                  % 邀请的类型(交易, 交好友等)
     ],
     [req_disagree_invitation,     % 不同意邀请
      {string, invitor},           % 邀请人帐号
      {int, type}                  % 邀请的类型(交易, 交好友等)
     ],
     [notify_cancel_invitation],   % 通知取消邀请
     %----------------------交易----------------------------------------
     [notify_start_trade,       % 通知开始交易
      {string, account}         % 对方账号
     ],
     [req_update_trade_money,        % 更新交易金额
      {int, money}                   % 金额
     ],
     [notify_update_trade_money, 
      {int, money}                 
     ],
     [notify_other_update_trade_money, 
      {int, money}                 
     ],
     [req_put_trade_item,           % 将物品放在交易栏
      {int, packet_index},          % 物品在包果中的索引 
      {int, trade_index}            % 物品在交易栏中的索引 
     ],
     [notify_put_trade_item,         
      {int, trade_index},          % 物品在交易栏中的索引 
      {item, item_data},           % 物品数据
      {int, item_count}            % 物品数量
     ],
     [notify_other_put_trade_item,         
      {int, trade_index},          % 物品在交易栏中的索引 
      {item, item_data},           % 物品数据
      {int, item_count}            % 物品数量
     ],
     [req_unput_trade_item,       % 将交易栏物品取下来
      {int, trade_index},         % 物品在交易栏中的索引
      {int, packet_index}         % 欲放到包果中的位置(-1表示无条件取下来)
     ],
     [notify_unput_trade_item,
      {int, trade_index}         % 物品在交易栏中的索引 
     ],
     [notify_other_unput_trade_item,
      {int, trade_index}         % 物品在交易栏中的索引 
     ],
     [req_swap_trade_item,       % 交换交易栏目中的物品
      {int, index1},
      {int, index2}
     ],
     [notify_swap_trade_item,    % 通知交易栏物品位置变了
      {int, index1},
      {int, index2}
     ],
     [notify_other_swap_trade_item,    % 通知交易栏物品位置变了
      {int, index1},
      {int, index2}
     ],
     [req_sure_trade            % 确定交易
     ],
     [notify_sure_trade         
     ],
     [notify_other_sure_trade   % 对方确定交易
     ],
     [req_cancel_trade          % 取消交易, 交易停止
     ],
     [notify_cancel_trade      
     ],
     [notify_trade_success      % 通知交易成功
     ],
     %----------------------商城----------------------------------------
     [furniture_goods_data,
      {int, goods_id},    % 商品ID
      {int, x},           % x坐标
      {int, z},           % z坐标
      {int, height},      % 离地高度
      {int, floor},       % 楼层
      {int, face},        % 朝向
      {int, room_id},     % 房间ID
      {int, type},        % 类型(地板，墙)
      {string, tex}       % 纹理
     ],
     [req_buy_sys_shop_goods,      % 请求购买系统商城的商品
      {int, goods_id}              % 商品ID
     ],
     [req_buy_sys_shop_goods_list, % 请求购买系统商城的商品(家装模式下的购买)
      {array, furniture_goods_data, goods_list}     % 商品ID列表
     ],
     [notify_buy_furniture_list,   % 通知客户端购买的家具列表(家装模式下的商城)
      {array, house_furniture, furnitures}     % 家具列表
     ],
     [player_coin,                 % 玩家货币
      {string, account},           % 玩家帐号
      {int, gamecoin},             % 游戏币
      {int, eqcoin}                % 点卡
     ],
     [notify_update_coin,          % 通知更新货币
      {player_coin, data}
     ],
     [req_buy_npc_shop_goods,      % 请求购买npc商城的商品
      {int, goods_id}              % 商品ID
     ],
     [req_sell_goods,              % 请求卖出物品给系统
      {int, grid_index}            % 玩家包裹的格子
     ],

     [notify_open_shop,            % 通知打开商店
      {int, type}
     ],
     %%--------------------农场-----------------------------------------
     [farm_pet_food,                                      % 玩家宠物
      {stime, period_date}                                % 有效期
     ],
     [farm_log_message,                                   % 消息日志
      {farm_log_message_type, log_type},                  % 日志类型
      {string, account},                                  % 帐号
      {stime, log_date},                                  % 时间
      {string, user_name},                                % 用户名
      {int, crop_id},                                     % 作物Id
      {int, crop_quantity},                               % 作物数量
      {int, pet_id},
      {int, bite_coin}
     ], 
     [farm_log_gain,                                      % 成果日志
      {int, crop_id},                                     % 作物Id
      {int, crop_quantity},                               % 作物数量
      {int, steal_quantity},                              % 偷取的数量
      {int, gain_quantity}                                % 收获的数量
     ],
     [farm_log_consumption,                               % 消费日志
      {farm_log_consumption_type, type},                  % 类型
      {stime, log_date},                                  % 时间
      {int, crop_id},                                     % 作物Id
      {int, crop_quantity},                               % 作物数量
      {int, use_coin},                                    % 花费金币
      {int, leave_coin}                                   % 剩余金币
     ],
     %% [farm_announcement,                                  % 公告
     %%  {int, id},                                          % 公告Id
     %%  {string, announcement}                              % 公告内容
     %% ],
     [farm_comment,
      {farm_comment_type, type},                          % 留言类型 
      {string, account},                                  % 留言帐号
      {stime, comment_date},                              % 留言时间
      {string, username},                                 % 留言用户
      {string, content}                                   % 留言内容
     ],
     [farm_decoration,                                    % 农场装饰
      {item, background},                                 % 背景
      {item, house},                                      % 房子
      {item, doghouse},                                   % 狗窝
      {item, dog},                                        % 狗
      {item, signpost},                                   % 路标
      {item, fence},                                      % 篱笆
      {item, scarecrow},                                  % 稻草人
      {item, billboard},                                  % 告示牌
      {item, billboard_content}                           % 告示牌内容
     ],
     [farm_crop_data,                                     % 农场的农作物
      {int, tempid},                                      % 庄稼模板ID
      {int, health},                                      % 健康度
      {array, int, curr_damage},                          % 庄稼当前受到的灾害(虫咬, 杂草, 已被收获)
      {stime, start_time},                                % 庄稼开始种植的时间
      {int, pick_num},                                    % 采摘量, 如果没有采摘行为, 则为0
      {int, remain},                                      % 剩余的数量
      {int, output},                                      % 产量
      {int, stage},                                       % 当前所处的阶段
      {int, land_type},                                   % 土地类型 1 -- 红土地, 0 -- 普通土地
      {int, picked},                                      % 采摘的状态, 0--可以采摘, 
                                                          %           1--已经采摘过,不能采摘
                                                          %           2--剩余量太低,不能采摘
      {array, int, stage_time},                           % 每个阶段需要的时间
      {int, aberrance_fruit}                              % 最终的果实ID
     ],

     [player_farm_data,                                   % 农场数据
      {string, account},                                  % 农场主人
      {int, exp},                                         % 农场经验 
      {farm_decoration, decoration},                      % 农场装饰
      {farm_pet_food, pet_food},                          % 宠物(狗)
      {array, farm_crop_data, crops},                     % 农作物
      {int, level}                                        % 农场等级
     ],
     [farm_exp_data,
      {int, plot_index},
      {int, add_exp}
     ],
     [farm_hint_data,
      {int, plot_index},
      {int, hint_id},
      {int, param1},
      {int, param2}
     ],
     [farm_pick_all_result,                               % 一键采摘结果数据
      {player_farm_data, farm_data},
      {array, farm_exp_data, exp_data},
      {array, farm_hint_data, hint}
     ],
     [player_farm_log,                                    % 农场日志
      {string, account},                                  % 农场主人
      {array, farm_log_message, message_log},             % 消息日志
      {array, farm_log_gain, gain_log},                   % 成果日志
      {array, farm_log_consumption, consumption_log}      % 消费日志
     ],
     [player_farm_comment,                                % 留言
      {string, account},                                  % 农场主人
      {array, farm_comment, comment}                      % 留言内容
     ],
     [farm_status,                 % 农场状态
      {string, account},           % 玩家帐号
      {string, name},              % 玩家名称
      {int, exp},                  % 农场经验
      {int, status}                % 农场状态
     ],
     [req_enter_farm,              % 请求进入场景
      {string, owner}              % 进入的场景的主人
     ],
     [notify_farm_data,            % 通知农场数据
      {player_farm_data, data}     % 农场数据
     ],
     [req_leave_farm               % 离开农场   
     ],
     [notify_leave_farm            % 离开家场
     ],
     [req_assart_plot              % 请求开垦农田
     ],            
     [req_sow,                     % 请求种植
      {int, plot_index},           % 农田索引
      {uint64, item_inst_id}       % 物品实例ID
     ],
     [req_pick_crop,               % 请求采摘农作物
      {string, account},           % 农场主
      {int, plot_index}            % 地块索引
     ],
     [req_pick_all_crops,          % 请求采摘农场中的所有农作物
      {string, account}            % 农场主
     ],
     [req_get_friend_farm_state   % 请求获得好友农场的状态
     ],
     [notify_friend_farm_status,    % 通知好友农场的状态
      {array, farm_status, data}    % 状态列表
     ],
     [req_weed,                    % 请求除草
      {string, account},           % 农场主
      {int, plot_index}            % 地块索引
     ],
     [req_worm,                    % 请求除虫
      {string, account},           % 农场主
      {int, plot_index}            % 地块索引
     ],
     [req_water,                   % 请求浇水
      {string, account},           % 农场主
      {int, plot_index}            % 地块索引
     ],
     [notify_pet_bite,             % 通知客户端, 被宠物咬了
      {int, hurt}                  % 伤害值
     ],
     [req_farm_log_list,                                  % 请求获取农场列表
      {farm_log_type, type},                              % 请求类型
      {string, account}
     ],
     [notify_farm_log_list,                               % 发送农场日志列表
      {player_farm_log, log}                              % 日志列表
     ],
     [req_clear_farm_log                                  % 请求清空农场日志
     ],
     [req_buy_farm_shop_goods,                            % 请求购买农场商店里的物品
      {buy_goods_type, type},                             % 购买类型
      {int, item_id},                                     % 农场商品的ID
      {int, count},                                       % 农场商品买入数量
      {pay_goods_type, pay_type}                          % 商品支付类型
     ],
     [req_sell_farm_shop_goods,                           % 请求卖出物品给农场商店
      {uint64, instance_id},                              % 农场仓库中物品实例ID
      {int, count}                                        % 卖出的数量
     ],
     [req_sell_farm_shop_all_goods,                       % 请求卖出所有的仓库物品
      {int, type}                                         % 种类
     ],
     [notify_farm_info,                                   % 通知农场信息
      {string, account},                                  % 帐号
      {int, plot_index},                                  % 地块的索引
      {int, exp},                                         % 农场经验
      {int, level},                                       % 农场等级
      {int, add_exp}                                      % 增加的经验值
     ],
     [req_farm_comment,                                   % 请求农场留言
      {string, account},                                  % 留言人
      {string, comment}                                   % 留言内容
     ],
     [notify_farm_comment,                                % 通知
      {farm_comment, comment}                             % 结果
     ],
     [req_reply_farm_comment,                             % 请求回复留言
      {string, account},                                  % 留言人
      {string, comment}                                   % 留言内容
     ],
     [notify_reply_farm_comment,
      {farm_comment, comment}
     ],
     [req_farm_comment_list,                              % 请求农场留言列表
      {string, account}
     ],
     [notify_farm_comment_list,                           % 发送农场留言
      {player_farm_comment, farm_comment}                 % 留言
     ],
     [req_clear_farm_comment                              % 请求清空农场留言
     ],
     [notify_clear_farm_comment,
      {comment_result, result}
     ],
     [req_remove_decoration,                              % 请求移除装饰
      {int, template_id}                                  % 模板Id
     ],
     [req_change_decoration,                              % 请求改变装饰
      {item, decoration_item}                             % 实例Id
     ],
     [req_put_grass,                                      % 请求放草
      {string, account},                                  % 帐号
      {int, plot_index}                                   % 地块索引
     ],
     [req_put_pest,                                       % 请求放虫
      {string, account},                                  % 帐号
      {int, plot_index}                                   % 地块索引
     ],
     [farm_setting,                                       % 农场设置
      {string, account},                                  % 玩家帐号
      {string, water},                                    % 浇水
      {string, weed},                                     % 除草
      {string, worm},                                     % 除虫
      {string, put_grass},                                % 放草
      {string, put_pest}                                  % 放虫
     ],
     [req_farm_setting                                    % 请求农场设置列表
     ],
     [req_set_farm_setting,                               % 请求农场设置
      {farm_setting, setting}                             % 设置
     ],
     [notify_set_farm_setting,                            % 返回农场设置结果
      {set_farm_setting_result, result}
     ],
     [req_reset_farm_setting                              % 重置农场设置
     ],
     [notify_reset_farm_setting,                          % 返回农场设置
      {farm_setting, setting}
     ],
     [notify_farm_setting,                                % 返回农场设置
      {farm_setting, setting}                             % 设置
     ],
     [req_lock_depot_item,                                % 请求锁定仓库物品
      {uint64, item_inst_id}                              % 物品实例ID
     ],
     [req_unlock_depot_item,                              % 请求解除锁定仓库物品
      {uint64, item_inst_id}                              % 物品实例ID
     ],
     %% [notify_total_price,                                 % 所有物品卖出的总售价
     %%  {uint64, data}                                      % 售价
     %% ],
     [req_hoeing,                                         % 请求锄地
      {int, plot_index}                                   % 地块索引
     ],
     [req_use_farm_item,
      {uint64, item_inst_id},                             % 物品实例ID
      {int, plot_index}                                   % 地块索引
     ],
     [req_can_assart_plot                                 % 请求服务端能否开垦土地
     ],
     [req_can_upgrade_land                                % 是否能升级到红土地
     ],
     [req_upgrade_land                                    % 升级到红土地
     ],
     [notify_farm_money,                                  % 通知红土地农场金币
      {string, account},                                  % 玩家
      {int, add_money},                                   % 增加金钱的额度
      {int, money}                                        % 金钱数
     ],
     [notify_farm_awards,                                 % 通知红土地农场金币
      {int, type},
      {int, money},
      {array, int, itemlist},                             % 增加金钱的额度
      {array, int, countlist}                             % 金钱数
     ],

     [notify_farm_eq_coin,
      {int, eq_coin}
     ],
     %%--------------------任务-----------------------------------------
     [req_task_list                                       % 请求任务列表
     ],

     [notify_task_list,                                   % 返回任务列表
      {array, player_task, task_list}                     
     ],

     [req_farm_task_list                                  % 请求农场任务列表
     ],

     [notify_farm_task_list,                              % 返回农场任务列表
      {array, player_task, task_list}
     ],
     
     [req_track_task,                                     % 请求跟踪任务
      {uint64, id}                                        % 任务Id
     ],

     [notify_track_task,                                  % 返回跟踪任务结果
      {uint64, id}
     ],

     [req_get_track_list                                  % 请求获取跟踪列表
     ],
     
     [notify_get_track_list,                              % 返回跟踪列表
      {array, player_task, task_list}
     ],

     [req_cancel_track_task,                              % 取消跟踪任务
      {uint64, id}                                        % 任务Id
     ],

     [notify_cancel_track_task,                           % 返回取消跟踪任务结果
      {uint64, id}
     ],
     
     [req_give_up_task,                                   % 请求放弃任务
      {uint64, id}                                        % 任务Id
     ],

     [notify_give_up_task,                                % 返回放弃任务结果
      {uint64, id}                       
     ],

     [notify_update_task_progress,                       % 更新任务进度   
      {uint64, id},
      {int, action_type},                                % 动作类型 
      {int, action_amount}                               % 动作数量
     ],

     [notify_task_complete,                               % 返回完成的任务
      {uint64, id}
     ],
     [notify_give_task,                                   % 通知客户端接了个任务
      {player_task, task}                                 % 任务
     ],
     [req_complete_task,                                  % 请求结束任务
      {int, task_id}
     ],

     [notify_give_farm_task,
      {player_task, task}
     ],

     [req_farm_complete_task,                             % 请求完成农场任务
      {int, task_id}
     ],

     [notify_farm_task_complete,                          % 通知农场完成任务
      {uint64, id}
     ],
     %% --------------------------邮件----------------------------------------
     [notify_mail_not_read                     %% 通知有邮件未看
     ],
     [notify_add_mail,                         %% 通知新邮件
      {mail_info, mail_data}
     ],
     [req_delete_mail,                         %% 删除邮件
      {uint64, id}                                %% 邮件ID 
     ],
     [notify_delete_mail,                      %% 通知删除邮件
      {uint64, id}                                %% 邮件ID 
     ],
     [req_get_mail_item,                       %% 取邮件物品
      {uint64, id},                               %% 邮件ID 
      {uint64, inst_id}                           %% 物品实例id
     ],
     [notify_delete_mail_item,                 %% 通知删除邮件物品
      {uint64, id},                               %% 邮件ID 
      {uint64, inst_id}                           %% 物品实例id
     ],
     [req_mail_list                            %% 请求邮件列表      
     ],
     [notify_mail_list,                        %% 发邮件列表
      {array, mail_info, mails}
     ],
     [req_mail_content,                        %% 阅读邮件内容
      {uint64, id}                                %% 邮件ID 
     ],
     [notify_mail_content,                     %% 通知邮件内容
      {uint64, id},                               %% 邮件ID 
      {string, content}                           %% 邮件内容
     ],
     [req_know_new_mail_title,                 %% 看过新邮件的标题(只发未看过的邮件)
      {array, uint64, id_list}
     ],
     [req_reject_mail,                         %% 退信
      {uint64, id}                                %% 邮件ID 
     ], 
     [req_send_mail,                           %% 请求发送邮件
      {string, recver_account},                   %% 接收者账号(客户端如果可以取得账号, 就传过来; 不知道账号填空字串)
      {string, recver_name},                      %% 接收者名字
      {string, title},                            %% 标题      
      {string, content},                          %% 内容      
      {array, uint64, items}                      %% 物品列表(物品实例id列表)   
     ],
     [notify_send_mail_success                 %% 通知发送邮件成功
     ],
     [notify_reject_mail_success,              %% 通知退邮件成功(客户收到此消息，删除信件)
      {uint64, id}                                %% 邮件ID 
     ],
     %% --------------------------界面----------------------------------------
     [req_open_ui,                         %% 请求打开界面
      {int, type}                          %% 界面类型
     ],
     [notify_open_ui,                      %% 通知打开界面
      {int, type}
     ],
     %% --------------------------系统时间--------------------------------------
     [req_sys_time                         %% 请求系统时间
     ],

     [notify_day_or_night,                 %% 通知白天或黑夜
      {day_or_night_flag, flag}            %% 
     ],
     
     [notify_sys_time,                     %% 返回系统时间
      {stime, sys_time}
     ],

     [notify_wallow_time,                     %% 返回自已防沉迷上线时间
      {int, wallow_seconds}
     ],
     %% --------------------------疾病系统--------------------------------------
     [notify_player_health_status,         %% 通知玩家健康状况
      {string, account},                   %% 玩家
      {disease_type, status}               %% 玩家健康状况
     ],
     [notify_disease_special_event,        %% 通知玩家疾病特殊事件
      {int, special_event_id}              %% 特殊事件Id
     ],
     %% --------------------------房屋买卖--------------------------------------
     [req_start_buy_house                  %% 开始购买房屋
     ],
     
     [notify_start_buy_house           
     ],

     [req_end_buy_house                    %% 结束购买房屋  
     ],
     
     [notify_end_buy_house         
     ],
     
     [req_buy_house,                       %% 请求购买房屋
      {int, id}                            %% 购买房屋id
     ],
     [notify_buy_house,                    %% 购买房屋结果
      {buy_house_result, result}
     ],

     [req_buy_house_replace,
      {buy_house_replace_type, type},
      {int, id}
     ],
     [req_buy_house_add,
      {int, id}
     ],
     
     [notify_buy_new_house_success         %% 玩家成攻购买了新房子
     ],
     %% --------------------------玩家仓库--------------------------------------
     [req_deposit_money_in_depot,          %% 存钱到仓库
      {int, money}
     ],
     [req_withdraw_money_in_depot,         %% 从仓库取钱到包裹
      {int, money}
     ], 
     [notify_player_depot,                 %% 玩家仓库的数据
      {int, money},                        %% 游戏币
      {array, pack_grid, grid_vec}         %% 包裹
     ],
     %% --------------------------家政--------------------------------------
     [req_start_domestic                  %% 请求家政开始
     ],
     [notify_start_domestic               %% 家政开始
     ],
     [req_domestic_service,               %% 请求家政服务  
      {int, id}                           %% 家政服务id
     ],
     [notify_domestic_result,
      {int, id},                          %% 家政服务id
      {int, change_house_clean},                        % 家具清洁
      {int, cost_money},                  %% 消耗金钱 
      {int, money_rate}                   %% 打折率(单位: 百分之一), 100 表示没打折 
     ],
     [req_end_domestic,                   %% 家政结束
      {int, id}                          
     ],
     [notify_end_domestic,                %% 通知家政结束
      {int, id}                          
     ],
     %% --------------------------宝箱系统, 垃圾系统-----------------------------------
     [req_walk_for_pick_magic_box,  % 行走计划(捡宝箱）
      {point, curr_pos},  % 当前位置
      {point, dest_pos},  % 目标位置
      {uint64, box_id},   % 宝箱实例id 
      {int, move_type}    % 移动类型
     ],

     [req_pick_magic_box,                  % 拾取宝箱
      {uint64, box_id}                      % 实例id
     ],

     [req_client_empty_box_positions,      % 请求客户端空位置列表
      {int, need_n}                         % 需要几个位置
     ],

     [notify_server_empty_box_positions,   % 通知服务端空位置列表
      {array, grid_pos, pos_list}              % 位置列表
     ],

     [notify_house_magic_box,              % 通知房屋宝箱
      {array, magic_box, box_list}          % 宝箱列表 
     ],

     [notify_del_house_magic_box,           % 删除
      {uint64, box_id}                      % 实例id      
     ],

     [notify_add_house_magic_box,           % 新增
      {array, magic_box, box_list}          % 宝箱列表 
     ],

     [notify_pick_magic_box_gain,          % 通知捡宝箱收获
      {magic_box_type, type},                % 类型(宝箱或垃圾)
      {int, item_id},                        % 物品
      {int, money}                           % 金钱
     ],

     [notify_friend_garbage,              % 通知垃圾信息
      {string, friend_account},            % 好友账号
      {int, n}                             % 垃圾个数(为0时，客户端不显示垃圾图标)
     ],
     %% --------------------------礼物系统-----------------------------------
     %% --礼物盒--
     [req_start_gift_service,                        % 请求开始礼物服务
      {gift_service_type, type}
     ],

     [notify_start_gift_service,                     % 开始礼物服务
      {gift_service_type, type}
     ],

     [req_end_gift_service                           % 请求结束礼物服务
     ],

     [notify_end_gift_service,                       % 通知结束礼物服务
      {gift_service_type, type}
     ],

     [req_pack_gift_box,                                 % 请求礼物盒
      {int, pack1_type},                                 % 包裹1的类型
      {int, index1},                                     % 包裹1中的物品格子的索引
      {int, pack2_type},                                 % 包裹2的类型
      {int, index2},                                     % 包裹2中的物品格子的索引
      {gift_box, gift}
     ],

     [notify_pack_gift_box,
      {gift_box, gift}
     ],

     [req_unpack_gift_box,                               % 请求取消礼物盒
      {uint64, inst_id}                                  % 礼物盒实例Id
     ],

     [notify_unpack_gift_box,
      {gift_box, gift}
     ],

     [req_put_gift_box,                                  % 请求放置礼物盒
      {uint64, inst_id},                                 % 礼物盒实例Id
      {grid_pos, pos}                                    % 放置的礼物位置
     ],

     [notify_put_gift_box
     ],

     [req_pickup_gift_box,                               % 请求捡起礼物盒
      {uint64, inst_id}                                  % 礼物盒实例Id
     ],

     [notify_pickup_gift_box,
      {uint64, inst_id}
     ],

     [notify_gift_box,
      {gift_box, gift}
     ],

     [req_get_send_box_vec,
      {int, page_index},
      {int, page_count}
     ],

     [notify_get_send_box_vec,
      {array, gift_box, send_box_vec},
      {int, total}
     ],

     [req_get_receive_box_vec,
      {int, page_index},
      {int, page_count}
     ],
     
     [notify_get_receive_box_vec,
      {array, gift_box, receive_box_vec},
      {int, total}
     ],

     [req_get_viewed_box_vec,
      {int, page_index},
      {int, page_count}
     ],

     [notify_get_viewed_box_vec,
      {array, gift_box, viewed_box_vec},
      {int, total}
     ],

     %% --礼物快递--
     [gift_express,                                      % 礼物快递
      {gift_express_type, type},
      {uint64, inst_id},                                 % 快递实例Id
      {string, sender},                                  % 发送者
      {string, sender_name},                             % 发送者名称
      {string, receiver},                                % 接收者
      {string, receiver_name},                           % 接收者名称
      {array, pack_grid, grid_vec},                      % 包裹
      {int, card_id},                                    % 礼物卡
      {string, comment},                                 % 赠言
      {stime, send_date},                                % 发送日期
      {stime, date}                                      % 记录日期
     ],
     
     [req_send_gift_express,                             % 请求礼物快递
      {int, pack1_type},                                 % 包裹1的类型
      {int, index1},                                     % 包裹1中的物品格子的索引
      {int, pack2_type},                                 % 包裹2的类型
      {int, index2},                                     % 包裹2中的物品格子的索引
      {gift_express, gift}
     ],

     [notify_send_gift_express,
      {uint64, inst_id}
     ],

     [req_receive_gift_express,
      {uint64, inst_id},
      {int, page_index},
      {int, page_count}
     ],

     [notify_receive_gift_express,
      {uint64, inst_id}
     ],

     [req_get_send_express_vec,
      {int, page_index},
      {int, page_count}
     ], 
     
     [notify_get_send_express_vec,
      {array, gift_express, send_express_vec},
      {int, total}
     ],

     [req_get_receive_express_vec,
      {int, page_index},
      {int, page_count}
     ],

     [notify_express_vec,                            %% 通知快递礼物 
      {array, gift_express, send_express_vec},       %% 注册的快递礼物
      {array, gift_express, receive_express_vec}     %% 发送的快递礼物
     ],

     [notify_send_express,                           %% 通知发送的快递礼物
      {gift_express, send_express}                   %% 快递礼物
     ],

     [req_send_express_by_subscribe,                 %% 请求发送注册的快递礼物
      {uint64, inst_id}                              %% 快递实例Id
     ],
     
     [notify_get_receive_express_vec,
      {array, gift_express, receive_express_vec},
      {int, total}
     ],

     [req_get_viewed_express_vec,
      {int, page_index},
      {int, page_count}
     ],

     [notify_get_viewed_express_vec,
      {array, gift_express, viewed_express_vec},
      {int, total}
     ],

     [req_clean_gift_log,
      {gift_log_type, type}
      ],

     %% --------------------------秀图--------------------------------------
     [notify_show_picture,
      {int, id}
     ],

     %% --------------------------打工--------------------------------------
     [req_start_work                     %% 请求打工开始
     ],
     [notify_start_work                  %% 打工开始
     ],
     [req_work,                          %% 请求打工
      {int, id}                          
     ],
     [notify_work_result,                %% 通知打工结果
      {int, id},                         %%
      {int, change_clean},                              % 人物清洁
      {int, change_health},                             % 人物健康
      {int, change_charm},                              % 人物魅力
      {int, change_active},                             % 活跃值
      {int, work_money},                 %% 打工赚多少钱
      {int, event_id},                   %% 触发事件id
      {int, reward_money},               %% 触发奖励金钱
      {int, reward_item}                 %% 触发奖励物品
     ],
     [req_end_work,                      %% 打工结束
      {int, id}                          
     ],
     [notify_end_work,                   %% 通知打工结束
      {int, id}                          
     ],
     %% -----------------------------微博相关-------------------------------------
     [req_get_weibo_access_secret        %% 请求获得微博的access secret
     ],
     [notify_weibo_access_secret ,       %% 通知客户端获取access secret
      {string, tx_access_key},
      {string, tx_access_secret},
      {string, sina_access_key},
      {string, sina_access_secret}
     ],
     [req_save_access_secret,            %% 请求保存微博的access key secret
      {string, tx_access_key},
      {string, tx_access_secret},
      {string, sina_access_key},
      {string, sina_access_secret}
     ],
     %-----------------------房屋预览----------------------------
     [req_preview_house,              % 请求预览房屋
      {string, account},              % 预览的房屋主人
      {uint64, inst_id}
     ],

     [notify_preview_house,           % 通知预览房屋结果
      {house_data, house}
     ],

     [req_preview_shop_house,         %% 请求获取房屋数据
      {int, house_id}
     ],

     [req_end_preview_house           % 重新看到当前场景(退出预览)
     ],

     [notify_end_preview_house        % 通知退出预览
     ],
     %-----------------------漂流瓶----------------------------
     [bottle_info,           % 漂流瓶信息 
      {uint64, id},          % id
      {string, account},     % 玩家
      {bottle_type, type},   % 瓶子类型
      {string, msg},         % 内容
      {stime, time}          % 时间      
     ],

     [req_del_bottle_log,    % 删除信息
      {uint64, id}           
      ],

     [req_drop_bottle,       % 请求丢瓶子
      {string, msg},         % 内容
      {bottle_type, type},   % 类型
      {sex_type, sex},       % 性别条件 (1:男, 2:女) 
      {int, account_visible} % 是否显示账号(0:不显示, 1:显示)
     ],

     [notify_drop_bottle,   % 通知丢瓶成功
      {bottle_info, bottle} 
     ],

     [req_pick_bottle        % 请求捞漂流瓶 
     ],

     [notify_pick_bottle,    % 通知捞到飘流瓶
      {bottle_info, bottle} 
     ],

     [req_all_bottle_info    % 请求自已的 飘流瓶相关信息
     ],     

     [notify_bottles_info,   % 
      {array, bottle_info, pick_bottles},  % 捞到的瓶子信息
      {array, bottle_info, drop_bottles}   % 丢出的瓶子信息
     ],

     [notify_today_bottle_info,    % 玩家当天的飘流瓶相关信息
      {int, year},           % 当天日期(因存在跨天情况，所以需要知道时间)
      {int, month},          % 
      {int, day},            % 
      {int, pick_count},     % 打捞次数
      {int, drop_count}      % 丢瓶次数
     ],

     [notify_open_float_bottle_pool    % 通知打开飘流池操作界面
     ],
     %% ----------------------------家装值---------------------------------------
     [notify_house_decoration, % 通知家装值    
      {string, account},       % 账号
      {int, decoration}
     ],
     %% ----------------------------鲜花和鸡蛋---------------------------------------
     [notify_new_day_reset             % 通知新的一天重置(鲜花记录清空，鸡蛋记录清空, 飘流瓶记录清空)
      ],

     [notify_total_flower_count,         % 通知鲜花总数    
      {string, account},                 % 账号
      {int, flower_count}
     ],

     [req_send_flower                 % 请求送鲜花
     ],

     [notify_send_flower_success,       % 通知送鲜花成功
      {player_info, player},            % 玩家的基本信息
      {array, pack_grid, body}          % 装备信息 
     ],

     [req_flower_logs               % 请求当天鲜花记录
     ],

     [notify_flower_logs,           % 通知当天鲜花记录
      {array, flower_log, logs}
     ],

     [notify_house_flower_change,    % 通知当前房间, 当天的鲜花数( 玩家进屋后会通知, 有改变也会通知 )
      {string, house_owner},         % 屋主
      {int, flower_count}          
     ],

     [req_send_egg                 % 请求送鸡蛋
     ],

     [notify_send_egg_success,     % 通知送鸡蛋成功
      {player_info, player},            % 玩家的基本信息
      {array, pack_grid, body}          % 装备信息 
     ],

     [req_egg_logs                 % 请求当天鸡蛋记录
     ],

     [notify_egg_logs,             % 通知当天鸡蛋记录
      {array, egg_log, logs}
     ],
     
     [notify_today_flower_egg_count,      % 通知鲜花鸡蛋信息
      {string, house_owner},        % 屋主
      {int, egg_count},
      {int, flower_count}
     ],
    
     [notify_house_egg_change,      % 通知当前房间, 当天的鸡蛋数( 玩家进屋后会通知, 有改变也会通知 )
      {string, house_owner},        % 屋主
      {int, egg_count}
     ],
     %% --------------------------------------------------------------------------
     [req_is_active_game,               %% 是否激活了相关的游戏
      {int, type}                       %% 小游戏类型
     ],
     [notify_active_game_result,        %% 是否激活了相关的游戏
      {int, result}                     %% 结果, 0 -- 没激活, 1 -- 激活
     ],
     [req_common_scene_state,           %% 具体场景的状态
      {int, teleport_id}                   %% 场景ID
     ],
     [scene_state,                      %% 场景状态
      {int, scene_id},                  %% 场景ID
      {int, copy_id},                   %% 副本ID
      {int, count},                     %% 场景中的人数
      {string, copy_name},              %% 副本名字
      {int, max_players},               %% 允许的最大玩家数
      {int, busy_players},              %% 繁忙状态玩家数
      {int, loose_players},             %% 宽松状态玩家数
      {int, status}                     %% 线路状态 0-闲 1-普通 2-忙 3-满 
     ],
     [notify_common_scene_state,        %% 具体场景的状态
      {array,scene_state,scene_list}
     ],
     %% --------------------------------丘比特之箭----------------------------------------%%
     [req_cupid_participate,            %% 报名
      {string, account},
      {int, sex},
      {array, string, tags}
     ],
     [req_cupid_cancel_participate,     %% 取消报名
      {string, account}
     ],
     [notify_cupid_participate,
      {int, result}            %%0-重复报名，1-报名成功
     ],
     [notify_cupid_cancel_participate,     %% 取消报名
      {int, result}              %%0-取消失败 1-取消成功
     ],
     [notify_cupid_girl_match_result,
      {array, string, tags}
     ],
     [notify_cupid_boy_match_result,
      {array, string, tags},
      {int, girl_result}       %%女生的选择 1-男生房间，2-女生房间
     ],
     [req_cupid_girl_confirm,  %%女生回复
      {string, account},      
      {int, result}            %%0-不愿意，1-男生房间，2-女生房间
     ],
     [req_cupid_boy_confirm,   %%男生回复
      {string, account},      
      {int, result},           %%0-不愿意，1-愿意
      {int, girl_result}       %%女生的选择 1-男生房间，2-女生房间
     ],

     %%--------------------------派对--------------------------------------
     [req_create_party,        %% 请求创建派对
	{int, type},         %% 派对类型
	{string, title},       %% 派对标题
	{string, description}  %%派对描述
     ],

     %% 通知派对创建结果
     [notify_create_party_result,
      {int, result} %% 结果, 1-- 成功, 0 --失败
     ],

     %% 请求修改派对信息
     [req_edit_party,
      {int, type}, %% 派对类型
      {string, title}, %% 派对标题
      {string, description} %%派对描述
     ],

     %% 通知修改派对信息的结果
     [notify_edit_party_result,
      {int, result} %% 结果, 1-- 成功, 0 --失败
     ],

     %% 请求删除派对
     [req_delete_party
     ],

     %% 请求获得派对列表
     [req_get_party_list
     ],

     %% 派对数据
     [party_data,
      {string, create_account}, %% 创建该派对的帐号
      {int,  type}, %% 派对类型
      {string, title}, %% 派对的标题
      {string, desc} %% 派对的内容(详细描述)
     ],

     %% 通知派对列表结果
     [notify_party_list,
      {array,  party_data, partys} %% 派对数据
     ]
    ].

% 获得枚举类型的定义
get_enum_def() ->
    [
     {ver_validate_result, [ver_ok, ver_fail]}, % 版本验证结果
     {login_result, [lr_ok, lr_fail, lr_fail_pwd, lr_fail_account, lr_fail_other]},          % 登录结果
     {create_role_result, [rr_ok, rr_fail]},    % 创建角色结果
     {enter_scene_result, [sr_ok, sr_fail]},    % 登录场景结果
     {package_type, % 包裹类型
      [pt_bag,      % 玩家背包
       pt_body,     % 玩家装备
       pt_farmdepot,% 农场仓库
       pt_depot,    % 玩家仓库
       pt_gift_bag  % 礼物包裹
      ]}, 
     {gm_command_type, [gct_add_item,  % 增加物品
			gct_get_money, % 获得金钱
			gct_replace_tex, % 换墙纸，地板纹理
			gct_add_farm_item, % 获得农场仓库物品
			gct_send_sys_mail,  % 发送系统邮件
			gct_send_n_mail,  % 发送n封邮件给自已
			gct_send_gm_mail  % 发送GM邮件
		       ]
     },
     {invitation_type, [ivt_trade,       % 邀请交易
			ivt_enter_house, % 邀请进房间
			ivt_make_friend, % 邀请加为好友
			ivt_kiss,        % 邀请亲吻
			ivt_hug,         % 邀请拥抱
			ivt_shake_hands  % 邀请握手 
		       ]
     },
     {mail_read_status,        % 邮件状态
      [
       mrs_not_read,               % 没看过
       mrs_read_title,             % 看过标题
       mrs_read_content            % 看过内容
      ]
     },
     {mail_type,          % 邮件类型
      [
       mt_person_mail,     % 个人邮件
       mt_gm_mail,         % GM邮件
       mt_system_mail,     % 系统邮件
       mt_reject_mail      % 退信  (因系统邮件不存在退信状况，故将退信放在邮件类型)
      ]},
     {sys_broadcast_type,  % 系统公告类型
      [
       sbt_sys_show,       %1、	系统自动显示播放的公告
       sbt_at_time,        %2、	系统定点发送的公告
       sbt_player_behavior,%3、	玩家行为引起的服务器公告
       sbt_gm,             %4、	对于GM或者其他的官方管理人员进行临时发出的公告
       sbt_farm,           %5、	农场的公告
       sbt_per_day         %6、	每天公告
      ]},
     {track_task_result, [ttr_ok, ttr_fail]},                      % 跟踪任务结果
     {cancel_track_task_result, [cttr_ok, cttr_fail]},             % 取消跟踪任务结果
     {task_give_up_result, [tgur_ok, tgur_fail]},                  % 放弃任务结果
     {farm_state, [fms_gain,    % 收获
		   fms_pest,    % 虫害
		   fms_grass,   % 长草
		   fms_drought, % 旱灾
		   fms_normal   % 正常状态
		  ]
     },
     {farm_crop_stage,     % 农作物的阶段
      [fcs_sprouting,    % 发芽阶段
       fcs_s_leafing,    % 小叶子阶段
       fcs_b_leafing,    % 大叶子阶段
       fcs_blooming,     % 开花阶段
       fcs_fruiting,     % 结果阶段
       fcs_picking,      % 采摘阶段
       fcs_die           % 枯死阶段
      ]
     },
     {ui_type, [uit_sysshop,   % 系统商城界面
		uit_npcshop,   % npc商店
		uit_farmdepot, % 农场仓库
		uit_depot,     % 玩家仓库
		uit_bag,       % 玩家包裹
		uit_housetransaction % 房屋买卖
	       ] 
     },
     %% 小游戏类型
     {sg_type, [sgt_farm       % 农场
	       ] 
     },
     {set_farm_setting_result, [sfsr_ok, sfsr_fail]},
     {comment_result, [cr_ok, cr_fail]},    % 留言结果
     {farm_log_type, [flt_help, flt_gain, flt_consumption]}, %% 农场日志类型
     {farm_log_message_type, [flmt_help, flmt_demage, flmt_extract, flmt_bite]},  %% 农场日志的消息类型
     {farm_log_consumption_type, [flct_buy, flct_sell]},  %% 农场日志的消费类型
     {farm_comment_type, [fct_owner, fct_guest, fct_reply]},  %% 农场留言
     {disease_type, [dt_normal, dt_white, dt_yellow, dt_purple, dt_red]},  %% 疾病类型
     {magic_box_type, [mbt_magic_box, mbt_garbage]},
     {buy_house_result, [bhr_replace, bhr_add]},
     {set_house_welcome_words_result, [shwwr_ok, shwwr_fail]},
     {set_house_permission_result, [shpr_ok, shpr_fail]},
     {house_permission_type, [hpt_none, hpt_friend]},
     {furniture_permission_type, [fpt_yes, fpt_no]},
     {house_visit_type, [hvt_visit, hvt_leave, hvt_pick_garbage]},
     {clear_house_log_result, [chlr_ok, chlr_fail]},
     {accept_invite_type, [accept_invite_yes, accept_invite_no]},
     {anonymous_type, [at_yes, at_no]},
     {gift_service_type, [gst_normal, gst_pack, gst_pickup, gst_put, gst_express, gst_reply_express]},
     {gift_log_type, [glt_send_box, glt_receive_box, glt_viewed_box, glt_send_express, glt_receive_express, glt_viewed_express ]},
     {buy_goods_type, [bgt_buy, bgt_use]},
     {pay_goods_type, [pgt_game_gold, pgt_eq_gold]},
     {buy_house_replace_type, [bhrt_yes, bhrt_no]},
     {task_type, [tt_yes, tt_no]},
     {sex_type, [st_boy, st_girl]},
     {gift_express_type, [gm, player]},
     {bottle_type, [bt_common,       %% 普通瓶
		    bt_make_friend   %% 交往瓶
		   ]},
     {day_or_night_flag, [dn_day, dn_night]},
     {task_action_type, [
			tat_visit_friend,	% 玩家访问三位好友
			tat_work,		% 玩家完成一次打工
			tat_wash_hands,		% 玩家洗过三次手
			tat_pick_garbage,	% 玩家拾取三个垃圾
			tat_change_hair,	% 玩家更换过一次发型
			tat_drop_float_bottle,	% 玩家扔过一次漂流瓶
			tat_pick_float_bottle,  % 玩家捞起过三次漂亮瓶
			tat_send_egg,		% 玩家扔过一次鸡蛋
			tat_send_flower,	% 玩家送过一次鲜花
			tat_domestic,		% 玩家使用过一次家政
			tat_bash,		% 玩家洗过一次澡
			tat_buy_shoes,		% 玩家购买一双鞋子
			tat_buy_clothes,	% 玩家购买一件衣服
			tat_buy_desk,		% 玩家购买一张桌子
			tat_buy_chair,		% 玩家购买一把椅子
			tat_buy_storage,	% 玩家购买一个柜子
			tat_buy_electric,	% 玩家购买一件电器
			tat_buy_hat,		% 玩家购买一顶帽子
			tat_buy_decoration	% 玩家购买一件装饰品
			]},
     {task_kind, [tk_newer,     % 新手任务 
		  tk_daily,     % 每日任务
		  tk_once,      % 一次性任务
		  tk_rate_once  % 带机率的一次性任务
		 ]},                   
     {gm_level, [gm_enable,gm_disable]}
    ].


get_version() ->
    290.
