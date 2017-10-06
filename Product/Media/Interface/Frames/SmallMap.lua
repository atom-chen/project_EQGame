-----------------------------------------
-- 文件说明	：小地图
-- 作者		：何展然
-- 修改		：
-- 日期		：2011/4/22
-----------------------------------------

-- 变量
local m_zoomMapUI = g_WinMgr:getWindow("map/1/")	-- 缩放地图界面
local m_mapNameTxt = g_WinMgr:getWindow("map/map5/txt")	-- 地图名字
local m_miniMapUI = g_WinMgr:getWindow("minimap/map")	-- 迷你地图界面
local m_curZoom = 3	-- 当前地图缩放大小(1.原始大小,按25%放大),初始为中间档
local m_zoomCount = 5	-- 每个场景中缩放地图的个数(目前为5个)
local m_zoomMapTable = {}	-- 保存所有缩放地图的表
local m_miniMapTalbe = {}	-- 保存迷你地图的表
local m_playerPoint, m_playerName = nil, nil	-- 玩家对应于小地图上的点和名字
local m_curSceneID = 0	-- mini_map.xml表中的id
local m_curSceneW = 0	-- 当前场景宽
local m_curSceneH = 0	-- 当前场景高
--
local m_screenW = g_GuiSystem:getRenderer():getRect():getWidth()
local m_screenH = g_GuiSystem:getRenderer():getRect():getHeight()
-- 事件注册
g_WinMgr:getWindow("map/1/+"):subscribeEvent("Clicked", "SmallMap_zoomLarge")
g_WinMgr:getWindow("map/1/-"):subscribeEvent("Clicked", "SmallMap_zoomSmall")
g_WinMgr:getWindow("map/2"):subscribeEvent("Clicked", "SmallMap_openMiniMap")
g_WinMgr:getWindow("minimap/likai"):subscribeEvent("Clicked", "SmallMap_closeMiniMap")
-- 屏蔽鼠标穿透
MainUI_ShieldClick(g_miniMap)

-----------------------------------------
-- 创建地图
function SmallMap_createMap(base, zoom)
	local map = g_WinMgr:createWindow("EQHomeLook/StaticImage")
	map:setMaxSize(CEGUI.UVector2(CEGUI.UDim(0, base*zoom), CEGUI.UDim(0, base*zoom)))
	map:setSize(CEGUI.UVector2(CEGUI.UDim(0, base*zoom), CEGUI.UDim(0, base*zoom)))
	map:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
	return map
end
-----------------------------------------
-- 创建点
function SmallMap_createPoint(type)
	local point = g_WinMgr:createWindow("EQHomeLook/StaticImage")
	point:setSize(CEGUI.UVector2(CEGUI.UDim(0, 6), CEGUI.UDim(0, 6)))
	point:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
	if 0 == type then	-- NPC
		point:setProperty("Image", "set:common1 image:Person_data4")
	elseif 1 == type then	-- 玩家
		point:setProperty("Image", "set:common1 image:Person_data8")
	end
	point:setAlwaysOnTop(true)
	return point
end
-----------------------------------------
-- 创建名字
function SmallMap_createName(type, str)
	local name = g_WinMgr:createWindow("EQHomeLook/MultiLineBox2")
	EQGlobaFunc.toMultiLineBox(name:getName()):setFont("simsun-10")
	if 0 == type then	-- NPC
		EQGlobaFunc.toMultiLineBox(name:getName()):setCols("#00CC00")
	elseif 1 == type then	-- 玩家
		EQGlobaFunc.toMultiLineBox(name:getName()):setCols("#FFFFFF")
	end
	EQGlobaFunc.toMultiLineBox(name:getName()):setSide("#000000", 1)
	local width = EQGlobaFunc.toMultiLineBox(name:getName()):getFitWidth("simsun-10", str)
	EQGlobaFunc.toMultiLineBox(name:getName()):setWidth(width)
	EQGlobaFunc.toMultiLineBox(name:getName()):addElementText(str, false)
	local height = EQGlobaFunc.toMultiLineBox(name:getName()):getFormatHeight()
	EQGlobaFunc.toMultiLineBox(name:getName()):setHeight(height)
	EQGlobaFunc.toMultiLineBox(name:getName()):setPosition(0, 0)
	name:setAlwaysOnTop(true)
	return name
