%%%-------------------------------------------------------------------
%%% @author  <>
%%% @copyright (C) 2010, 
%%% @doc
%%% 时间，日期函数
%%% @end
%%% Created :  8 Apr 2010 by  <>
%%%-------------------------------------------------------------------
-module(datetime).

-include("packet_def.hrl").

-export([diff_time/2, localtime/0, make_time/1, make_time/6, empty/0]).
-export([date_to_gregorian_days/1, 
	 date_to_gregorian_days/3,
	 datetime_to_gregorian_seconds/1,
	 day_of_the_week/1,
	 day_of_the_week/3,
	 gregorian_days_to_date/1,
	 gregorian_seconds_to_datetime/1,
	 is_leap_year/1,
	 last_day_of_the_month/2,
	 local_time/0, 
	 local_time_to_universal_time/2, 
	 local_time_to_universal_time_dst/1, 
	 now_to_datetime/1,			% = now_to_universal_time/1
	 now_to_local_time/1,
	 now_to_universal_time/1,
	 seconds_to_daystime/1,
	 seconds_to_time/1,
	 time_difference/2,
	 time_to_seconds/1,
	 universal_time/0,
	 universal_time_to_local_time/1,
	 valid_date/1,
	 valid_date/3,
	 date/0,
	 time/0,
	 now/0,
	 add_time/2,
	 dec_time/2,
	 is_equal/2]).

%%----------------------------------------------------------------------
%% Types
%%----------------------------------------------------------------------

-type year()     :: non_neg_integer().
-type year1970() :: 1970..10000.	% should probably be 1970..
-type month()    :: 1..12.
-type day()      :: 1..31.
-type hour()     :: 0..23.
-type minute()   :: 0..59.
-type second()   :: 0..59.
-type daynum()   :: 1..7.
-type ldom()     :: 28 | 29 | 30 | 31. % last day of month

-type t_now()    :: {non_neg_integer(),non_neg_integer(),non_neg_integer()}.

-type t_date()         :: {year(),month(),day()}.
-type t_time()         :: {hour(),minute(),second()}.
-type t_datetime()     :: {t_date(),t_time()}.
-type t_datetime1970() :: {{year1970(),month(),day()},t_time()}.



-spec date_to_gregorian_days(year(),month(),day()) -> non_neg_integer().
date_to_gregorian_days(Year, Month, Day) when is_integer(Day), Day > 0 ->
     calendar:date_to_gregorian_days(Year, Month, Day).


-spec date_to_gregorian_days(t_date()) -> non_neg_integer().
date_to_gregorian_days({Year, Month, Day}) ->
     calendar:date_to_gregorian_days(Year, Month, Day).


%% datetime_to_gregorian_seconds(DateTime) = Integer
%%
%% Computes the total number of seconds starting from year 0,
%% January 1st.
%%
-spec datetime_to_gregorian_seconds(t_datetime()) -> non_neg_integer().
datetime_to_gregorian_seconds({Date, Time}) ->
     calendar:datetime_to_gregorian_seconds({Date, Time}).



%% day_of_the_week(Year, Month, Day)
%% day_of_the_week({Year, Month, Day})
%%
%% Returns: 1 | .. | 7. Monday = 1, Tuesday = 2, ..., Sunday = 7.
%%
-spec day_of_the_week(year(), month(), day()) -> daynum().
day_of_the_week(Year, Month, Day) ->
     calendar:day_of_the_week(Year, Month, Day).

-spec day_of_the_week(t_date()) -> daynum().
day_of_the_week({Year, Month, Day}) ->
     calendar:day_of_the_week(Year, Month, Day).


%% gregorian_days_to_date(Days) = {Year, Month, Day}
%%
-spec gregorian_days_to_date(non_neg_integer()) -> t_date().
gregorian_days_to_date(Days) ->
     calendar:gregorian_days_to_date(Days).



