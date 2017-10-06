%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  ��Ʒ����, ���Ӽ�װֵ
%%% @end
%%% Created : 19 Sep 2011 by hongjx <hongjx@35info.cn>

-module(db_update_item_20110919).

-export([add_decoration_field/0, add_decoration_field2/0, fix_array_default_val/0]).


%% ������
%-record(item, {instance_id=0, template_id=0, del_time=[]}).
%% ������
%%-record(item, {instance_id=0, template_id=0, del_time=#stime{}, decoration_val=0}).
%%%=================================================================================
%%% ��ʱ�����ݽṹ
-record(pack_grid, {count=0, lock=0, item_data=[]}).
-record(player_body, {owner="", grid_vec=[]}).
-record(player_pack, {owner="", grid_vec=[]}).
-record(farm_depot, {account, grids}).
-record(player_depot, {account,
		       money,   %% ��Ϸ��
		       pack     %% ����
		      }).
-record(player_mail, {
	  id, 
	  reciever,       %% �������˺�
	  send_time,      %% ����ʱ��
	  sender,         %% �������˺�
	  type,           %% ����
	  title,          %% ����
	  content,        %% ����
	  items,          %% ��Ʒ
	  read_status,    %% ״̬��1.δ����2.�������⣬3.��������)
	  recver_name,
	  sender_name
	 }
       ).

-record(gift_box, {inst_id=0, sender="", sender_name="",
		   receiver="", receiver_name="", name="", grid_vec=[],
		   box_id=0, card_id=0, comment="", pos, date, is_anonymous=0}).

-record(player_gift, {account,                      %% ����ʺ�
		      send_box = #gift_box{},       %% ��ǰ�������
		      send_box_vec = [],            %% ��ҷ��͹�������б�
		      receive_box_vec = [],         %% ��ҽ��յ�(δ�鿴)����б�
		      viewed_box_vec = [],          %% ����Ѳ鿴��������б�
		      send_express_vec = [],        %% ��ҷ��͵Ŀ���б�
		      receive_express_vec = [],     %% ��ҽ��չ��Ŀ���б�
		      viewed_express_vec = [],      %% �Ѳ鿴���Ŀ���б�
		      subscribe_express_vec = []    %% ���ĵĿ���б�
		     }).
-record(gift_express, {type=0, inst_id=0, sender="", sender_name="", 
		       receiver="", receiver_name="", grid_vec=[], 
		       card_id=0, comment="", send_date, date}).

-record(farm_decoration, {background,
			  house,
			  doghouse,
			  dog,
			  signpost,
			  fence,
			  scarecrow,
			  billboard,
			  billboard_content}).

-record(player_farm,    % ũ������
	{account,       % ũ������
	 exp=0,         % ũ������
	 decoration,    % ũ��װ��
	 pet_food,      % ����ʳ��
	 crops=[],      % ũ����
	 money=0,
	 fop_grass,     % ���ò��ֵ�����
	 fop_pest,      % ���ó��ֵ�����
	 foc_grass,     % �����ֵ�����
	 foc_pest       % �����ֵ�����
	}
       ).

-record(house_data, {owner="", instance_id=0, template_id=0, furniture_permission=0, 
		     furniture_vec=[], room_tex_vec=[], component_tex_vec=[], 
		     component_mesh_vec=[], welcome_words="", house_permission=0, 
		     visit_log=[], buy_date, level=0, gift_box_vec=[], magic_box_vec=[], house_clean=0}).

-record(house_plot,         % ������ؿ��ϵ��
	{account,           % ����ʺ�
	 plots              % �ؿ鼯��
	}).

-record(plot,               % �ؿ�
	{id,                % �ؿ��ʶ
	 house_data         % ��������
	}).


add_decoration_field() ->
    convert_table(player_body),
    convert_table(player_pack),
    convert_table(player_depot),
    convert_table(farm_depot),
    convert_table(player_mail),
    convert_table(player_farm),
    convert_table(player_gift),
    {atomic, ok}.


add_decoration_field2() ->
    convert_table(house_data),
    convert_table(house_plot),
    {atomic, ok}.


convert_table(Tab) ->
    Keys = db_common:all_keys(Tab),
    [begin
	 %%io:format("key:~p~n", [{Tab, K}]),
	 R = db_common:select_one(Tab, K),
	 NewR = convert_record(R),
	 db_common:write(NewR) 
     end || K <- Keys].


convert_record(#house_plot{plots=Plots}=R) -> 
    NPlots = convert_plot_list(Plots),
    R#house_plot{plots=NPlots};
convert_record(#house_data{gift_box_vec=GiftBoxList}=R) -> 
    NGiftBoxList = convert_gift_box_list(GiftBoxList),
    R#house_data{gift_box_vec=NGiftBoxList};


convert_record(#player_mail{items=GridVec}=R) -> 
    NGridVec = convert_grid_vec(GridVec),
    R#player_mail{items=NGridVec};
convert_record(#player_depot{pack=GridVec}=R) -> 
    NGridVec = convert_grid_vec(GridVec),
    R#player_depot{pack=NGridVec};
convert_record(#farm_depot{grids=GridVec}=R) -> 
    NGridVec = convert_grid_vec(GridVec),
    R#farm_depot{grids=NGridVec};
convert_record(#player_body{grid_vec=GridVec}=R) -> 
    NGridVec = convert_grid_vec(GridVec),
    R#player_body{grid_vec=NGridVec};
convert_record(#player_pack{grid_vec=GridVec}=R) -> 
    NGridVec = convert_grid_vec(GridVec),
    R#player_pack{grid_vec=NGridVec};
convert_record(#player_farm{decoration=Decoration}=R) -> 
    NDecoration = convert_decoration(Decoration),
    R#player_farm{decoration=NDecoration};
convert_record(#player_gift{
		      send_box = GiftBox,       %% ��ǰ�������
		      send_box_vec = SendBoxVec,            %% ��ҷ��͹�������б�
		      receive_box_vec = RecvBoxVec,         %% ��ҽ��յ�(δ�鿴)����б�
		      viewed_box_vec = ViewBoxVec,          %% ����Ѳ鿴��������б�
		      send_express_vec = SendExpressVec,        %% ��ҷ��͵Ŀ���б�
		      receive_express_vec = RecvExpressVec,     %% ��ҽ��չ��Ŀ���б�
		      viewed_express_vec = ViewExpressVec,      %% �Ѳ鿴���Ŀ���б�
		      subscribe_express_vec = SubscribeExpressVec}    %% ���ĵĿ���б�
	       =R) ->
    NGiftBox = convert_gift_box(GiftBox),
    NSendBoxVec = convert_gift_box_list(SendBoxVec),
    NRecvBoxVec = convert_gift_box_list(RecvBoxVec),
    NViewBoxVec = convert_gift_box_list(ViewBoxVec),

    NSendExpressVec = convert_gift_express_list(SendExpressVec),
    NRecvExpressVec = convert_gift_express_list(RecvExpressVec),
    NViewExpressVec = convert_gift_express_list(ViewExpressVec),
    NSubscribeExpressVec = convert_gift_express_list(SubscribeExpressVec),

    R#player_gift{
      send_box = NGiftBox,       %% ��ǰ�������
      send_box_vec = NSendBoxVec,            %% ��ҷ��͹�������б�
      receive_box_vec = NRecvBoxVec,         %% ��ҽ��յ�(δ�鿴)����б�
      viewed_box_vec = NViewBoxVec,          %% ����Ѳ鿴��������б�
      send_express_vec = NSendExpressVec,        %% ��ҷ��͵Ŀ���б�
      receive_express_vec = NRecvExpressVec,     %% ��ҽ��չ��Ŀ���б�
      viewed_express_vec = NViewExpressVec,      %% �Ѳ鿴���Ŀ���б�
      subscribe_express_vec = NSubscribeExpressVec}.    %% ���ĵĿ���б�

convert_decoration(#farm_decoration{background=A, 
				    house=B, 
				    doghouse=C, 
				    dog=D, 
				    signpost=E, 
				    fence=F, 
				    scarecrow=G, 
				    billboard=H, 
				    billboard_content=I}=R) ->
    
    R#farm_decoration{background=convert_item(A),
		      house=convert_item(B), 
		      doghouse=convert_item(C),  
		      dog=convert_item(D),  
		      signpost=convert_item(E),  
		      fence=convert_item(F),  
		      scarecrow=convert_item(G),  
		      billboard=convert_item(H),  
		      billboard_content=convert_item(I)}.
    


convert_gift_express_list(List) ->
    F = fun(GiftExp, Acc) ->
		NGiftExp = convert_gift_express(GiftExp),
		[NGiftExp | Acc]
	end,
    lists:foldr(F, [], List).


convert_gift_express(#gift_express{grid_vec=GridVec}=GiftBox) ->
    NGridVec = convert_grid_vec(GridVec),    
    GiftBox#gift_express{grid_vec=NGridVec}.

convert_plot(#plot{house_data=HouseData}=R) ->
    NHouseData = case HouseData of
		     [] -> HouseData;
		     _ ->  convert_record(HouseData)
		 end,
    R#plot{house_data=NHouseData}.


convert_plot_list(List) ->
    F = fun(Plot, Acc) ->
		NPlot = convert_plot(Plot),
		[NPlot | Acc]
	end,
    lists:foldr(F, [], List).


convert_gift_box_list(List) ->
    F = fun(GiftBox, Acc) ->
		NGiftBox = convert_gift_box(GiftBox),
		[NGiftBox | Acc]
	end,
    lists:foldr(F, [], List).


convert_gift_box(#gift_box{grid_vec=GridVec}=GiftBox) ->
    NGridVec = convert_grid_vec(GridVec),    
    GiftBox#gift_box{grid_vec=NGridVec}.

convert_grid_vec(full) ->  %% �������ݿ�����������
    full;
convert_grid_vec(GridVec) when is_list(GridVec) ->
    F = fun(Grid, Acc) ->
		NGrid = convert_grid(Grid),
		[NGrid | Acc]
	end,
    lists:foldr(F, [], GridVec);
convert_grid_vec(GridVec) ->
    F = fun(Idx, Grid, Acc) -> 
		NGrid = convert_grid(Grid),
		array:set(Idx, NGrid, Acc)
	end,
    array:foldl(F, pack:new(array:size(GridVec)), GridVec).

convert_grid(#pack_grid{item_data=Item}=Grid) ->
    NItem = convert_item(Item), 
    Grid#pack_grid{item_data=NItem}.

%% ���Ӽ�װֵ
convert_item(Item) when size(Item) == 4 -> % ������
    erlang:append_element(Item, 0);
convert_item(Item) when size(Item) == 5 -> % �Ѿ�ת������
    Item.


fix_array_default_val() ->
    add_decoration_field(),
    add_decoration_field2().



