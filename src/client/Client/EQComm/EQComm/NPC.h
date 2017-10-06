/*******************************************************************************
* 作者: 麦_Mike
* 时间: 2009-11-23
* 描述: NPC
*******************************************************************************/
#ifndef NPC_H
#define NPC_H

#include "EQMainPrerequisites.h"
#include "GameObj.h"
#include "NetPacket.h"
#include "ObjMgr.h"

class ItemPack;

class NPC : public GameObj
{
public:
	NPC()
	{	
		setType(ObjMgr::GOT_NPC);
	}
	~NPC(){}

	void setMaping(npc_map_mapping_info& map);
	std::string getName(){return mMaping.npc_name;}

	void showHeadup();
	virtual void setVisible(bool visible);
	
	void updateLook();
	void equipOn(uint slot, uint itemid);
	/**
	*/
	virtual void onMovein();
	virtual void onMoveout();
	//
	bool onClickObj();

	void saveOrient();
	void resetOrient();

	void closeDia();
	void reqNPCommand(int page_index, int command_index, uint64 unique_id);
	void reqNPCommandBymsg();
	void reqNPCommandByOpt();
	void showDiaByOpt(notify_npc_show_dialog_by_option opt);
	void showDiaByMsg(notify_npc_show_dialog_by_message msg);

	int	getOptTpltID(){return mDiaOpt.template_id;}
	void setText(std::string winname, std::string buf);
	
	npc_command& getOptCmd(int idx)
	{
		assert(idx >=0 && idx < mOptCmdSize);
		return mDiaOpt.command_list[idx];
	}

	notify_npc_show_dialog_by_message& getMsg()
	{
		return mDiaMsg;
	}

	void setClickCmdIdx(int idx){mClickIdx=idx;}

	int																mOptCmdSize;
private:
	void set_onNPCMouseCursor();

private:
	int																mClickIdx;
	npc_map_mapping_info							mMaping;
	notify_npc_show_dialog_by_option		mDiaOpt;
	notify_npc_show_dialog_by_message	mDiaMsg;
	std::string mHeadup;
	Ogre::Quaternion mOrientation;
};

#endif