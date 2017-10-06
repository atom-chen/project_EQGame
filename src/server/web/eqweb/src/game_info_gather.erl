%%% @author hongjx <hongjx@35info.cn>
%%% @copyright (C) 2011, hongjx
%%% @doc
%%%  ��Ϸ��Ϣ�ռ�
%%% @end
%%% Created :  4 May 2011 by hongjx <hongjx@35info.cn>

-module(game_info_gather).
-export([run/2]).


run(Req, HostName) ->
    Result = rpc:call(HostName, gm_tool, game_info_gather, []),
    Req:respond({200, [{"Content-Type", "text/html; charset=gb2312"}], [Result]}).



