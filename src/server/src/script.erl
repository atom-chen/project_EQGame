%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2010, linyibin
%%% @doc
%%% �ű�API
%%% @end
%%% Created : 20 Dec 2010 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(script).

-include("packet_def.hrl").
-include("sys_msg.hrl").
-include("common_def.hrl").
-include("enum_def.hrl").

%% API
-compile(export_all).
%%%===================================================================
%%% API
%%%===================================================================
promot(Content, State) ->
    Account = player_data:get_account(State),
    sys_msg:send(Account, ?msg_npc_script, Content).

%%-------------------------------���------------------------------------
%% ��ȡ��ҽ��
get_game_coin(State) ->
    player_coin:get_coin(?GAMECOIN, State).
get_eq_coin(State) ->
    player_coin:get_coin(?EQCOIN, State).

%% ������ҽ��
add_game_coin(Count, State) ->
    NewState = player_coin:add_coin(?GAMECOIN, Count, State),
    %% ����, ��Ʒ��log��¼
    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, Count),
    Account = player_data:get_account(State),
    item_money_log:log_money(Account, script_add_money, MoneyInfo),
    NewState.
add_eq_coin(Count, State) ->
    NewState = player_coin:add_coin(?EQCOIN, Count, State),
    %% ����, ��Ʒ��log��¼
    MoneyInfo = item_money_log:get_money_info(?EQCOIN, Count),
    Account = player_data:get_account(State),
    item_money_log:log_money(Account, script_add_money, MoneyInfo),
    NewState.

%% ������ҽ��
dec_game_coin(Count, State) ->
    NewState = player_coin:dec_coin(?GAMECOIN, Count, State),
    %% ����, ��Ʒ��log��¼
    MoneyInfo = item_money_log:get_money_info(?GAMECOIN, -Count),
    Account = player_data:get_account(State),
    item_money_log:log_money(Account, script_dec_money, MoneyInfo),
    NewState.
dec_eq_coin(Count, State) ->
    NewState = player_coin:dec_coin(?EQCOIN, Count, State),    
    %% ����, ��Ʒ��log��¼
    MoneyInfo = item_money_log:get_money_info(?EQCOIN, -Count),
    Account = player_data:get_account(State),
    item_money_log:log_money(Account, script_dec_money, MoneyInfo),
    NewState.
%%----------------------------------------------------------------------
add_item(TempId, State) ->
    Account = player_data:get_account(State),
    Item = item:make_item(TempId),
    Reply = 
    case player_pack:add_item2(Account, Item, State) of
	{ok, NState} ->
	    %% ��̨��Ǯ��Ʒ��¼
	    ItemCount = 1,
	    ItemInfo = {item:get_tempid(Item), ItemCount, item:get_instid(Item)},
	    Account = player_data:get_account(State),	    
	    item_money_log:log_item(Account, script_add_item, ItemInfo),
	    {true, NState};
	{error, full} ->
	    sys_msg:send(Account, ?err_bag_full),
	    {false, State};
	_ ->
	    {false, State}
    end,
    Reply.

del_item(TempId, State) ->
    Bag = player_data:get_pack(?pt_bag, State),
    Index = pack:get_index_by_tempid(TempId, Bag),   
    NewState = player_pack:del_item2(Index, State),
    %% ��̨��Ǯ��Ʒ��¼
    ItemCount = 1,
    ItemInfo = {TempId, -ItemCount, 0},
    Account = player_data:get_account(State),	    
    item_money_log:log_item(Account, script_del_item, ItemInfo),
    NewState.

%%-------------------------------�������--------------------------------
update_player_property(PlayerClean, PlayerHealth, PlayerCharm, ActiveValue, State) ->
    player_property_by_action:update(?msg_player_property_by_npc, 
				     PlayerClean, 
				     PlayerHealth, 
				     PlayerCharm, 
				     ActiveValue,
				     State).

get_player_clean(State) ->
    PlayerProperty = player_data:get_property(State),
    player_property:get_player_property(?PLAYERCLEAN, PlayerProperty).
