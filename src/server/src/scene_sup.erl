%% 鍦烘櫙鐨勭洃鎺ф爲
%% 鐢ㄤ簬鐩戞帶鍦烘櫙
%%
-module(scene_sup).  
  
-behaviour(supervisor).  
  
-export([init/1]).  
  

%% 初始化时不创建场景子进程，具体场景动态加上去
init([]) ->
    {ok,
        {  {one_for_one, 10, 100},
            [ 
            ]
        }
    }.
