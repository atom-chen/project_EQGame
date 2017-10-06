%%% -------------------------------------------------------------------
%%% Author  : NoteBook
%%% Description :
%%%
%%% Created : 2009-9-24
%%% -------------------------------------------------------------------
-module(db_cupid).

-include("enum_def.hrl").

-export([add/3, create/0, update_status/2, remove/1, select/1]).
-export([first/0,next/1, first_registered/0, next_registered/1]).
-export([first_matched_boy/0, next_matched_boy/1, first_matched_girl/0, next_matched_girl/1]).

-define(table_name, cupid).
-define(null, undefined).

create()->
    ets:new(?table_name, [set, public, named_table]).

add(Account, Sex, Tags)->
    case select(Account) of 
	?null->
	    ets:insert(?table_name,{Account, Sex, Tags, registered});
	_->
	    false
    end.

update_status(Account, Status)->
    {Account, Sex, Tags, _OldStatus} = select(Account),
    ets:insert(?table_name, {Account, Sex, Tags, Status}),
    ok.

remove(Account)->
    ets:delete(?table_name, Account).

select(Account)->
    ItemList=ets:lookup(?table_name, Account),
    case ItemList of 
	[]->
	    ?null;
	_->
	    [Item|_Other]=ItemList,
	    Item
    end.

first_registered()->
    Item = first(),
    case Item of 
	?null->
	    ?null;
	_->
	    {Account, _Sex, _Tages, Status} = Item,
	    case Status of 
		registered->
		    Item;
		_ ->
		    next_registered(Account)
	    end
    end.

first()->
    Key = ets:first(?table_name),
    case Key of 
	'$end_of_table'->
	    ?null;
	_->
	    select(Key)
    end.

next_registered(Account)->
    NextKey = ets:next(?table_name, Account),
    case NextKey of 
	'$end_of_table'->
	    ?null;
	_->
	    Item = select(NextKey),
	    {_Account, _Sex, _Tages, Status} = Item,
	    case Status of 
		registered->
		    Item;
		_ ->
		    next(NextKey)
	    end
    end.

next(Account)->
    NextKey = ets:next(?table_name, Account),
    case NextKey of 
	'$end_of_table'->
	    ?null;
	_->
	    select(NextKey)
    end.

is_matched_boy(Item)->
    {_Account, Sex, _Tags, Status} = Item,
    case Sex of 
	?st_boy->
	    case Status of 
		matched->
		    true;
		_->
		    false
	    end;	    
	_->
	    false
    end.

is_matched_girl(Item)->
    {_Account, Sex, _Tags, Status} = Item,
    case Sex of 
	?st_girl->
	    case Status of 
		matched->
		    true;
		_->
		    false
	    end;	    
	_->
	    false
    end.

next_matched_boy(Current)->
    {CurrentAccount, _Sex, _Tags, _Status} = Current,
    Item = next(CurrentAccount),
    case Item of
	undefined-> %%已经末尾
	    undefined;
	_->
	    case is_matched_boy(Item) of 
		true->
		    Item;
		_ ->
		    next_matched_boy(Item)
	    end
    end.
next_matched_girl(Current)->
    {CurrentAccount, _Sex, _Tags, _Status} = Current,
    Item = next(CurrentAccount),
    case Item of
	undefined-> %%已经末尾
	    undefined;
	_->
	    case is_matched_girl(Item) of 
		true->
		    Item;
		_ ->
		    next_matched_girl(Item)
	    end
    end.

first_matched_boy()->
    Item = first(),
    case Item of 
	undefined->
	    undefined;
	_->
	    case is_matched_boy(Item) of 
		true->
		    Item;
		_ ->
		    next_matched_boy(Item)
	    end   
    end.

first_matched_girl()->
    Item = first(),
    case Item of 
	undefined->
	    undefined;
	_->
	    case is_matched_girl(Item) of 
		true->
		    Item;
		_ ->
		    next_matched_girl(Item)
	    end   
    end.
