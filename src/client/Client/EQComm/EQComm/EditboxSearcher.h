#ifndef EditboxSearcher_H
#define EditboxSearcher_H

namespace CEGUI
{
	class Window;
}

class EditboxSearcher
{
public:	
	// �жϴ��ڿɷ�����	
	static bool canInput(CEGUI::Window * win);
	 
	// ���������(�򵥵ݹ����)
	static CEGUI::Window * simpleSearchTextbox(CEGUI::Window * win);
	
	// �ݹ��������� (�Ȳ�����, �ٲ�childs, �ٲ�������)
	// ���� pSkip: ����ĳ������
	static CEGUI::Window * searchTextbox(CEGUI::Window * win, CEGUI::Window * pSkip);

	// ȡ��ǰ����Ĵ���
	static CEGUI::Window * findActivateWindow();
	
	// ���Ҳ������ı���
	static void activeTextbox();	
};

#endif