%%% @author linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%% 程序中通用的类型定义
%%% @end
%%% Created : 19 Mar 2010 by  <>

%%----系统类型定义-------------------------
-define(st_player, 1).
-define(st_item, 2). % 物品系统
-define(st_pack, 3). % 包裹系统
-define(st_house, 4).  % 房屋
-define(st_furniture, 5). % 房屋家具
-define(st_friend, 6). % 房屋家具
-define(st_npc, 7).    % npc
-define(st_invitation, 8). % 邀请系统
-define(st_action, 9).     % 动作系统 
-define(st_shop, 10).  % 商城, 商店
-define(st_task, 11).  % 任务
-define(st_trade, 12). % 交易
-define(st_farm, 13).  % 农场
-define(st_mail, 14).  % 邮件
-define(st_house_transaction, 15).  %% 房屋交易
-define(st_depot, 16). %% 玩家仓库
-define(st_farm_ui, 17).  % 农场界面相关操作
-define(st_gift_box, 18). % 礼物盒
-define(st_gift_express, 19).  % 礼物快递
-define(st_sys_boardcast, 20). % 系统公告
-define(st_house_plot, 21).    % 房屋地块
-define(st_magic_box, 22).     % 宝箱
-define(st_garbage, 23).       % 垃圾
-define(st_float_bottle, 24).  % 飘流瓶
-define(st_item_money_log, 25).  % 物品金钱记录


%%----性别定义-----------------------------
-define(ps_male, 1).   %% 男性
-define(ps_female, 2). %% 女性

-define(init_pack_grid_count, 30). %% 初始包裹格子的数量
-define(init_gift_grid_count, 1).  %% 礼物包裹格子的数量

%%----货币类型的定义-----------------------
-define(GAMECOIN, 1).  %% 游戏币
-define(EQCOIN, 2).    %% EQ币, 点卡

-define(MAX_GAMECOIN, 1000000000). %% 最大游戏币 
-define(MAX_EQCOIN, 1000000000).   %% 最大EQ币

%%----玩家任务状态定义--------------------
-define(ACTIVE, 1).    %% 正在进行的任务
-define(COMPLETE, 2).  %% 已经完成的任务
-define(GIVEUP, 3).    %% 已经放弃的任务

%%----好友--------------------------
%% -define(MAX_FRIENDS, 200).    %% 最大好友数量
%% -define(PAGE_FRIEND_COUNT, 10).    %% 每页好友数量

%%----交易相关--------------------------
%% -define(MAX_TRADE_DISTANCE, 5 * 80). %% 交易最大距离5格
%% -define(MAX_TRADE_DISTANCE2, ?MAX_TRADE_DISTANCE * ?MAX_TRADE_DISTANCE). %% 交易最大距离的平方
%% -define(TRADE_GRID_COUNT, 9).       %% 交易栏格子个数
%% -define(POST_TRADE_SECOND, 3). %% 双方都确定交易，等待3秒升效

-define(GRID_SIZE, 80).        %% 每格表示80厘米

-define(ITEM_CAN_TRADE, 3).    %% 物品可交易

%%----时间相关-------------------------
-define(dt_onlinetime, 1).  %% 根据在线时长订阅
-define(dt_datetime, 2).    %% 根据固定时间订阅
-define(dt_interval, 3).    %% 根据间隔时间订阅

%%----物品时效-------------------------
%% -define(ITEM_UPDATE_INTERVAL, 3600). %% 物品更新间隔(秒)
%% -define(ITEM_MIN_TRADE_SECOND, 10 * 24 * 3600). %% 物品最小交易时间

%%----邮件相关-------------------------
%% -define(MAX_MAIL_ITEM_COUNT, 6). %% 邮件物品最大个数
%% -define(MAX_MAIL_TITLE_LENGTH, 24). %% 标题最大字符数不包含re:
%% -define(MAX_MAIL_CONTENT_LENGTH, 1024). %% 邮件内容最大字符数
%%-define(MAX_MAIL_DAY, 30).              %% 邮件存在最大天数

%%----人物属性-------------------------
-define(PLAYERCLEAN, 1).
-define(PLAYERHEALTH, 2).
-define(PLAYERCHARM, 3).
-define(ACTIVEVALUE, 4).
-define(HOUSECLEAN, 5).


-define(FALSE, 0).
-define(TRUE, 1).

