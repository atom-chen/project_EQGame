%%%-------------------------------------------------------------------
%%% @author linyibin <linyb@35info.cn>
%%% @copyright (C) 2011, linyibin
%%% @doc
%%% 任务初始化
%%% @end
%%% Created :  6 Apr 2011 by linyibin <linyb@35info.cn>
%%%-------------------------------------------------------------------
-module(player_task_init).


%% API
-export([init/2]).


%%%===================================================================
%%% API
%%%===================================================================
init(Account, Sex) ->
    case Sex of
	1 ->
	    [script:gen_task(Account, 1100),
	     script:gen_task(Account, 10000),
	     script:gen_task(Account, 1102)];
	2 ->
	    [script:gen_task(Account, 1101),
	     script:gen_task(Account, 10000),
	     script:gen_task(Account, 1102)]
    end.