end
-----------------------------------------
-- 设置地图的位置
function SmallMap_setMapPos(map, sX, sY)
	if nil==map or nil==sX or nil==sY then
		g_Loger:logError("--- SmallMap.lua -> SmallMap_setMapPos() -> parameters is nil ---")
		return
	end
	local pointX = map:getWidth():asAbsolute(m_screenW) * sX	-- 点在地图中的x坐标
	local pointY = map:getHeight():asAbsolute(m_screenH) * sY	-- 点在地图中的y坐标
	local viewW = m_zoomMapUI:getWidth():asAbsolute(m_screenW)	-- 视口的宽
	local viewH = m_zoomMapUI:getHeight():asAbsolute(m_screenH)	-- 视口的高
	local x = -(pointX - viewW/2)
	local y = -(pointY - viewH/2 - 10)
	map:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
end
-----------------------------------------
-- 设置点在地图上的位置
function SmallMap_setPointPos(point, map, sX, sY)
	if nil==point or nil==map or nil==sX or nil==sY then
		g_Loger:logError("--- SmallMap.lua -> SmallMap_setPointPos() -> parameters is nil ---")
		return
	end
	map:addChildWindow(point)
	local x = map:getWidth():asAbsolute(m_screenW) * sX - 3	-- 减去自身宽度一半
	local y = map:getHeight():asAbsolute(m_screenH) * sY - 3	--  减去自身高度一半
	point:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x), CEGUI.UDim(0, y)))
end
-----------------------------------------
-- 设置点名字在地图上的位置
function SmallMap_setNamePos(name, map, sX, sY)
	if nil==name or nil==map or nil==sX or nil==sY then
		g_Loger:logError("--- SmallMap.lua -> SmallMap_setNamePos() -> parameters is nil ---")
		return
	end
	map:addChildWindow(name)
	local x = map:getWidth():asAbsolute(m_screenW) * sX - 3	-- 减去点宽的一半
	local y = map:getHeight():asAbsolute(m_screenH) * sY - 6	-- 减去点高
	local w = name:getWidth():asAbsolute(m_screenW)
	local h = name:getHeight():asAbsolute(m_screenH)
	name:setPosition(CEGUI.UVector2(CEGUI.UDim(0, x-w/2), CEGUI.UDim(0, y-h)))	
end
-----------------------------------------
-- 该场景是否有地图
function SmallMap_hasMapImage(scene_id)
	if g_logictableMgr:getMiniMapRow(scene_id) then
		return true
	end
	SmallMap_shieldMap()
	return false
end
-----------------------------------------
-- 设置地图图片
function SmallMap_setMapImage(map, scene_id)
	local data = g_logictableMgr:getMiniMapRow(scene_id)
	if nil==map or nil==data then
		-- g_Loger:logError("--- SmallMap.lua -> SmallMap_setMapImage() ---")
		return
	end
	local ctrl = EQGlobaClass.createImageCombineCtrl()
	ctrl:init(data.row, data.col, map:getName())
	for i=0, data.row*data.col-1 do
		local imageset = g_logictableMgr:getMiniMapImageset(data.scene_id, i)
		local image = g_logictableMgr:getMiniMapImage(data.scene_id, i)
		ctrl:addImage(imageset, image)
	end
