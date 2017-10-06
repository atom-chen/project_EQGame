%% ---------- 标记 --------------------------------------
-define(action_allow_mark, 1). % 允许标记
-define(action_stop_mark, 2).  % 强制停止标记
-define(action_error_mark, 3). % 错误标记

%% ---------- 动作列表 --------------------------------------
-define(action_type_walk, 1).         % 行走
-define(action_type_edit_house, 2).   % 编辑房屋
-define(action_type_change_scene, 3). % 场景切换
-define(action_type_use_furniture, 4).% 使用家具
-define(action_type_invite, 5).       % 发起邀请
-define(action_type_be_invite, 6).    % 被邀请
-define(action_type_trade, 7).        % 交易
-define(action_type_work, 8).         % 打工
-define(action_type_domestic, 9).         % 家政
-define(action_type_change_looks, 10).    % 美容
-define(action_type_farm, 11).            % 在农场
-define(action_type_body_action, 12).     % 玩家动作表情
-define(action_type_gift, 13).            % 礼物服务
-define(action_type_house_exist_gift, 14). % 房屋存在礼物
-define(action_type_buy_house, 15).        % 购买房屋
-define(action_type_be_kick, 16).          % 被踢
-define(action_type_pick_magic_box, 17).   % 拾取宝箱，垃圾
-define(action_type_change_house, 18).     % 切换房屋
-define(action_type_preview_house, 19).    % 预览房屋
-define(action_type_wear, 20).             % 穿衣服
