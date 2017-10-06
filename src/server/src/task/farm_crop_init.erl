%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 农场初始化
%%% @end
%%% Created :  6 Apr 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(farm_crop_init).


%% API
-export([init/0]).

%%%===================================================================
%%% API
%%%===================================================================
%% script:make_damage(Type)  放置灾害
%% 1.当Type==2时，代表放虫.
%% 2.当Type==3时，代表放草.                         
%% 3.当Type==4时，代表干旱.
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
