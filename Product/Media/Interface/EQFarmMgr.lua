-----------------------------------------
-- 文件说明	：游戏脚本统一入口
-- 作者		：麦
-- 修改		:
-- 日期		：10/03/25
-----------------------------------------
g_ifpath = "../Media/Interface/"
dofile(g_ifpath.."farmLoginLoads.lua")
dofile(g_ifpath.."farmGameLoads.lua")
dofile(g_ifpath.."glues/InitialFarmLogin.lua")
dofile(g_ifpath.."glues/InitialFarmGame.lua")

--commLoads()
farmLoginLoads()


