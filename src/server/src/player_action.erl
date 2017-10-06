%%% @author  <hjx>
%%% @copyright (C) 2010, 
%%% @doc
%%%   Íæ¼Ò¶¯×÷
%%% @end
%%% Created :  1 Jun 2010 by  <hjx>

-module(player_action).

-export([behaviour_info/1]).

%%%===================================================================
%%% API
%%%===================================================================
-spec behaviour_info(atom()) -> 'undefined' | [{atom(), arity()}].

behaviour_info(callbacks) ->
    [{do_start_action, 2},
     {do_stop_action, 2},
     {can_start_action, 2},
     {get_action_register_msgs, 2}
    ];
behaviour_info(_Other) ->
    undefined. 



