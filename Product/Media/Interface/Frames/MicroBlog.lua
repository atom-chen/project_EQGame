-----------------------------------------
-- 文件说明	：微博
-- 作者		：何展然
-- 修改		：
-- 日期		：2011/4/15
-----------------------------------------

--事件注册
g_WinMgr:getWindow("map/4"):subscribeEvent("Clicked", "MicroBlog_clickPhotoBtn")
g_WinMgr:getWindow("jietu_blog__auto_closebutton__"):subscribeEvent("Clicked", "MicroBlog_closeFullScreenUI")
g_WinMgr:getWindow("jietu_blog/button1"):subscribeEvent("Clicked", "MicroBlog_fullUpload")
g_WinMgr:getWindow("jietu_blog/button2"):subscribeEvent("Clicked", "MicroBlog_closeFullScreenUI")
g_WinMgr:getWindow("yanzheng-qq-blog__auto_closebutton__"):subscribeEvent("Clicked", "MicroBlog_clickVerifierCancel")
g_WinMgr:getWindow("yanzheng-qq-blog/yes"):subscribeEvent("Clicked", "MicroBlog_clickVerifierOK")
g_WinMgr:getWindow("yanzheng-qq-blog/no"):subscribeEvent("Clicked", "MicroBlog_clickVerifierCancel")
g_WinMgr:getWindow("map/paizhao/quanjing"):subscribeEvent("Clicked", "MicroBlog_fullScreenShot")
g_WinMgr:getWindow("map/paizhao/jinshen"):subscribeEvent("Clicked", "MicroBlog_partScreenShot")
g_WinMgr:getWindow("zhaoxiangji__auto_closebutton__"):subscribeEvent("Clicked", "MicroBlog_clickCloseCameraUI")
g_WinMgr:getWindow("zhaoxiangji/bottom/paizhao"):subscribeEvent("Clicked", "MicroBlog_takePhoto")
g_WinMgr:getWindow("updatebtn/update"):subscribeEvent("Clicked", "MicroBlog_partUpload")
g_WinMgr:getWindow("zxliuyan/done"):subscribeEvent("Clicked", "MicroBlog_liuyanOk")
g_WinMgr:getWindow("zxliuyan/cancel"):subscribeEvent("Clicked", "MicroBlog_liuyanCancel")
g_cameraUI:subscribeEvent("Moved", "MicroBlog_cameraMove")
--鼠标穿透屏蔽
MainUI_ShieldClick(g_jietuBlogUI)
MainUI_ShieldClick(g_blogVerifierUI)
MainUI_ShieldClick(g_cameraUploadUI)
MainUI_ShieldClick(g_WinMgr:getWindow("zhaoxiangji/bottom"))
local m_jietuText = g_WinMgr:getWindow("jietu_blog/edittext")
local m_jietuTextContent = m_jietuText:getText()
local m_liuyanText = g_WinMgr:getWindow("zxliuyan/text")
local m_liuyanTextContent = m_liuyanText:getText()
--微博类型
local m_type = {qq=1, sina=2}
--发布操作类型
local m_sendType = 0
--截图的完整名称(含路径和后缀)
local m_imageFullName = ""
--截图的名称(不含后缀)
local m_imageName = ""
--上传结果类型
local m_result = {ok=0, timeout=1, cantresolve=2, cantconnect=3, err=4, twolarge=5, fail=6}
--限制最多输入140个字
CEGUI.toMultiLineEditbox(m_jietuText):setMaxTextLength(140)
--保存截图的目录路径
local m_dirPath = nil
--拍照选择界面
local m_photoSelectUI = g_WinMgr:getWindow("map/paizhao")
m_photoSelectUI:setVisible(false)
--照相机大小
local m_cameraSize = 1	-- 初始大小为中
local m_x, m_y, m_w, m_h = 0, 0, 0, 0
--拍照闪烁
local m_twinkleStartTime = 0
local m_twinkleStartFlag = false
--全屏或区域截图标识
local m_fullOrPart = 0	-- (0.全屏,1.区域)
--其他需要disable的按钮
local m_otherButtonTable = {}
-----------------------------------------
-- 相关初始
function MicroBlog_init()
	local win = g_WinMgr:getWindow("MicroBlog/fullScreen/OpenImageFile")
	if nil == win then
		win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "MicroBlog/fullScreen/OpenImageFile")
		EQGlobaFunc.toMultiLineBox(win:getName()):setSize(255, 20)
		EQGlobaFunc.toMultiLineBox(win:getName()):setPosition(25, 30)
		EQGlobaFunc.toMultiLineBox(win:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(win:getName()):setCols("#00FFFC")
		EQGlobaFunc.toMultiLineBox(win:getName()):setAlign("AT_CENTER_VER")
		EQGlobaFunc.toMultiLineBox(win:getName()):addElementText("拍照成功，", false)
		EQGlobaFunc.toMultiLineBox(win:getName()):addElementHyperLink("打开文件夹", "", false)
		win:subscribeEvent("HyperLink_MouseClick", "MicroBlog_openImageFile")
		g_WinMgr:getWindow("jietu_blog"):addChildWindow(win)
	end
	win = g_WinMgr:getWindow("MicroBlog/partScreen/OpenImageFile")
	if nil == win then
		win = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2", "MicroBlog/partScreen/OpenImageFile")
		EQGlobaFunc.toMultiLineBox(win:getName()):setSize(90, 20)
		EQGlobaFunc.toMultiLineBox(win:getName()):setPosition(10, 28)
		EQGlobaFunc.toMultiLineBox(win:getName()):setFont("simsun-10")
		EQGlobaFunc.toMultiLineBox(win:getName()):setCols("#00FFFC")
		EQGlobaFunc.toMultiLineBox(win:getName()):setAlign("AT_CENTER_VER")
		EQGlobaFunc.toMultiLineBox(win:getName()):addElementHyperLink("打开文件夹", "", false)
		win:subscribeEvent("HyperLink_MouseClick", "MicroBlog_openImageFile")
		g_cameraUploadUI:addChildWindow(win)
	end
	--设置资源组目录
	m_dirPath = EQGlobaFunc.GetGameUserDir().."Screenshots/"
	EQGlobaClass.getDynamicCreateImagesetMgr():setResourceGroupDirectory(m_dirPath, "FileSystem", "imagesets")
end
-----------------------------------------
-- 打开保存图片的文件夹
function MicroBlog_openImageFile()
	EQGlobaFunc.OpenDir(m_dirPath)
end
MicroBlog_init()
-----------------------------------------
-- 点击照相机按钮
function MicroBlog_clickPhotoBtn(args)
	if m_photoSelectUI:isVisible() then
		MicroBlog_setPhotoSelectUI(false)
	else
		MicroBlog_setPhotoSelectUI(true)
	end
end
-----------------------------------------
-- 设置拍照选择界面
function MicroBlog_setPhotoSelectUI(open)
	m_photoSelectUI:setVisible(open)
end
-----------------------------------------
-- 选择全屏截图
function MicroBlog_fullScreenShot(args)
	PlaySpecialVoice(g_special_voice_map.screenshot)
	MicroBlog_setPhotoSelectUI(false)
	MicroBlog_playTwinkle(g_MainUIWindows, 0)
end
-----------------------------------------
-- 全屏截图
function MicroBlog_full()
	m_imageFullName = EQGlobaClass.getMicroBlogMgr():fullScreenShot(m_dirPath)
	m_imageName = MicroBlog_saveScreenShotImage(m_imageFullName)
	if nil == m_imageName then
		return
	end
	EQGlobaClass.getDynamicCreateImagesetMgr():setWindowImage("jietu_blog/picture", m_imageName)
	g_MainUIWindows:addChildWindow(g_jietuBlogUI)	-- 打开界面
	Common_setModalState(g_jietuBlogUI:getName(), true)
	EQGlobaFunc.SetText(m_jietuText:getName(), m_jietuTextContent)
end
-----------------------------------------
-- 选择部分截图
function MicroBlog_partScreenShot(args)
	OpenCameraUI()
	UIM_DoOpenAndCloseUI(g_CameraUINames.root)
	MicroBlog_setOtherButtonEnabled(false)
end
-----------------------------------------
-- 打开照相机界面(用于界面互斥)
function OpenCameraUI()
	UIM_OpenWindow(g_CameraUINames.root)	-- 放在最上面
	MicroBlog_openCameraUI()
	MicroBlog_setTakePhotoState(true)
	MicroBlog_fixedCameraPosition()
	MicroBlog_fixedCameraAngle()
	MicroBlog_setPhotoSelectUI(false)
end
-----------------------------------------
-- 关闭照相机界面(用于界面互斥)
function CloseCameraUI()
	MicroBlog_closeCameraUI()
	UIM_CloseWindow(g_CameraUINames.root)
end
-----------------------------------------
-- 部分截图
function MicroBlog_part()
	m_imageFullName = EQGlobaClass.getMicroBlogMgr():partScreenShot(m_x, m_y, m_w, m_h, m_dirPath)
	m_imageName = MicroBlog_saveScreenShotImage(m_imageFullName)
	if nil == m_imageName then
		return
	end
	EQGlobaClass.getDynamicCreateImagesetMgr():setWindowImage("zhaoxiangji/image", m_imageName)
	MicroBlog_openUploadSelectUI(true)
end
-----------------------------------------
-- 保存截图
function MicroBlog_saveScreenShotImage(imageFullName)
	local imageName, imageExt = GetFileName(imageFullName)
	local ret = EQGlobaClass.getDynamicCreateImagesetMgr():saveImageEx(imageName, imageName.."."..imageExt) -- 保存图片
	if 0 ~= ret then
		g_Loger:logError("Screenshots failed.")
		return nil
	end
	return imageName
end
-----------------------------------------
-- 设置其他按钮有效性
function MicroBlog_setOtherButtonEnabled(enable)
	if enable then
		for i, v in pairs(m_otherButtonTable) do
			if nil ~= g_WinMgr:getWindow(v) then
				g_WinMgr:getWindow(v):setEnabled(true)
			end
		end
	else
		m_otherButtonTable = {}
		for i=0, g_MainUIWindows:getChildCount()-1 do
			local name = g_MainUIWindows:getChildAtIdx(i):getName()
			if "zhaoxiangji" ~= name and "updatebtn"~=name and "MainRoot/SImg_HMenubar"~=name then
				table.insert(m_otherButtonTable, name)
				g_WinMgr:getWindow(name):setEnabled(false)
			end
		end
		for i=0, g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):getChildCount()-1 do
			local name = g_WinMgr:getWindow("MainRoot/SImg_HMenubar"):getChildAtIdx(i):getName()
			if "MainRoot/HMenubar/Btn_Action" ~= name then
				table.insert(m_otherButtonTable, name)
				g_WinMgr:getWindow(name):setEnabled(false)
			end
		end
		g_ActionUI:setEnabled(true)
	end