get_Player_health(State) ->
    PlayerProperty = player_data:get_property(State),
    player_property:get_player_property(?PLAYERHEALTH, PlayerProperty).
get_player_charm(State) ->
    PlayerProperty = player_data:get_property(State),
    player_property:get_player_property(?PLAYERCHARM, PlayerProperty).
get_active_value(State) ->
    PlayerProperty = player_data:get_property(State),
    player_property:get_player_property(?ACTIVEVALUE, PlayerProperty).
%%-------------------------------����------------------------------------
whether_have_disease(State) ->
    doctor:whether_have_disease(State).

treat_disease(State) ->
    doctor:treat_disease(State).

get_treat_money(State) ->
    doctor:get_treat_money(State).

%%-------------------------------����------------------------------------
send_express_gift(SenderName, ItemId, Comment, State) ->
    player_gift_express:send_gm_gift(SenderName, ItemId, Comment, State).

%%-------------------------------����------------------------------------
give_task(TaskId, State) ->
    player_task:give(TaskId, State).

complete_task(TaskId, ItemId, State) ->
    player_task:complete(TaskId, ItemId, State).

get_task_flag(TaskId, State) ->
    player_task:get_flag(TaskId, State).

set_task_flag(TaskId, Flag, State) ->
    player_task:set_flag(TaskId, Flag, State).

task_is_exist(TaskId, State) ->
    player_task:is_exist(TaskId, State).

task_is_complete(TaskId, State) ->
    player_task:is_exist_complete_task(TaskId, State).

gen_task(Account, TaskId) ->
    player_task_data:create_task(Account, TaskId).

%%----------------------------------------------------------------------
%% ��ȡ�Ա�
get_sex(State) ->
    BasicData = player_data:get_basic_data(State),
    basic_data:get_data(sex, BasicData).

%% ��ͼ
show_picture(Id, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_show_picture{id=Id}).

%% ���̵�
open_shop(Type, State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_open_shop{type=Type}).

%% ������Ժ
open_game_coin_looks_ui(State) ->
    player_change_looks:start_change_looks(?GAMECOIN, State).

open_eq_coin_looks_ui(State) ->
    player_change_looks:start_change_looks(?EQCOIN, State).

open_house_transaction_ui(State) ->
    player:do_open_ui(?uit_housetransaction, State).

open_npc_shop_ui(State) ->
    player:do_open_ui(?uit_npcshop, State).

%% ��Ʈ����
open_float_bottle_pool_ui(State) ->
    Account = player_data:get_account(State),
    net_helper:send2client(Account, #notify_open_float_bottle_pool{}).

%%-------------------------------ũ��------------------------------------
make_crop() ->
    farm_crop:new_crop().

make_crop(TempId, DamageList, Second) ->
    farm_crop:new_crop(TempId, DamageList, datetime:dec_time(datetime:localtime(), Second)).

make_damage(Type) ->
    farm_damage:make_damage(Type, datetime:localtime(), gm, 2).

give_farm_task(TaskId, State) ->
    farm_task:give(TaskId, State).
%%---------------------------------ϵͳ�ʼ�-----------------------------------
%% �����ѷ�ϵͳ�ʼ�
%% ����:  script:send_self_sys_mail(newer_task_tile, newer_task_content, [117, 118], State) 
%%   ע�� 117 Ϊ��������Զ����ɫ�޲�����, ȡ��item_tplt.xml 
send_self_sys_mail(ATitle, AContent, MailItems, State) when is_list(MailItems) ->
    %% ����: ȡ��common_define_tplt.xml
    Title = tplt:get_common_def(atom_to_list(ATitle)),
    %% ����: ȡ��common_define_tplt.xml
    Content = tplt:get_common_def(atom_to_list(AContent)),
    
    Account = player_data:get_account(State),
    player_mail:send_sys_mail(Account, Title, Content, MailItems).


%%%===================================================================
%%% Internal functions
%%%===================================================================

