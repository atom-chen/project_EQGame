%% 消息的定义
-record(msg, {event,  %% 消息事件
	      src,    %% 消息源
	      target  %% 目标
	     }).