end
-----------------------------------------
-- 解析位置坐标
function SmallMap_parsePos(pos)
	-- g_Loger:logMessage("--- pos2D: "..pos.." ---")
	local sem = string.find(pos, ";")
	if nil == sem then
		return nil
	end
	-- 截取相对坐标、绝对坐标字符串
	local scalePos = string.sub(pos, 1, sem-1)
	local offsetPos = string.sub(pos, sem+1, -1)
	-- 截取相对坐标的值
	local colon = string.find(scalePos, ":")
	local comma = string.find(scalePos, ",")
	if nil==colon or nil==comma then
		return nil
	end
	local scaleX = tonumber(string.sub(scalePos, colon+1, comma-1))
	local scaleY = tonumber(string.sub(scalePos, comma+1, -1))
	-- 截取绝对坐标的值
	colon = string.find(offsetPos, ":")
	comma = string.find(offsetPos, ",")
	if nil==colon or nil==comma then
		return nil
	end
	local offsetX = tonumber(string.sub(offsetPos, colon+1, comma-1))
	local offsetY = tonumber(string.sub(offsetPos, comma+1, -1))
	-- 返回解析后的坐标
	-- g_Loger:logMessage("--- scaleX: "..scaleX.." scaleY: "..scaleY.." offsetX: "..offsetX.." offsetY: "..offsetY.." ---")
	return scaleX, scaleY, offsetX, offsetY
end
-----------------------------------------
-- 打开迷你地图
function SmallMap_openMiniMap(args)
	if g_MainUIWindows:isChild(g_miniMap) then
		g_MainUIWindows:removeChildWindow(g_miniMap)
	else
		do_task_script("script4")	-- (点击小地图按钮)执行新手任务脚本4(add by hezhr)
		g_MainUIWindows:addChildWindow(g_miniMap)
	end
end
-----------------------------------------
-- 关闭迷你地图
function SmallMap_closeMiniMap(args)
	g_MainUIWindows:removeChildWindow(g_miniMap)
end
-----------------------------------------
-- 创建对应场景的缩放地图
function SmallMap_createZoomMap(scene_id)
	-- 该场景没有地图
	if nil == SmallMap_hasMapImage(scene_id) then
		return nil
	end
	-- 检查该场景是已经存在缩放地图
	for i, v in pairs(m_zoomMapTable) do
		if scene_id == v.scene_id then
			return v
		end
	end
	-- 创建缩放地图
	local zoom_map = {}
	for i=1, m_zoomCount do	-- 不同等级的缩放地图
		-- 地图
		local map = SmallMap_createMap(1024, 1+(i-1)*0.25)
		SmallMap_setMapImage(map, scene_id)
		-- 点
		local point = SmallMap_createPoint(1)
		map:addChildWindow(point)
		-- 名字
		local name = SmallMap_createName(1, g_mainplayer:getName())
		map:addChildWindow(name)
		-- 保存
		local mapItem = {}
		mapItem["map"] = map
		mapItem["point"] = point
		mapItem["name"] = name
		table.insert(zoom_map, mapItem)
	end
	-- 保存同个场景的所有缩放地图
	local scene = {}
	scene["scene_id"] = scene_id	-- 场景id
	scene["zoom_map"] = zoom_map	-- 包含所有缩放级别的地图
	table.insert(m_zoomMapTable, scene)
	return scene
end
-----------------------------------------
-- 获取对应场景的相应缩放地图
function SmallMap_getZoomMap(scene_id, zoom)
	local scene = SmallMap_createZoomMap(scene_id)
	if nil == scene then
		return nil
	end
	return scene.zoom_map[zoom]	-- 返回包含map,point,name的表
end
-----------------------------------------
-- 设置缩放地图
function SmallMap_setZoomMap(scene_id, zoom)
	SmallMap_removeAllZoomMap()
	local zoom_map = SmallMap_getZoomMap(scene_id, zoom)
	if nil == zoom_map then
		return
	end
	m_zoomMapUI:addChildWindow(zoom_map.map)
end
-----------------------------------------
-- 移除所有缩放地图
function SmallMap_removeAllZoomMap()
	for i, v in pairs(m_zoomMapTable) do
		for j, w in pairs(v.zoom_map) do
			m_zoomMapUI:removeChildWindow(w.map)
		end
	end
end
-----------------------------------------
-- 创建玩家在迷你地图上的点和名字
function SmallMap_createPlayer()
	 if nil == m_playerPoint then
		m_playerPoint = SmallMap_createPoint(1)
	 end
	 if nil == m_playerName then
		m_playerName = SmallMap_createName(1, g_mainplayer:getName())
	 end
