--请求进入农场
function reqEnterFarm()
	--EQGlobaFunc.openFarmExe()
	EQGlobaFunc.runExe("open", "EQFarm.exe", "-FuckingAutoRun -u "..g_clientUser.." "..g_clientPwd, "", 5)
end