%% gregorian_seconds_to_datetime(Secs)
%%
-spec gregorian_seconds_to_datetime(non_neg_integer()) -> t_datetime().
gregorian_seconds_to_datetime(Secs) when Secs >= 0 ->
     calendar:gregorian_seconds_to_datetime(Secs).


%% is_leap_year(Year) = true | false
%%
-spec is_leap_year(year()) -> boolean().
is_leap_year(Y) when is_integer(Y), Y >= 0 ->
     calendar:is_leap_year(Y).



%% last_day_of_the_month(Year, Month)
%%
%% Returns the number of days in a month.
%%
-spec last_day_of_the_month(year(), month()) -> ldom().
last_day_of_the_month(Y, M) when is_integer(Y), Y >= 0 ->
     calendar:last_day_of_the_month(Y, M).



%% local_time()
%%
%% Returns: {date(), time()}, date() = {Y, M, D}, time() = {H, M, S}.
-spec local_time() -> t_datetime().
local_time() ->
     calendar:local_time().



-spec local_time_to_universal_time(t_datetime1970(),
				   'true' | 'false' | 'undefined') ->
					  t_datetime1970().
local_time_to_universal_time(DateTime, IsDst) ->
     calendar:local_time_to_universal_time(DateTime, IsDst).

-spec local_time_to_universal_time_dst(t_datetime1970()) -> [t_datetime1970()].
local_time_to_universal_time_dst(DateTime) ->
     calendar:local_time_to_universal_time_dst(DateTime).

%% now_to_universal_time(Now)
%% now_to_datetime(Now)
%%
%% Convert from now() to UTC.
%%
%% Args: Now = now(); now() = {MegaSec, Sec, MilliSec}, MegaSec = Sec
%% = MilliSec = integer() 
%% Returns: {date(), time()}, date() = {Y, M, D}, time() = {H, M, S}.
%% 
-spec now_to_datetime(t_now()) -> t_datetime1970().
now_to_datetime({MSec, Sec, _uSec}) ->
     calendar:now_to_datetime({MSec, Sec, _uSec}).


-spec now_to_universal_time(t_now()) -> t_datetime1970().
now_to_universal_time(Now) ->
     calendar:now_to_universal_time(Now).


%% now_to_local_time(Now)
%%
%% Args: Now = now()
%%
-spec now_to_local_time(t_now()) -> t_datetime1970().
now_to_local_time({MSec, Sec, _uSec}) ->
     calendar:now_to_local_time({MSec, Sec, _uSec}).


%% seconds_to_daystime(Secs) = {Days, {Hour, Minute, Second}}
%%
-spec seconds_to_daystime(integer()) -> {integer(), t_time()}.
seconds_to_daystime(Secs) ->
     calendar:seconds_to_daystime(Secs).

%%
%% seconds_to_time(Secs)
%%
%% Wraps.
%%

seconds_to_time(Secs) when Secs >= 0 ->
     calendar:seconds_to_time(Secs).

%% time_difference(T1, T2) = Tdiff
%%
%% Returns the difference between two {Date, Time} structures.
%%
%% T1 = T2 = {Date, Time}, Tdiff = {Day, {Hour, Min, Sec}}, 
%% Date = {Year, Month, Day}, Time = {Hour, Minute, Sec},
%% Year = Month = Day = Hour = Minute = Sec = integer()
%%
-type timediff() :: {integer(), t_time()}.
-spec time_difference(t_datetime(), t_datetime()) -> timediff().
time_difference({{Y1, Mo1, D1}, {H1, Mi1, S1}}, 
		{{Y2, Mo2, D2}, {H2, Mi2, S2}}) ->
     calendar:time_difference({{Y1, Mo1, D1}, {H1, Mi1, S1}}, 
		{{Y2, Mo2, D2}, {H2, Mi2, S2}}).