end
-----------------------------------------
-- 打开拍照界面
function MicroBlog_openCameraUI()
	g_MainUIWindows:addChildWindow(g_cameraUI)
	MicroBlog_changeCameraSize(m_cameraSize)
	MicroBlog_openUploadSelectUI(false)
end
-----------------------------------------
-- 点击关闭拍照界面
function MicroBlog_clickCloseCameraUI(args)
	CloseCameraUI()
end
-----------------------------------------
-- 关闭拍照界面
function MicroBlog_closeCameraUI()
	g_MainUIWindows:removeChildWindow(g_cameraUI)
	MicroBlog_openUploadSelectUI(false)
	EQGlobaClass.getDynamicCreateImagesetMgr():destroyImageset(m_imageName)
	g_WinMgr:getWindow("zhaoxiangji/image"):setProperty("Image", "set: image:")
	g_WinMgr:getWindow("zhaoxiangji/image/jiaodian"):setVisible(true)
	MicroBlog_setOtherButtonEnabled(true)
end
-----------------------------------------
-- 照相机大小配置
function MicroBlog_cameraSizeConfig()
	local sizeCfg = CEGUI.toCombobox(g_WinMgr:getWindow("zhaoxiangji/bottom/daxiao"))
	EQGlobaFunc.createComboBoxTextItem(sizeCfg:getName(), "小", 0,"")
	EQGlobaFunc.createComboBoxTextItem(sizeCfg:getName(), "中", 1,"")
	EQGlobaFunc.createComboBoxTextItem(sizeCfg:getName(), "大", 2,"")
	sizeCfg:setReadOnly(true)
	sizeCfg:getDropList():subscribeEvent("ListSelectionAccepted", "MicroBlog_selectCameraSize")
	sizeCfg:setItemSelectState(m_cameraSize, true)
