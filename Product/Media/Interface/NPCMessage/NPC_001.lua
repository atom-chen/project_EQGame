--NPC对话模板
--NPC_001的对话模板--g_npctalk 二维数组：NPC索引，NPC信息显示模板索引
--g_npctalk["001"] 001这是NPC索引
--["002"] = "模板002",这是NPC信息显示模板索引
--服务器填写的id为1，2，10到999
--同个NPC索引下的信息模板不可重复，不同NPC的可以--信息模板可以不必按顺序写

g_npctalk["001"] = 
{
	["001"] = "护士：游戏的同时要注意身体健康哦！有什么需要我帮助的吗？",
	["002"] = "我生病了,我想看病",
	["003"] = "我想了解一下健康值和疾病的关系",
	["004"] = "没事我是来忽悠你的",
	["005"] = "护士：你的健康状况已经非常糟糕了,是否马上治疗? (该次治疗需要花费XXX金币)",
	["006"] = "是,请马上帮我治疗",
	["007"] = "不,暂时还不想治疗",
	["008"] = "护士：健康值的多少会直接影响疾病发生的几率,健康值越高,则越不容易产生疾病,反之健康值越低,就越容易产生疾病,所以要时刻关注健康值哦！",
	["009"] = "我知道了,谢谢",
	["010"] = "任务XXX, 需要购买一件“宁静致远”黑色棉布衬衣",
	["011"] = "接受任务",
	["012"] = "完成任务",
--新的npc----售货员-----
	["013"] = "售货员：欢迎光临",
	["014"] = "我想看看你们的商品",
	["015"] = "离开",
--新手教程的npc----<道具店>妙丽-----
	["016"] = "我是道具商人妙丽。我的道具店正在开业准备中，期待你的光临。",
	["017"] = "浏览帮助",
	["018"] = "查看地图",
	["019"] = "查看主界面的功能",
	["020"] = "查看鼠标功能",
	["021"] = "查看如何换装",
	["022"] = "任务：美丽的新世界",
	["023"] = "下次再来。",
	["024"] = "您就是那个新来的吧，我很早就从乐园入境管理处了解到您了。没想到您这么快就来了。",
	["025"] = "我看到任务就来了。",
	["026"] = "我正好路过。",
	["027"] = "好的，谢谢。",
	["028"] = "这里有一些提供给你的物品，请收好。",
	["029"] = "家具店店老板就在城中心的服装店里，你沿着这条路走，很快就能找到他。",
--新手教程的npc----<服装店>洛克-----
	["030"] = "上交礼券",
	["031"] = "你就是新来的~n啊，欢迎欢迎。这些是我们为每一位新来的伙伴准备的，请收好。",	
--新手教程的npc----<家具店>麦克斯韦-----	
	["032"] = "上交礼券",
	["033"] = "看上去是个很可靠的新伙伴啊。这些是属于你的东西，请拿好。",	
--npc废话----<服装店>洛克-----
	["034"] = "欢迎光临我的服装店，有什么是我可以帮你的？",
	["035"] = "再见",
--npc废话----<家具店>麦克斯韦-----	
	["036"] = "欢迎欢迎。小店物美价廉，请放心选购。",
	["037"] = "再见",
--npc废话---<整容大师>何塞
	["038"] = "你好，欢迎光临本店，本店提供整容服务，所有的服务都需要商城点付费！",
	["039"] = "开始进行整容",
	["040"] = "再见",
--npc废话---<造型师>马里尼加斯	
	["041"] = "您好，欢迎光临本店，本店提供造型改变，所有的服务都需要商城点付费！",
	["042"] = "开始造型改变",
	["043"] = "再见",
--新手教程的npc----<道具店>妙丽-----
	["044"] = "完成任务。",
	["045"] = "我这里还有服装店老板和家具店老板提供的免费礼券，你可以拿着礼券找他们领取免费的服饰和家具。",
	["046"] = "让我看看你的商品",
	["047"] = "查看如何摆放家具",
	["048"] = "预留",
	["049"] = "预留",
--活动npc----<活动专员>钱多多-----	
	["050"] = "庆祝eq乐园开测，对新创建的角色发放活动奖励，没有任务的玩家不能参加此次活动。",
	["051"] = "我要领取活动奖励。",
	["052"] = "活动奖励为25000EQ币和250EQ点。",
	["053"] = "把奖励给我吧。",
	["054"] = "你领到奖励了。",
--服装店新手任务选项-----------
	["055"] = "#FFE60000任务：新人报道。",
--家具店新手任务选项-----------
	["056"] = "#FFE60000任务：麦克斯韦的礼物。",
	["057"] = "我现在很忙，没空理你。",
}