%%%-------------------------------------------------------------------
%%% @author  linyijie
%%% @copyright (C) 2010, 
%%% @doc
%%%  家具类的物品
%%% @end
%%% Created :  6 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(item_furniture).

-include("tplt_def.hrl").
-include("enum_def.hrl").

-behaviour(gen_item).

%% API
-export([can_use/4, do_use/5]).

can_use(_Item, _Owner, _Param, _PlayerData) ->
    true.

do_use(_TemplateID, _Item, _Index, _Owner, _Pos)->
    ok.


