#ifndef EditboxSearcher_H
#define EditboxSearcher_H

namespace CEGUI
{
	class Window;
}

class EditboxSearcher
{
public:	
	// 判断窗口可否输入	
	static bool canInput(CEGUI::Window * win);
	 
	// 查找输入框(简单递归查找)
	static CEGUI::Window * simpleSearchTextbox(CEGUI::Window * win);
	
	// 递归查找输入框 (先查自已, 再查childs, 再查其它的)
	// 参数 pSkip: 忽略某个窗口
	static CEGUI::Window * searchTextbox(CEGUI::Window * win, CEGUI::Window * pSkip);

	// 取当前激活的窗口
	static CEGUI::Window * findActivateWindow();
	
	// 查找并激活文本框
	static void activeTextbox();	
};

#endif