%%
%% time_to_seconds(Time)
%%
time_to_seconds({H, M, S}) when is_integer(H), is_integer(M), is_integer(S) ->
     calendar:time_to_seconds({H, M, S}).
      

%% universal_time()
%%
%% Returns: {date(), time()}, date() = {Y, M, D}, time() = {H, M, S}.
-spec universal_time() -> t_datetime().
universal_time() ->
     calendar:universal_time().
 

%% universal_time_to_local_time(DateTime)
%%
-spec universal_time_to_local_time(t_datetime()) -> t_datetime().
universal_time_to_local_time(DateTime) ->
     calendar:universal_time_to_local_time(DateTime).


%% valid_date(Year, Month, Day) = true | false
%% valid_date({Year, Month, Day}) = true | false
%%
-spec valid_date(integer(), integer(), integer()) -> boolean().
valid_date(Y, M, D) when is_integer(Y), is_integer(M), is_integer(D) ->
     calendar:valid_date(Y, M, D).


-spec valid_date({integer(),integer(),integer()}) -> boolean().
valid_date({Y, M, D}) ->
     calendar:valid_date(Y, M, D).

date() ->
    erlang:date().

time() ->
    erlang:time().

now() ->
    erlang:now().


%% 获得系统的当前时间
%% 返回#stime{}
localtime() ->
    {{Y, M, D}, {H, Min, S}} = erlang:localtime(),
    #stime{year=Y, month=M, day=D, hour=H, minute=Min, second=S}.

%% 生成#stime{}
make_time({{Y, M, D}, {H, Min, S}}) ->
    #stime{year=Y, month=M, day=D, hour=H, minute=Min, second=S}.       
make_time(Y, M, D, H, Min, S) ->
    #stime{year=Y, month=M, day=D, hour=H, minute=Min, second=S}.    

%% 得到两个时间的时间差, 
%% 返回秒数
diff_time(#stime{year=Y1, month=M1, day=D1, hour=H1, minute=Min1, second=S1}, 
	  #stime{year=Y2, month=M2, day=D2, hour=H2, minute=Min2, second=S2})->
    diff_time({{Y1, M1, D1}, {H1, Min1, S1}}, {{Y2, M2, D2}, {H2, Min2, S2}});
diff_time({Date1, Time1}, {Date2, Time2})->
    T1 = calendar:datetime_to_gregorian_seconds({Date1, Time1}),
    T2 = calendar:datetime_to_gregorian_seconds({Date2, Time2}),
    T2 - T1.

%% 在指定的时间的基础上增加n秒的时间
%% 返回#stime, 或者{Date, Time}
add_time(#stime{year=Y, month=M, day=D, hour=H, minute=Min, second=S}, Second) ->
    {{Y1, M1, D1}, {H1, Min1, S1}} = add_time({{Y,M,D}, {H,Min,S}}, Second),
    #stime{year=Y1, month=M1, day=D1, hour=H1, minute=Min1, second=S1};
add_time({_Date, _T}=Time, Second) ->
    Sec = datetime_to_gregorian_seconds(Time),
    gregorian_seconds_to_datetime(Sec + Second).

dec_time(#stime{year=Y, month=M, day=D, hour=H, minute=Min, second=S}, Second) ->
    {{Y1, M1, D1}, {H1, Min1, S1}} = dec_time({{Y,M,D}, {H,Min,S}}, Second),
    #stime{year=Y1, month=M1, day=D1, hour=H1, minute=Min1, second=S1};
dec_time({_Date, _T}=Time, Second) ->
    Sec = datetime_to_gregorian_seconds(Time),
    gregorian_seconds_to_datetime(Sec - Second).

is_equal(#stime{year=Y1, month=M1, day=D1}, #stime{year=Y2, month=M2, day=D2}) ->
    ((Y1 == Y2) and (M1 == M2) and (D1 == D2)).

empty() ->
    #stime{year=0, month=0, day=0, hour=0, minute=0, second=0}.
    
    