end
-----------------------------------------
-- 获取迷你地图
function SmallMap_getMiniMap(scene_id)
	-- 该场景没有地图
	if nil == SmallMap_hasMapImage(scene_id) then
		return nil
	end
	-- 检查该场景是已经存在缩放地图
	for i, v in pairs(m_miniMapTalbe) do
		if scene_id == v.scene_id then
			return v
		end
	end
	-- 创建迷你地图
	local width = m_miniMapUI:getWidth():asAbsolute(m_screenW)
	local height = m_miniMapUI:getHeight():asAbsolute(m_screenH)
	local map = g_WinMgr:createWindow("EQHomeLook/StaticImage")
	map:setSize(CEGUI.UVector2(CEGUI.UDim(0, width), CEGUI.UDim(0, height)))
	map:setPosition(CEGUI.UVector2(CEGUI.UDim(0, 0), CEGUI.UDim(0, 0)))
	SmallMap_setMapImage(map, scene_id)
	-- 保存迷你地图
	local scene = {}
	scene["scene_id"] = scene_id	-- 场景id
	scene["map"] = map	-- 迷你地图
	table.insert(m_miniMapTalbe, scene)
	return scene
end
-----------------------------------------
-- 设置迷你地图
function SmallMap_setMiniMap(scene_id)
	SmallMap_removeMiniMap()
	local mini_map = SmallMap_getMiniMap(scene_id)
	if nil == mini_map then
		return
	end
	m_miniMapUI:addChildWindow(mini_map.map)
	m_miniMapUI:addChildWindow(m_playerPoint)
	m_miniMapUI:addChildWindow(m_playerName)
end
-----------------------------------------
-- 移除迷你地图
function SmallMap_removeMiniMap()
	if nil==m_playerPoint or nil == m_playerName then
		return
	end
	for i, v in pairs(m_miniMapTalbe) do
		m_miniMapUI:removeChildWindow(v.map)
	end
	m_miniMapUI:removeChildWindow(m_playerPoint)
	m_miniMapUI:removeChildWindow(m_playerName)
end
-----------------------------------------
-- 屏蔽地图功能
function SmallMap_shieldMap()
	if 0 == m_curSceneID then
		return
	end
	SmallMap_removeAllZoomMap()
	SmallMap_removeMiniMap()
	g_MainUIWindows:removeChildWindow(g_miniMap)
	g_WinMgr:getWindow("map/2"):disable()	-- 使地球按钮失效
	m_curSceneID = 0
end
-----------------------------------------
-- 设置地图坐标
function SmallMap_setMapCorner(scene_id)
	local beginX = EQGlobaClass.getSmallMapMgr():getSceneBeginPosX()
	local beginY = EQGlobaClass.getSmallMapMgr():getSceneBeginPosY()
	local endX = EQGlobaClass.getSmallMapMgr():getSceneEndPosX()
	local endY = EQGlobaClass.getSmallMapMgr():getSceneEndPosY()
	-- g_Loger:logMessage("--- beginX: "..beginX.." beginY: "..beginY.." endX: "..endX.." endY: "..endY.." ---")
	local leftUp = tostring(beginX..", 0, "..beginY)
	local leftDown = tostring(beginX..", 0, "..endY)
	local rightUp = tostring(endX..", 0, "..beginY)
	local rightDown = tostring(endX..", 0, "..endY)
	EQGlobaClass.getSmallMapMgr():setActualMapCorner3D(leftUp, leftDown, rightUp, rightDown)
	m_curSceneW = math.abs(beginX) + math.abs(endX)
	m_curSceneH = math.abs(beginY) + math.abs(endY)