end
-----------------------------------------
-- 选择照相机大小
function MicroBlog_selectCameraSize(args)
	local listbox = CEGUI.toListbox(CEGUI.toWindowEventArgs(args).window)
	local listitem = listbox:getFirstSelectedItem()
	if nil == listitem then
		return
	end
	local id = listitem:getID()
	if id == m_cameraSize then
		return
	else
		m_cameraSize = id
	end
	MicroBlog_changeCameraSize(m_cameraSize)
end
-----------------------------------------
-- 改变照相机大小
function MicroBlog_changeCameraSize(size)
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	if 0 == size then		-- 小(1/3)
		MicroBlog_setCameraSize(screenH/3, screenH/3)
	elseif 1 == size then	-- 中(1/2)
		MicroBlog_setCameraSize(screenH/2, screenH/2)
	elseif 2 == size then	-- 大(2/3)
		MicroBlog_setCameraSize((screenH*2)/3, (screenH*2)/3)
	end
end
MicroBlog_cameraSizeConfig()
-----------------------------------------
-- 设置照相机大小
function MicroBlog_setCameraSize(w, h)
	g_cameraUI:setSize(CEGUI.UVector2(CEGUI.UDim(0, 6+w+6), CEGUI.UDim(0, 26+h+60)))
	g_WinMgr:getWindow("zhaoxiangji/image"):setSize(CEGUI.UVector2(CEGUI.UDim(0, w), CEGUI.UDim(0, h)))
	MicroBlog_fixedCameraPosition()
