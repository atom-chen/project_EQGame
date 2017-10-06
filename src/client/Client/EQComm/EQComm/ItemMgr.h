/******************************************************************
* 作者: 麦_Mike
* 时间: 20010-3-29
* 描述: 游戏物品管理
*******************************************************************/
#ifndef ItemMgr_H
#define ItemMgr_H

class ItemMgr : public Ogre::Singleton<ItemMgr>
{
public:
	ItemMgr(void);
	~ItemMgr(void);

	void openUI(std::string win);
	void print(std::string msg);
	void playAnimation(std::string ani, std::string target);

private:
	void parseStr(std::string& str);
};

#endif