end
-----------------------------------------
-- 第一步: 改变地图
function SmallMap_changeMap(scene_id, scene_name)
	if scene_id == m_curSceneID then	-- 同个室外场景
		return
	end
	m_curSceneID = scene_id
	SmallMap_setSceneName(scene_name, 1)
	if false == SmallMap_hasMapImage(m_curSceneID) then	-- 该室外场景没有小地图
		return
	end
	m_curZoom = 3
	SmallMap_createPlayer()
	SmallMap_setMiniMap(m_curSceneID)
	SmallMap_setZoomMap(m_curSceneID, m_curZoom)
	SmallMap_setMapCorner(m_curSceneID)
	g_WinMgr:getWindow("map/2"):enable()	-- 使地球按钮有效
end
-----------------------------------------
-- 第二步: 更新位置
function SmallMap_updatePos(playerPos)
	local sX, sY = SmallMap_parsePos(playerPos) -- 获得玩家在地图上的相对位置
	if nil==sX or nil==sY or false==SmallMap_hasMapImage(m_curSceneID) then
		-- g_Loger:logError("--- SmallMap.lua -> SmallMap_updatePos() ---")
		return
	end
	sX, sY = SmallMap_matchOffset(m_curSceneID, sX, sY)
	-- 五个缩放地图
	for i=1, m_zoomCount do
		local temp = SmallMap_getZoomMap(m_curSceneID, i)
		-- 地图
		SmallMap_setMapPos(temp.map, sX, sY)
		-- 玩家对应的点(point, map, sX, sY)
		SmallMap_setPointPos(temp.point, temp.map, sX, sY)
		-- 玩家对应的名字
		SmallMap_setNamePos(temp.name, temp.map, sX, sY)
	end
	-- 迷你地图
	SmallMap_setPointPos(m_playerPoint, m_miniMapUI, sX, sY)
	SmallMap_setNamePos(m_playerName, m_miniMapUI, sX, sY)
end
-----------------------------------------
-- 放大小地图
function SmallMap_zoomLarge(args)
	-- 设置缩放
	if m_zoomCount==m_curZoom or false==SmallMap_hasMapImage(m_curSceneID) then
		return
	end
	m_curZoom = m_curZoom + 1
	-- 设置地图
	SmallMap_setZoomMap(m_curSceneID, m_curZoom)
end
-----------------------------------------
-- 缩小小地图
function SmallMap_zoomSmall(args)
	-- 设置缩放
	if 1==m_curZoom or false==SmallMap_hasMapImage(m_curSceneID) then
		return
	end
	m_curZoom = m_curZoom - 1
	-- 设置地图
	SmallMap_setZoomMap(m_curSceneID, m_curZoom)
end
-----------------------------------------
-- 设置场景名字,flag:1.直接使用text;2.text表示账号,根据账号获取名字
function SmallMap_setSceneName(text, flag)
	local name = text
	if 1 ~= flag then
		if g_mainplayer:getPlayerAccount() == text then	-- 玩家
			name = g_mainplayer:getName()
		else	-- 好友
			name = g_FriendSys:getFriendName(text)
			if "" == name then	-- 非好友列表里的好友
				local pickObj = EQGlobaClass.getObjMgr():getPickPlayer()
				if nil ~= pickObj then
					name = pickObj:getName()
				end
			end
		end
		name = name.."的家"
	end
	FSetText(m_mapNameTxt:getName(), name)
end
-----------------------------------------
-- 修正不同地图的偏移量(这是个bug,在这里用比较挫的方法来修正)
function SmallMap_matchOffset(scene_id, sX, sY)
	-- g_Loger:logMessage("--- scene_id: "..scene_id.." sX: "..sX.." sY: "..sY.." ---")
	local x, y = sX, sY
	if 1 == scene_id then	-- 乐园第一街
		x = (m_curSceneW/(m_curSceneH/4096)*sX + 252)/4096
	elseif 2 == scene_id then	-- 幽静山谷
		x = (m_curSceneW/(m_curSceneH/4096)*sX + 111)/4096
	elseif 3 == scene_id then	-- 乐园大道
		y = (m_curSceneH/(m_curSceneW/4096)*sY + 1020)/4096
	end
	return x, y
end
-----------------------------------------
