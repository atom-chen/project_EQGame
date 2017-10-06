%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% ũ����ʼ��
%%% @end
%%% Created :  6 Apr 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_crop_init).


%% API
-export([init/0]).

%%%===================================================================
%%% API
%%%===================================================================
%% script:make_damage(Type)  �����ֺ�
%% 1.��Type==2ʱ������ų�.
%% 2.��Type==3ʱ������Ų�.                         
%% 3.��Type==4ʱ������ɺ�.
%%
%%
%%

init() ->
    [script:make_crop(80001, [], 32400),
     script:make_crop(80001, [script:make_damage(3),
			      script:make_damage(3)], 
		      10000),
     script:make_crop(80001, [script:make_damage(4)], 10000),
     script:make_crop(80001,[script:make_damage(2),
			     script:make_damage(2)],10000),
     script:make_crop(),
     script:make_crop()].