end
-----------------------------------------
-- 固定照相机位置
function MicroBlog_fixedCameraPosition()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local width = g_cameraUI:getWidth():asAbsolute(screenW)
	local height = g_cameraUI:getHeight():asAbsolute(screenH)
	local x = (screenW - width)/2
	local y = (screenH - height)/2
	g_cameraUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
	MicroBlog_autoUploadSelectUIPosition()
end
-----------------------------------------
-- 固定照相机角度
function MicroBlog_fixedCameraAngle()
	local camera = EQGlobaFunc.getMainCamera()
	-- 获取玩家的本地空间角度
	local localYaw = EQGlobaClass.getMainPlayer():getYaw()
	if localYaw < 0 then
		localYaw = 360 + localYaw
	end
	-- 获取玩家的世界空间角度
	local worldYaw = localYaw
	if worldYaw>=90 and worldYaw<=360 then
		worldYaw = worldYaw - 90
	else
		worldYaw = worldYaw + 270
	end
	-- 设置摄像机角度
	camera:pitch(5)			-- 摄像机窗口上下移动
	camera:yaw(worldYaw)	-- 摄像机窗口左右移动
	camera:zoom(210)		-- 缩放
end
-----------------------------------------
-- 照相机移动
function MicroBlog_cameraMove(args)
	MicroBlog_autoUploadSelectUIPosition()
end
-----------------------------------------
-- 设置拍照状态,true:拍照,false:准备重新拍照
function MicroBlog_setTakePhotoState(state)
	if state then
		g_WinMgr:getWindow("zhaoxiangji/bottom/paizhao"):setID(1)
		EQGlobaFunc.SetText("zhaoxiangji/bottom/paizhao", "拍照")
		g_WinMgr:getWindow("zhaoxiangji/bottom/daxiao"):enable()
	else
		g_WinMgr:getWindow("zhaoxiangji/bottom/paizhao"):setID(0)
		EQGlobaFunc.SetText("zhaoxiangji/bottom/paizhao", "重新拍照")
		g_WinMgr:getWindow("zhaoxiangji/bottom/daxiao"):disable()
	end
end
-----------------------------------------
-- 获取拍照状态
function MicroBlog_getTakePhotoState()
	if 0 == g_WinMgr:getWindow("zhaoxiangji/bottom/paizhao"):getID() then
		return false	-- 准备重新拍照
	end
	return true	-- 拍照
