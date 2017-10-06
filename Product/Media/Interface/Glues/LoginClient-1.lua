-----------------------------------------
-- 文件说明：登陆相关逻辑处理
-----------------------------------------
local loginAuto = false

g_clientUser = ""
g_clientPwd = ""

g_WinMgr:getWindow("Login/Port"):setText("2222")
g_WinMgr:getWindow("Login/UserEdit"):activate()

if loginAuto == false then
g_WinMgr:getWindow("Login/UserEdit"):setText(EQGlobaFunc.getUser())
g_WinMgr:getWindow("Login/UserPassw"):setText(EQGlobaFunc.getPwd())
else
g_WinMgr:getWindow("Login/UserEdit"):setText("xiejun@35info.cn")
g_WinMgr:getWindow("Login/UserPassw"):setText("xiejun1")
end

g_WinMgr:getWindow("Login"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("Login/Ok"):subscribeEvent("Clicked", "LoginWindows_OnLogin")
g_WinMgr:getWindow("Login/Ok"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("Login/UserEdit"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("Login/UserPassw"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("Login/IP"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("Login/Port"):subscribeEvent("KeyDown", "LoginWindows_OnKeyEvent")
g_WinMgr:getWindow("MessageBox/OK"):subscribeEvent("Clicked", "LoginWindows_OnMessageBoxBack")

g_GameEventMgr:subscribeEvent("GAME_INIT_DONE","LoginWindows_ShowLoginWindow")
g_GameEventMgr:subscribeEvent("NPC_DIALOG","OnNpcDlg")

CEGUI.toEditbox(g_WinMgr:getWindow("Login/UserPassw")):setTextMasked(true)
--CEGUI.toEditbox(g_WinMgr:getWindow("Login/UserPassw")):setValidationString("[A-Za-z0-9]*")
-----------------------------------------
-- 设置初始界面
-----------------------------------------

function LoginWindows_ShowLoginWindow()
	--注册相关事件
	LoginWindows_RegisterGameEvent()
	--若有帐号传入则自动登入
	if g_WinMgr:getWindow("Login/UserEdit"):getText() ~= "" then
		LoginWindows_OnLogin(nil)
	else
		g_GuiSystem:setGUISheet(g_LoginWindows)
	end
end

function LoginWindows_RegisterGameEvent()
	g_GameEventMgr:subscribeEvent("VERSION_REQUEST_ING","LoginWindows_OnVersionRequest")
	g_GameEventMgr:subscribeEvent("VERSION_OK","LoginWindows_OnVersionOK")
	g_GameEventMgr:subscribeEvent("VERSION_FAILED","LoginWindows_OnVersionFail")
	g_GameEventMgr:subscribeEvent("LOGIN_OK","LoginWindows_OnOK")
end

-----------------------------------------
-- 版本验证
-----------------------------------------

function LoginWindows_OnVersionRequest()
	LoginWindows_OnShowMessageBox("版本验证......", "LoginWindows_OnMessageBoxEmpty")
end

function LoginWindows_OnVersionOK()
	g_Loger:logMessage("Version success.")
end

function LoginWindows_OnVersionFail()
	g_Loger:logError("Version fail.")
	LoginWindows_OnShowMessageBox("版本验证失败！", "LoginWindows_OnMessageBoxExit")
end

-----------------------------------------
-- 请求登录服务器
-----------------------------------------

function LoginWindows_OnKeyEvent(args)
	local win = CEGUI.toWindowEventArgs(args).window
	local wAcc = g_WinMgr:getWindow("Login/UserEdit")
	local wPw = g_WinMgr:getWindow("Login/UserPassw")
	local wIP = g_WinMgr:getWindow("Login/IP")
	local wPort = g_WinMgr:getWindow("Login/Port")
	local wOkBtn = g_WinMgr:getWindow("Login/Ok")
	local keyvalue = CEGUI.toKeyEventArgs(args).scancode 
	print(keyvalue)
	if keyvalue == 15 then   -- tab键
		if win:getName() == wAcc:getName() then
			wPw:activate()
		elseif win:getName() == wPw:getName() then
			wIP:activate()
		elseif win:getName() == wIP:getName() then
			wPort:activate()
		elseif win:getName() == wPort:getName() then
			wOkBtn:activate()
		end
	end
	if keyvalue == 28 or keyvalue == 156 then   --回车
		LoginWindows_OnLogin()
	end	
end

function LoginWindows_OnLogin(args)
	g_clientUser = g_WinMgr:getWindow("Login/UserEdit"):getText()
	g_clientPwd = g_WinMgr:getWindow("Login/UserPassw"):getText()
	local ip 		= g_WinMgr:getWindow("Login/IP"):getText()
	local port		= g_WinMgr:getWindow("Login/Port"):getText()
	
	if g_clientUser == "" then
		return
	end
	
	if ip == "" then 
		return
	end	
	if port == "" then
		return
	end	
	
	if false == EQGlobaFunc.VersionRequest(g_clientUser, g_clientPwd, ip, tonumber(port)) then
		g_Loger:logError("Create network fail.")
		LoginWindows_OnShowMessageBox("无法创建网络连接", "LoginWindows_OnMessageBoxBack")
	end
end

-----------------------------------------
-- 错误框处理
-----------------------------------------
function LoginWindows_OnMessageBoxBack(args)
	g_GuiSystem:setGUISheet(g_LoginWindows)
end

function LoginWindows_OnMessageBoxEmpty(args)
end

function LoginWindows_OnMessageBoxExit(args)
	EQGlobaFunc.ExitClient()
end

function LoginWindows_OnShowMessageBox(args,callFun)
	EQGlobaFunc.SetText("MessageBox/Text",args)
	g_WinMgr:getWindow("MessageBox/OK"):removeAllEvents()
	g_WinMgr:getWindow("MessageBox/OK"):subscribeEvent("Clicked", callFun)
	g_GuiSystem:setGUISheet(g_MessageBox)
end

-----------------------------------------
-- 登录处理
-----------------------------------------
function LoginWindows_OnFail(args)
	local errMsg = "登录失败"
	
	if 3 == args then
		errMsg = errMsg.."：密码错误！"
	elseif 4 == args then
		errMsg = errMsg.."：帐号错误！"
	elseif 5 == args then
		errMsg = errMsg.."：帐号或密码错误！"
	else
		errMsg = errMsg.."！"
	end
	
	g_Loger:logError("Login error:"..tostring(args))
	
	LoginWindows_OnShowMessageBox(errMsg, "LoginWindows_OnMessageBoxBack")
end

function LoginWindows_OnOK(args)
	g_Loger:logMessage("Login success.")
	LoginWindows_OnShowMessageBox("正在加载场景...", "LoginWindows_OnMessageBoxEmpty")
	EQGlobaFunc.RolesRequest()
end
