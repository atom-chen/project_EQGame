-----------------------------------------
-- 文件说明	：游戏脚本统一入口
-- 作者		：麦
-- 修改		:
-- 日期		：10/03/25
-----------------------------------------
g_ifpath = "../Media/Interface/"
dofile(g_ifpath.."clientLoginLoads.lua")
dofile(g_ifpath.."clientGameLoads.lua")
dofile(g_ifpath.."glues/InitialClientLogin.lua")
dofile(g_ifpath.."glues/InitialClientGame.lua")

--commLoads()
clientLoginLoads()