end
-----------------------------------------
-- 获取拍照的区域
function MicroBlog_getPhotoArea()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local parentPos = {x, y}
	parentPos = g_cameraUI:getPosition()
	local parentW = g_cameraUI:getWidth():asAbsolute(screenW)
	local parentH = g_cameraUI:getHeight():asAbsolute(screenH)
	--
	local child = g_WinMgr:getWindow("zhaoxiangji/image")
	local childPos = {x, y}
	childPos = child:getPosition()
	local w = child:getWidth():asAbsolute(parentW)
	local h = child:getHeight():asAbsolute(parentH)
	--
	local x = parentPos.x:asAbsolute(screenW) + childPos.x:asAbsolute(screenW)
	local y = parentPos.y:asAbsolute(screenH) + childPos.y:asAbsolute(screenH)
	--
	return x, y, w, h
end
-----------------------------------------
-- 判断拍照的区域是否有效
function MicroBlog_checkPhotoArea(x, y, w, h)
	local maxW = g_GuiSystem:getRenderer():getRect():getWidth()
	local maxH = g_GuiSystem:getRenderer():getRect():getHeight()
	if x<0 or x>maxW or y<0 or y>maxH then
		return false
	end
	if (x+w)>maxW or (y+h)>maxH then
		return false
	end
	return true
end
-----------------------------------------
-- 播放闪烁
function MicroBlog_playTwinkle(pareWin, fullOrPart)
	if nil == pareWin then
		return
	end
	--
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	local w = pareWin:getWidth():asAbsolute(screenW)
	local h = pareWin:getHeight():asAbsolute(screenH)
	local twinkle = g_WinMgr:createWindow("EQHomeLook/StaticImage", "zhaoxiangji/twinkleWin")
	if nil == twinkle then
		return
	end
	twinkle:setSize(CEGUI.UVector2(CEGUI.UDim(0, w), CEGUI.UDim(0, h)))
	twinkle:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
	twinkle:setProperty("Image", "set:windows3 image:window5_mid")
	pareWin:addChildWindow(twinkle)
	--
	m_twinkleStartTime = EQGlobaFunc.GetTickCountByLua()
	m_twinkleStartFlag = true
	m_fullOrPart = fullOrPart
end
-----------------------------------------
-- 控制闪烁
function MicroBlog_twinkle()
	if m_twinkleStartFlag then
		-- 控制闪烁时间
		if (EQGlobaFunc.GetTickCountByLua()-m_twinkleStartTime)>=100 then
			m_twinkleStartFlag = false
			g_WinMgr:destroyWindow("zhaoxiangji/twinkleWin")
			if 0 == m_fullOrPart then	-- 全屏截图
				MicroBlog_full()
			elseif 1 == m_fullOrPart then	-- 区域截图
				MicroBlog_part()
			end
		end
	end
end
-----------------------------------------
-- 点击拍照按钮
function MicroBlog_takePhoto(args)
	if MicroBlog_getTakePhotoState() then	-- 拍照
		m_x, m_y, m_w, m_h = MicroBlog_getPhotoArea()
		if MicroBlog_checkPhotoArea(m_x, m_y, m_w, m_h) then	-- 此拍照区域有效
			MicroBlog_setTakePhotoState(false)
			g_WinMgr:getWindow("zhaoxiangji/image/jiaodian"):setVisible(false)
			PlaySpecialVoice(g_special_voice_map.screenshot)
			MicroBlog_playTwinkle(g_WinMgr:getWindow("zhaoxiangji/image"), 1)
		else	-- 此拍照区域无效
			MsgBox_createMsgBoxExA(g_MainUIWindows, 200, "拍照提示", "无法在此区域拍照。", {}, "MsgBoxEx_close", "")
		end
	else	-- 准备重新拍照
		MicroBlog_setTakePhotoState(true)
		g_WinMgr:getWindow("zhaoxiangji/image/jiaodian"):setVisible(true)
		g_WinMgr:getWindow("zhaoxiangji/image"):setProperty("Image", "set: image:")
		EQGlobaClass.getDynamicCreateImagesetMgr():destroyImageset(m_imageName)
		MicroBlog_openUploadSelectUI(false)
	end
end
-----------------------------------------
-- 打开/关闭上传选择界面
function MicroBlog_openUploadSelectUI(open)
	-- 打开或关闭界面
	if open then
		g_MainUIWindows:addChildWindow(g_cameraUploadUI)
		MicroBlog_autoUploadSelectUIPosition()
		-- 重置选择状态
		CEGUI.toCheckbox(g_WinMgr:getWindow("updatebtn/weibo")):setSelected(true)
		CEGUI.toCheckbox(g_WinMgr:getWindow("updatebtn/tx")):setSelected(true)
	else
		g_MainUIWindows:removeChildWindow(g_cameraUploadUI)
	end
