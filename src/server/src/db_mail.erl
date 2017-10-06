%%% @author  hjx
%%% @copyright (C) 2010, 
%%% @doc
%%%  邮件数据库操作
%%% @end
%%% Created : 24 Mar 2010 by  <hjx>

-module(db_mail).

%% Include files


%% Exported Functions
-export([select_mail/1, delete_mail/1, get_player_mails/1, write/1, change/1]).


-define(table_name, player_mail).



select_mail(ID) when is_integer(ID) ->    
    db_common:select_one(?table_name, ID).


delete_mail(ID) when is_integer(ID) ->
    db_common:delete(?table_name, ID).


get_player_mails(Account) when is_atom(Account) ->    
    db_common:index_select(?table_name, Account, reciever).

write(Data) when is_tuple(Data) ->
    db_common:write(Data).

change(OpList) when is_list(OpList) ->
    NewOpList = [{Op, Datas} || {Op, Datas} <- OpList, length(Datas) > 0], 
    case NewOpList of
	[] -> ok;
	_ ->
	    db_common:change(NewOpList)
    end.



