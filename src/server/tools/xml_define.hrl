%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  excelӳ���ļ��Ľṹ�嶨��, ������ṹ���, �����������Ӧ����Դӳ���ļ���excelʹ��
%%% @end

%%% Created : 22 Mar 2010 by  <>

%% ���͵Ķ��������: int, float, string, list_int, list_float, list_string
%% ������list_ ��ͷ�Ķ����б����ʽ, ģ�������д�ĸ�ʽ���� 1, 2, 3 Ҫ�Զ���Ϊ�ָ���
%% ������range ��ʾ����,  ģ�������д�ĸ�ʽ���� 1~2,  �������ɵ�����ΪԪ��{1, 2}�� Ҳ��ֱ����2, ��ʾ{2, 2} 

-define(tplt_list,(
	  [{house_tplt,     % ����ģ���
	    [{int, id},     % id
	     {string, name},% ����
	     {int, level},  % �ȼ�
	     {int, type},   % ����
	     {string, scene_id},        % ����id���ͻ�����
%%	     {string, res},  % ��Ӧ�ķ���ģ����Դ
	     {string, house_furniture}, % ��Ӧ�ķ��ݸ����Ҿ�
	     {int, born_id}, %������Id
	     {int, max_players},              % �������
	     {int, preview_born} %Ԥ��������
	    ]},
	   {test_tplt,        % �����õ�ģ���
	    [{int, id},       
	     {string, name}   
	    ]},
	   {gm_friend_tplt,       % gm����(Ĭ���Զ��ӵ��������)
	    [{string, account},   % �˺�    
	     {string, name},      % ��������
	     {int, house_id},     % ��Ӧ�ķ���ģ��id
	     {int, first_time_magic_box}, % �״α���
	     {int, first_time_garbage_box} % �״�����
	    ]},
	   {house_furniture_tplt, % ���ݸ����Ҿ�(�����ݵ�ʱ���Ĭ�ϼҾ�)
	    [{int, id},           % id
	     {int, furniture_id}, % �Ҿ�ID
	     {int, item_temp_id}, % ��Ӧ����Ʒģ��ID(���ڰѼҾ�ת������Ʒ)
	     {int, x},      % x����
	     {int, z},      % y����
	     {int, height}, % ��ظ߶�
	     {int, flr},      % ¥��
	     {int, face}      % ����
	    ]},
	   {item_tplt,        % ��Ʒģ���
	    [{int, id},       % ��Ʒ��ID
	     {string, name},  % ��Ʒ������
	     {int, type},     % ��Ʒ����������
	     {string, imageset}, % icon���ڵ�ͼƬ��
	     {string, icon},  % ��Ʒ��ͼ��
	     {int, intro_id}, % ��Ʒ��˵��ID
	     {int, overlap},  % ���ѵ���
	     {int, bind},     % �Ƿ��(�󶨵���Ʒ���ܶ���, ���׵�)
	     {int, use_del},  % ��Ʒʹ�ú��Ƿ�Ӱ�����ɾ��
	     {int, sell_price},% �ۼ�(0��ʾ��������)
	     {int, sub_id},    % �������ӱ�ID
	     {int, effect_time_id}, % ʱЧid
	     {int, use_id},
	     {int, voiceid},   % ��Ч���
	     {int, dvoiceid},  % ��Ч���
	     {int, uvoiceid},   % ��Ч���
	     {int, decoration}  % ��װֵ
	    ]},
	   {item_dress_tplt,  % ��Ʒ��װ��
	    [{int, id},       % ID
	     {string, name},  % ����
	     {int, sex},      % �Ա�
	     {int, equip_pos},% װ����λ��
	     {list_int, equip_placeholder}, % ռλ��
	     {int, equip_disp_id}, %װ������ʾID. ��ӦEquipDisplay.xml(װ�����ֱ�)��ID
	     {int, place},    % ��ŷ�ʽ
	     {string, action},% ����
	     {int, type1},    % ����1
	     {int, type2}     % ����2
	    ]},
	   {house_comp,       % ���ݲ�����
	    [{int, id},       % ������ID
	     {string, name},  % ����
	     {int, prop_id},  % 
	     {int, dis_id},   % 
	     {string, pos},   % ����
	     {string, rot},   % ��ת
	     {string, scale}  % ����
	    ]},
	   {common_scene,     % ��������
	    [{int, id},       % ����id
	     {string, name},  % ��������
	     {string, file_res},% �ļ���Դ
	     %% {float, x},
	     %% {float, y},
	     %% {float, z},
	     {int, born_id}, %������Id
	     {int, max_players},            %%�������������
	     {int, busy_players},           %%��æ״̬�����
	     {int, loose_players}           %%����״̬�����
	    ]},	   
	   {scene_copy,                              %% ��Ʒ������
	    [          
	     {int, id},
	     {int, scene_id},                        %% ����ID      
	     {int, copy_id},                         %% ����ID
 	     {string, name}                          %% ��������  
	    ]},
	   {mutex_actions,    % ��������
	    [{int, id},       % id  
	     {string, name},  % ��������
	     {list_int, allow_actions} % ����Ķ�������
	    ]},	   
	   {domestic_service_tplt,    % ��������
	    [{int, id},               % id  
	     {string, name},
	     {int, house_clean},      % ��������
	     %% {int, property_id},      % Ӱ���������id
	     {int, money_rate},       % ������(��λ:�ٷ�֮һ)
	     {int, discount_rate},     % ���۴�������(��λ:�ٷ�֮һ)
	     {string, image}
	    ]},	   	   
	   {domestic_price_tplt,   % ��������۸�
	    [{int, house_level},   % ���ݵȼ�  
	     {int, money}          % ÿ������ ���ĵĽ�Ǯ 
	    ]},	   	   
	   {work_tplt,             % ��
	    [{int, id}, 
	     {int, grade},         % �ȼ�
	     {int, money},         % �ɻ�õĽ�Ǯ��
	     {int, need_clean},             % �������
	     {int, need_health},            % ���｡��
	     {int, need_charm},             % ��������
	     {int, need_active},            % ��Ծֵ
	     {int, need_disease},           % ����ֵ
	     {int, property_id},            % �Ӽ��������
	     {int, event_rate},             % �����¼��ܻ���           
	     {list_int, event_ids},         % �����¼��б�           
	     {list_int, event_rates},       % �����¼���������
	     {string, workImage},
	     {string, typeImageSet},
	     {string, typeImage}
	    ]},
	   {house_magic_box_tplt,          % ���ݱ���
	    [{int, house_level},             % ���ݵȼ� 
	     {list_int, box_ids},            % �ɳ��ֵı���id
	     {list_int, box_rates},          % ������ֻ���
	     {int, max_box_count},           % �������
	     {range, player_box_range},      % ÿ��������÷��ݣ�ÿ���ܲ����ı�������
	     {int, max_create_box_times}     % ÿ�챦���������
	    ]},
	   {magic_box_tplt,                 % ����
	    [{int, id},                      % ����id 
	     {range, money_range},           % �����Ǯ
	     {list_int, item_ids},           % ��Ʒ
	     {list_int, item_rates}          % ��Ʒ���ֻ���
	    ]},
	   {player_level_magic_box_tplt,     % ��ұ����
	    [{int, player_level},            % ��ҵȼ� 
	     {int, max_box_per_day},         % ÿ���ܲ�������������
	     {int, reward_times}             % ������������
	    ]},
	   
	   {house_garbage_tplt,            % ��������
	    [{int, house_level},             % ���ݵȼ� 
	     {list_int, garbage_ids},        % �ɳ��ֵ�����id
	     {list_int, garbage_rates},      % �������ֻ���
	     {int, max_count}                % ���������
	    ]},
	   {garbage_tplt,                 % ����
	    [{int, id},                      % ����id 
	     {range, money_range},           % �����Ǯ
	     {list_int, item_ids},           % ��Ʒ
	     {list_int, item_rates}          % ��Ʒ���ֻ���
	    ]},	
   
	   {random_garbage_tplt,          % �������
	    [{int, id},                      % ����id 
	     {list_int, interval},           % ���������
	     {list_int, garbage_count}       % ��Ʒ���ָ���
	    ]},	   
	   
	   {work_event_tplt,                % �򹤴������¼�
	    [{int, id},                     % �¼�id
	    {int, reward_money},            % ��Ǯ����
	    {list_int, reward_items},       % ��Ʒ����
	    {int, item_rate},               % ��Ʒ���ֻ���
	    {string, image}
	    ]},
	   {change_looks_tplt,             % ����
	    [
	     {int, id},          % �Ա� * 10000 + �ı�(1.����, 2.��ɫ 3.���� 4.��ɫ 5.����) * 1000 + �ı�����ֵ(�·��ͣ�����ɫ)
	     {int, money_type},  % ��������(1Ϊ��Ϸ��ң�2Ϊ�㿨����)
	     {int, money}        % ����
	   ]},
	   {teleport_tplt,      % ���͵�
	    [
	     {int, id},           % ��ˮ��
	     {int, src_scene_id}, % ��������id
	     {int, born_id}, %������Id
	     {float, src_x},        % ���͵�
	     {float, src_y},
	     {float, src_z},
	     {float, src_radius},  % ���͵�뾶
	     {int, dest_scene_id}, % Ŀ�곡��id
	     %% {float, dest_x},      % Ŀ���
	     %% {float, dest_y},
	     %% {float, dest_z},
	     {int, dest_dir}      % ��������ﳯ��
	   ]},
	   {create_random_dress,     % ���������װ�����
	    [{int, id},       % id
	     {int, sex},      % �Ա�
	     {int, fashion1}, % ��װ1
	     {int, fashion2}, % ��װ2
	     {int, fashion3}, % ��װ3
	     {int, fashion4}, % ��װ4
	     {int, fashion5}  % ��װ5
	    ]},
	   {npc_tplt,             % npc
	    [{int, npc_id},      % npc����
	     {int, body},        % ����
	     {int, head},        % ͷ
	     {int, hair},        % ͷ��
	     {int, equip1},      % ��ʼװ��1
	     {int, equip2},      % ��ʼװ��2
	     {int, skeleton},    % ����
	     {string, npc_name}  % npc����
	    ]},
	   {npc_map_mapping_tplt,% ��ͼ��Ӧnpc��ϵ
	    [{int, id},         % ��ͼ����
	     {int, npc_id},     % ������npc
	     {string, npc_name},% npc����
	     %% {float, x},        % x������
	     %% {float, y},        % y������
	     %% {float, z},        % z������
	     {int, born_id}, %������Id
	     {int, script_id},  % ִ�нű����
	     {int, action},     % �Զ����Ŷ���
	     {int, towards}     % ����
	    ]},
	   {player_level_tplt,    % ��ҵȼ���ʱ�����չ�ϵ
	    [{int, level},        % ��ҵȼ�
	     {int, online_time} % �������ʱ��
	    ]},
	   {player_property_tplt,     %% ϵͳʱ����������ԵĹ�ϵ
	    [{int, property_id},                %% ������Id
	     {int, player_clean},        %% �������
	     {int, player_health},       %% ���｡��
	     {int, player_charm},        %% ��������
	     {int, active_value}         %% ��Ծֵ
	    ]},
	   {player_property_by_stime_tplt,
	    [{int, min_property},          %% ʱ�䷶Χ(��Сʱ��)
	     {int, max_property},          %% ʱ�䷶Χ(���ʱ��)
	     {int, ref_id}                 %% ������Id
	    ]},
	   {sys_shop_tplt,            %% ϵͳ�̳�
	    [{int, goods_id},         %% ��ƷID
	     {int, item_id},          %% ��ƷID
	     {int, type},             %% ����
	     {int, sex},              %% �Ա�
	     {int, money_type},       %% ��������
	     {int, price},            %% �۸�
	     {int, intro},            %% ����
	     {int, commend},          %% �Ƽ�
	     {int, broadcast_id}      %% ϵͳ����id
	    ]},
	   {npc_shop_tplt,            %% ϵͳ�̳�
	    [{int, goods_id},         %% ��ƷID
	     {int, item_id},          %% ��ƷID
	     {int, type},             %% ����
	     {int, sex},              %% �Ա�
	     {int, price},            %% ��������Ʒ�ļ۸�
	     {int, intro},            %% ����
	     {int, sale_type}         %% ��������
	    ]},
	   {furni_tplt,               %% �Ҿ�ģ���
	    [{int, furniture_id},     %% �Ҿ�Id
	     {int, max_use_player},   %% ���ʹ������
	     {int, use_sex},          %% ʹ�õ��Ա� 
	     {int, permission},       %% ʹ��Ȩ��
	     {list_int, function_list},%% �����б�
	     {int, use_type},          %% ʹ������
	     {int, status_qty}        %% ״̬����
	    ]},
	   {item_effect_time_tplt,      %% ��ƷʱЧ
	    [{int, id},                % id
     	     {int, type},              % ����(1��ʾ����ʱʱЧ��2��ʾʱ���ʱ��)
     	     {int, effect_time},       % ��Чʱ��(��)
	     {int, trade_cut_time},    % ���׿۳�ʱ��(��)
	     {int, del_time}           % ɾ��ʱ��(��ʽYYYYMMDD), 00:00��ʼɾ��
	    ]},
	   {furni_interact_tplt,
	    [{int, function_id},      %% ����id
	     {int, property_id},      %% �������Id
	     {int, status},           %% �޸ĵļҾ�״̬
	     {int, action_type}       %% ��������(0:δ����, 3:ϴ�� 11:ϴ��)
	    ]},
	   {common_scene_furniture,
	    [{int, id},        %% id (��ʱ��ʵ��id����)
	     {int, object_id}  %% �Ҿ�id
	    ]},
	   {player_task_action_tplt,
	    [{int, id},        %% id (�ݷú��ѣ�����֮���)
	     {string, desc}    %% ���� (�������1/3����)
	    ]},
	   {player_task_tplt,                       %% �������ģ���
	    [{int, task_id},                        %% ����Id
	     {string, title},
	     {string, intention},
	     {int, step_count},                     %% ��������Ҫ�Ĳ�������
	     {list_int, props},                     %% ���������õĵ���
	     {list_int, props_count},               %% ���������õĵ�������
	     {list_int, require_items},             %% ��Ҫ����ƷId
	     {list_int, require_items_count},       %% ��Ҫ����Ʒ����
	     {string, describe},
	     {list_int, select_reward_items},       %% ѡ��������Ʒ
	     {list_int, select_reward_items_count}, %% ѡ��������Ʒ����
	     {list_int, fixed_reward_items},        %% �̶���������Ʒ
	     {list_int, fixed_reward_items_count},  %% �̶���������Ʒ����
	     {int, reward_game_coin},               %% ��������Ϸ��
	     {int, reward_eq_coin},                 %% ������EQ��
	     {int, time_limit},                     %% ʱ������(����Ϊ��λ)
	     {int, property_id},                    %% ��Ӧ���������Id
	     {int, is_repeat},                      %% �Ƿ���ظ���
	     {int, is_show},                        %% �Ƿ���ʾ����
	     {int, is_give_up},                     %% �Ƿ�����������
	     {int, sub_id},                         %% 
	     {list_int, require_actions},           %% ��Ҫ�Ķ���Id(����˵�ݷú���֮��)
	     {list_int, require_actions_times},     %% ��Ҫ�Ķ�������
	     {int, kind}                            %% ����: 2 ÿ������,1 ��������
	    ]},
	   {farm_task_tplt,
	    [{int, task_id},                        %% ����Id
	     {string, title},                       %% �������
	     {string, intention},                   %% ����Ŀ��
	     {string, describe},                    %% ��������
	     {string, imageset},                    %% ������ʾ��ͼƬ
	     {string, image},                       %% 
	     {list_int, fixed_reward_items},        %% �̶���������Ʒ
	     {list_int, fixed_reward_items_count},  %% �̶���������Ʒ����
	     {int, reward_game_coin},               %% ��������Ϸ��
	     {int, reward_eq_coin},                 %% ������EQ��
	     {int, exp},
	     {int, time_limit},                     %% ʱ������(����Ϊ��λ)
	     {int, is_repeat}                       %% �Ƿ���ظ���
	    ]},
	   {chain_task_tplt,                         %% ��ʽ����
	    [{int, task_id},                         %% ��ʽ����Id
	     {list_int, tasks},                      %% �����б�
	     {int, is_repeat},                       %% �Ƿ���ظ���
	     {int, is_restart}                       %% �Ƿ��������¿�ʼ
	    ]},
	   {farm_seed_tplt,                         %% ����
	    [{int, id},                             %% Id
	     {string, name},                        %% ����
	     {int, expect_output},                  %% Ԥ�ڲ���
	     {int, fruit_id},                       %% ��ʵID
	     {int, sprout_time},                    %% ��ѿ��ʱ��
	     {int, s_leaf_time},                    %% СҶ�ӵ�ʱ��
	     {int, b_leaf_time},                    %% ��Ҷ�ӵ�ʱ��
	     {int, bloom_time},                     %% ������ʱ��
	     {int, fruit_time},                     %% ���ʱ��
	     {int, pick_time},                      %% ��ժʱ��
	     {int, drought_id},                     %% ���ֵ�id
	     {int, pest_id},                        %% ���ֵ�id
	     {int, grass_id},                       %% �Ӳݵ�id
	     {int, use_level},                      %% ʹ�õȼ�(��ֲ�ȼ�)
	     {int, pick_exp},                       %% ��ժ�ܹ���õľ���
	     {int, redland}                         %% �Ƿ������ں�������(1--����ͨ���غͺ���������ֲ��
	                                            %% 2--Ϊ���������ӣ�ֻ���ں���������ֲ)
	    ]},
	   {farm_damage_tplt,                       %% ũ���ֺ�
	    [{int, id},                             %% �ֺ�Id
	     {int, sprout_ratio},                   %% ��ѿ�׶ε��ֺ�����
	     {int, s_leaf_ratio},                   %% СҶ�ӽ׶ε��ֺ�����
	     {int, b_leaf_ratio},                   %% ��Ҷ�ӽ׶ε��ֺ����� 
	     {int, bloom_ratio},                    %% �����׶ε��ֺ�����
	     {int, fruit_ratio},                    %% ����׶ε��ֺ�����
	     {int, interval},                       %% �ֺ������ļ��ʱ��
	     {int, reduce_health}                   %% Ӱ�콡����
	    ]},
	   {farm_pet_tplt,                          %% ũ������
	    [{int, pet_id},                         %% ����Id
	     {int, rate},                           %% ���ּ���
	     {int, min_coin},                       %% ������֣�����۳������ٽ��
	     {int, max_coin}                        %% ������֣�����۳��������
	    ]},
	   {common_define_tplt,                     %% ��������ģ���
	   [{string, key},                          
	    {string, value}
	   ]},
	   %% {farm_announcement_tplt,                 %% ����
	   %%  [{int, id},                             %% ΨһId
	   %%   {string, announcement}                    %% ����Id
	   %%  ]},
	   {farm_shop_tplt,           %% ũ���̳�
	    [{int, item_id},          %% ��ƷID
	     {int, gameprice},        %% ��Ҫʹ����Ϸ�ҹ���
	     {int, eqprice},          %% ��Ҫʹ��eq�㿨����
	     {string, intro},         %% ����
	     {string, special},       %% ������;
	     {int, level},            %% �ȼ�����
	     {int, repeat_buy},       %% �Ƿ������ظ�����
	     {int, add_exp},          %% �������Ʒ�����ӵľ���
	     {list_string, rewards}   %% �����б�
	    ]},
	   {farm_assart_tplt,         %% ũ������
	    [{int, plot_index},       %% ���ѵĵؿ���(��һ���, �ڶ����, ...)
	     {int, level},            %% ũ���ȼ�
	     {int, money}             %% ��Ҫ���ĵ���Ϸ��
	    ]},
	   {farm_level_tplt,          %% ũ������
	    [{int, level},            %% �ȼ�
	     {int, next_exp},         %% ��һ������ľ���
	     {int, sow_exp},          %% ���ֻ�õľ���
	     {int, water_exp},        %% ��ˮ��õľ���
	     {int, weed_exp},         %% ���ݻ�õľ���
	     {int, worm_exp},         %% �����õľ���
	     {int, hoeing_exp},       %% ���ػ�õľ���
	     {string, title}          %% �ƺ�
	    ]},
	   {farm_decoration_tplt,     %% ũ��װ�α�
	    [{int, id},               %% ��Ʒ��Id
	     {int, type},             %% װ������
	     {int, static_model},  %% ��̬ģ��
	     {int, dynamic_model},    %% ��̬ģ��
	     {string, scene_name}
	    ]},
	   {farm_decoration_init_tplt,
	    [{int, background},       %% ����
	     {int, house},            %% ����
	     {int, doghouse},         %% ����
	     {int, signpost},         %% ·��
	     {int, fence},            %% ��� 
	     {int, scarecrow},        %% ������
	     {int, billboard},        %% ��ʾ��
	     {int, billboard_content} %% ��ʾ������
	    ]},
	   {farm_setting_tplt,        %% ũ������
	    [{string, water},         %% ��ˮ
	     {string, weed},          %% ����
	     {string, worm},          %% ����
	     {string, put_grass},     %% �Ų�
	     {string, put_pest}       %% ����
	    ]},
	   {player_disease_probability_tplt,  %% ��Ҽ����������ʱ�
	    [{int, player_health},          %% ��������
	     {int, probability}               %% ��������
	    ]},
	   {player_disease_type_tplt,         %% ��������ģ���
	   [{int, type},                      %% ��������
	    {int, rate},                      %% �ò�ͬ�����ı���
	    {int, upgrate_probability},       %% ��ߴ�������
	    {int, level},                     %% �����ĵȼ�
	    {int, treatment_costs},           %% ���Ʒ���
	    {int, min},                       %% ��С����ֵ
	    {int, max},                       %% ��󸡶�ֵ
	    {list_int, special_event_id},     %% �����¼�Id
	    {list_int, event_id_probability}, %% �����¼�Id�ļ���
	    {int, special_event_probability}  %% �����¼�����
	   ]},
	   {player_disease_special_event_tplt,%% ���������¼�ģ���
	     [{int, special_event_id},          %% �����¼�Id
	     {int, property_id},               %% Ӱ�������Id
	     {string, image}                   %% ͼƬ��ַ
	    ]},
	   {farm_prop_tplt,                    %% ũ������
	    [{int, id},                        %% ��ƷId
	     {int, type},                      %% ����
	     {int, effect_range},              %% Ӱ�췶Χ
	     {int, effect_time},               %% Ӱ��ʱ��
	     {int, effect_output},             %% Ӱ�����
	     {list_int, effect_stage},         %% Ӱ��׶�
	     {int, reduce_time}                %% ����ʱ��
	    ]},
	   {house_transaction_tplt,           %% ���ݽ��ױ�
	   [{int, id},                        %% ����Id
	    {string, name},                   %% ����
	    {int, type},                      %% ����
	    {int, recommend},                 %% �Ƽ�
	    {int, level},                     %% �ȼ�
	    {int, pay_type},                  %% ֧������
	    {int, price},                     %% �۸�
	    {int, cost_flower},               %% �����ʻ�ֵ
	    {int, need_decoration},           %% ��Ҫ��װֵ
	    {int, house_id},                  %% ��Ӧ�ķ���ģ��Id
	    {string, intro},                  %% ���
	    {int, broadcast_id}               %% ϵͳ����id
	   ]},
	   {sys_broadcast_tplt,               %% ϵͳ����
	   [{int, id},                        %% ����Id
	    {string, content},                %% ����
	    {int, show_seconds}               %% ��ʾʱ��
	   ]},
	   {player_property_interaction_tplt,
	    [{int, id},
	     {int, monitor_player_clean},           %% �������
	     {int, monitor_player_health},          %% ���｡��
	     {int, monitor_player_charm},           %% ��������
	     {int, monitor_active_value},           %% ��Ծֵ
	     {int, ref_id}                          %% ����������Id
	    ]},
	   {farmland_upgrade_tplt,                  %% ũ�������ؿ��ѱ�
	    [{int, land_num},                       %% ϵͳԼ�������ؿ�
	     {int, level},                          %% ��Ҫ�ĵȼ�
	     {int, money}                           %% ��Ҫ�Ľ��
	    ]},
	   {red_farmland_harvest_tplt,              %% �������ջ�����
	    [{int, id},                             %% ���ظ���id, û��ʵ����;
	     {int, ratio},                          %% ����
	     {int, type},                           %% ����
	     {int, data},                           %% ������ƥ������ݶ�
	     {int, count}                           %% ��dataƥ�������
	    ]},
	   {farmfruit_aberrance_tplt,               %% ũ����ʵ����
	    [{int, seed_id},                        %% ����ID�μ�farm_seed_tplt��id
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
	   {gift_box_tplt,                            %% �����ģ���
	    [{int, id},                               %% ����Id
	     {int, type},                             %% ��������
	     {string, name},                          %% ��������
	     {int, game_coin},                        %% ��Ҫ����Ϸ��
	     {int, eq_coin},                          %% ��Ҫ��EQ��
	     {int, model_id},
	     {string, imageset},
	     {string, image},
	     {string, desc}                           %% ���ӽ���
	    ]},
	   {gift_card_tplt,                           %% ���￨ģ���
	    [{int, id},                               %% ���￨Id
	     {int, type},                             %% ���￨����
	     {string, name},                          %% ���￨����
	     {int, game_coin},                        %% ��Ҫ����Ϸ��
	     {int, eq_coin},                          %% ��Ҫ��EQ��
	     {string, imageset},
	     {string, image},
	     {string, desc},                          %% ���￨����
	     {string, url}
	    ]},	   
	   {day_night_config_tplt,                    %% �����ҹ
	    [{list_int, hh_mm_ss},                    %% ʱ��
	     {int, is_day}                            %% �Ƿ����
	    ]},	   
	   {farmhoeing_awards_tplt,                 %% ũ�����ؽ�����
	    [{int, level},                          %% ũ���ȼ�
	     {int, ratio},                          %% ����
	     {list_int, item_id},                   %% ��Ʒid(����)
	     {int, count}                           %% ��Ʒ������
	    ]},
	   {born,                              %% �������
	    [{int, id},                            
	     {list_space_int, pos},
	     {int, dir},
	     {string, lookat},
	     {int, pitch},
	     {int, yaw},
	     {int, zoom}
	    ]
	   },
	   {                               %%�����֮����tag
	     cupid_tag,
	     [{string, tag}]
	   }
	  ])
       ).