end
-----------------------------------------
-- 自动调整上传选择界面位置
function MicroBlog_autoUploadSelectUIPosition()
	local screenW = g_GuiSystem:getRenderer():getRect():getWidth()
	local screenH = g_GuiSystem:getRenderer():getRect():getHeight()
	--
	local w1 = g_cameraUI:getWidth():asAbsolute(screenW)
	local h1 = g_cameraUI:getHeight():asAbsolute(screenH)
	local pos = {x, y}
	pos = g_cameraUI:getPosition()
	--
	local w2 = g_cameraUploadUI:getWidth():asAbsolute(screenW)
	local h2 = g_cameraUploadUI:getHeight():asAbsolute(screenH)
	--
	local x, y = 0, pos.y:asAbsolute(screenH) + h1 - h2
	--
	if (pos.x:asAbsolute(screenW)+w1+w2) > screenW then
		x = pos.x:asAbsolute(screenW) - w2
	else
		x = pos.x:asAbsolute(screenW) + w1
	end
	--
	g_cameraUploadUI:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
end
-----------------------------------------
-- 点击部分截屏上传按钮
function MicroBlog_partUpload(args)
	local cbWeibo = CEGUI.toCheckbox(g_WinMgr:getWindow("updatebtn/weibo"))
	local cbBg = CEGUI.toCheckbox(g_WinMgr:getWindow("updatebtn/tx"))
	if false==cbWeibo:isSelected() and false==cbBg:isSelected() then	-- 没有选择
		MsgBox_createMsgBoxExA(g_MainUIWindows, 200, "上传提示", "请至少选择一种上传类型。", {}, "MsgBoxEx_close", "")
		return
	end
	CloseCameraUI()
	if false==cbWeibo:isSelected() and cbBg:isSelected() then	-- 设为微博头像
		EQGlobaClass.getMicroBlogMgr():SendTo(m_type.qq, 2, "", m_imageFullName)
		return
	end
	g_MainUIWindows:addChildWindow(g_cameraLiuyanUI)
	Common_setModalState(g_cameraLiuyanUI:getName(), true)
	EQGlobaFunc.SetText(m_liuyanText:getName(), m_liuyanTextContent)
	if cbWeibo:isSelected() and false==cbBg:isSelected() then	-- 上传到QQ微博
		m_sendType = 1
		return
	end
	if cbWeibo:isSelected() and cbBg:isSelected() then	-- 上传到QQ微博,设为微博头像
		m_sendType = 3
		return
	end
end
-----------------------------------------
-- 点击照相留言界面确定按钮
function MicroBlog_liuyanOk(args)
	g_MainUIWindows:removeChildWindow(g_cameraLiuyanUI)
	Common_setModalState(g_cameraLiuyanUI:getName(), false)
	EQGlobaClass.getMicroBlogMgr():SendTo(m_type.qq, m_sendType, EQGlobaFunc.UTF8ToGB2312Bylua(m_liuyanText:getText()), m_imageFullName)
end
-----------------------------------------
-- 点击照相留言界面取消按钮
function MicroBlog_liuyanCancel(args)
	g_MainUIWindows:removeChildWindow(g_cameraLiuyanUI)
	Common_setModalState(g_cameraLiuyanUI:getName(), false)
end
-----------------------------------------
-- 关闭全屏截图所用界面
function MicroBlog_closeFullScreenUI(args)
	EQGlobaClass.getDynamicCreateImagesetMgr():destroyImageset(m_imageName)
	g_MainUIWindows:removeChildWindow(g_jietuBlogUI)	-- 关闭界面
	Common_setModalState(g_jietuBlogUI:getName(), false)
end
-----------------------------------------
-- 点击全屏截屏上传按钮
function MicroBlog_fullUpload(args)
	EQGlobaClass.getDynamicCreateImagesetMgr():destroyImageset(m_imageName)
	g_MainUIWindows:removeChildWindow(g_jietuBlogUI)	-- 关闭界面
	Common_setModalState(g_jietuBlogUI:getName(), false)
	EQGlobaClass.getMicroBlogMgr():SendTo(m_type.qq, 1, EQGlobaFunc.UTF8ToGB2312Bylua(m_jietuText:getText()), m_imageFullName)
end
-----------------------------------------
-- 弹出第一次注册提示
function MicroBlog_showFirstUseTip()
	local tip = "<sp/><sp/><sp/>请在点击确定后弹出的网页中进行<clr v=\"#FF0000\">授权</clr>，并记下<clr v=\"#FF0000\">授权码</clr>。"
	MsgBox_createMsgBoxExA(g_MainUIWindows, 250, "微博发布提示", tip, {}, "MicroBlog_okFisrtUse", "")
end
-----------------------------------------
-- 点击注册提示框确定按钮
function MicroBlog_okFisrtUse(args)
	EQGlobaClass.getMicroBlogMgr():getTokenOfQQ()
	MicroBlog_showVerifierBox()
end
-----------------------------------------
-- 弹出授权码框让玩家输入verifier
function MicroBlog_showVerifierBox()
	g_MainUIWindows:addChildWindow(g_blogVerifierUI)
	Common_setModalState(g_blogVerifierUI:getName(), true)
	EQGlobaFunc.SetText("yanzheng-qq-blog/inputbox", "")
end
-----------------------------------------
-- 点击授权码框确定按钮
function MicroBlog_clickVerifierOK(args)
	g_MainUIWindows:removeChildWindow(g_blogVerifierUI)
	Common_setModalState(g_blogVerifierUI:getName(), false)
	local verifer = g_WinMgr:getWindow("yanzheng-qq-blog/inputbox"):getText()
	EQGlobaClass.getMicroBlogMgr():saveAccessSecret(m_type.qq, verifer)
end
-----------------------------------------
-- 点击授权码框取消按钮
function MicroBlog_clickVerifierCancel(args)
	g_MainUIWindows:removeChildWindow(g_blogVerifierUI)
	Common_setModalState(g_blogVerifierUI:getName(), false)
end
-----------------------------------------
-- 弹出上传结果框
function MicroBlog_showResult(eHttpRetCode, pData)
	local ret = tonumber(string.sub(pData, -2, -2))
	if m_result.ok == eHttpRetCode then
		if 0 == ret then
			local msg = MicroBlog_cutResult(pData, "\"msg\":\"")
			if "ok" == msg then
				MicroBlog_setResultBox("发布成功。")
			else
				MicroBlog_setResultBox(msg)
			end
		elseif 1 == ret then
			MicroBlog_setResultBox("参数失败。")
		elseif 2 == ret then
			MicroBlog_setResultBox("频率受限。")
		elseif 3 == ret then
			MicroBlog_setResultBox("签权失败。")
		elseif 4 == ret then
			MicroBlog_setResultBox("服务器内部错误。")
		end
	elseif m_result.timeout == eHttpRetCode then
		MicroBlog_setResultBox("请求超时。")
	elseif m_result.cantresolve == eHttpRetCode then
		MicroBlog_setResultBox("不能解析地址。")
	elseif m_result.cantconnect == eHttpRetCode then
		MicroBlog_setResultBox("不能连接。")
	elseif m_result.err == eHttpRetCode then
		MicroBlog_setResultBox("HTTP请求的数据错误。")
	elseif m_result.twolarge == eHttpRetCode then
		MicroBlog_setResultBox("返回的数据量太大。")
	elseif m_result.fail == eHttpRetCode then
		MicroBlog_setResultBox("发布失败。")
	end
end
-----------------------------------------
-- 截取上传返回消息
function MicroBlog_cutResult(pData, sep)
	local s, e = string.find(pData, sep)
	if nil == s then
		return ""
	end
	local str = string.sub(pData, e+1, -1)
	local s1, e1 = string.find(str, "\"")
	if nil == s1 then
		return ""
	end
	local ret = string.sub(str, 1, e1-1)
	return ret
end
-----------------------------------------
-- 设置上传结果框
function MicroBlog_setResultBox(text)
	MsgBox_createMsgBoxExA(g_MainUIWindows, 200, "微博发布提示", text, {}, "MsgBoxEx_close", "")
end
