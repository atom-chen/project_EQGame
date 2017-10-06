/*==============================================================================
	EQ GAME 
	Copyright (c) 2009 Mr.Q All rights reserved.

	filename:   CommonHelper.cpp
	created:    Mr.Q 2009-11-15 0:07
	purpose:    Í¨ÓÃº¯Êý
==============================================================================*/
#include "EQCommPCH.h"
//#include "ClientShell.h"
#include "CommonHelper.h"

#include "EQGameMgr.h"
#include "EQCamera.h"
#include "NetPacket.h"
/*==============================================================================
	purpose: CommonHelper
	created: Mr.Q 2009-11-15 0:07
  =============================================================================*/
//Ogre::Ray CommonHelper::ProjectionLine(Ogre::Real x, Ogre::Real y)
//{
	//Ogre::Camera* pCamera = CClientShell::getSingleton().GetCamera();

	//if (!pCamera)
	//	return Ogre::Ray();

	//return pCamera->getCameraToViewportRay
	//	(
	//		((x - pCamera->getViewport()->getActualLeft()) / pCamera->getViewport()->getActualWidth()),
	//		((y - pCamera->getViewport()->getActualTop()) / pCamera->getViewport()->getActualHeight())
	//	);
//}

//Ogre::Vector3 CommonHelper::ScreenPointTo3D(const Ogre::Vector2 &point)
//{
//    Ogre::Vector2 pos;
//
//    pos.x = (point.x / Ogre::Real(CClientShell::getSingleton().GetRenderWindow()->getWidth()));
//    pos.y = (point.y / Ogre::Real(CClientShell::getSingleton().GetRenderWindow()->getHeight()));
//
//    Ogre::Vector3 aPoint3;
//
//    Ogre::Camera* pCamera = CClientShell::getSingleton().GetCamera();
//
//    if (!pCamera)
//        return aPoint3;
//
//    Ogre::Ray ray = pCamera->getCameraToViewportRay(pos.x, pos.y);
//
//     Ogre::Plane backPlane(Ogre::Vector3::UNIT_Y, 0);
//    /*Ogre::Plane backPlane(ray.getDirection(), 0);*/
//
//    std::pair<bool, Ogre::Real> result = ray.intersects(backPlane);
//
//    Ogre::Vector3 point3 = ray.getPoint(result.second);
//
//    return point3;
//}

bool CommonHelper::Vector3ToScreenPoint(const Ogre::Vector3& pos, int& x, int& y)
{
	bool inScreen=true;

	Ogre::Camera* camera = EQGameMgr::getSingleton().getMainCamera()->getOgreCam();
	if(!camera->isVisible(pos))
		inScreen = false;

	Ogre::Vector3 offset = camera->getProjectionMatrix() * (camera->getViewMatrix() * pos); 

	if((offset.x < -1.0f) || (offset.x > 1.0f) || (offset.y < -1.0f) || (offset.y > 1.0f)) 
		inScreen = false;

	int width = camera->getViewport()->getActualWidth();
	x = width/2 + (int)((float)(width/2) * offset.x); 

	int height = camera->getViewport()->getActualHeight();
	y = height/2 - (int)((float)(height/2) * offset.y); 

	return inScreen;
}

PWCHAR CommonHelper::mbcs_to_utf16(PCHAR src)
{
	int length = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, 0);
	PWCHAR ws = (PWCHAR)malloc(length * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, src, -1, ws, length);
	return ws;
}

PCHAR CommonHelper::utf16_to_mbcs(PWCHAR src)
{
	int length = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
	PCHAR as = (PCHAR)malloc(length * sizeof(CHAR));
	WideCharToMultiByte(CP_ACP, 0, src, -1, as, length, NULL, NULL);
	return as;
}

PWCHAR CommonHelper::utf8_to_utf16(PCHAR src)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	PWCHAR ws = (PWCHAR)malloc(length * sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, src, -1, ws, length);
	return ws;
}

PCHAR CommonHelper::utf16_to_utf8(PWCHAR src)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
	PCHAR as = (PCHAR)malloc(length * sizeof(CHAR));
	WideCharToMultiByte(CP_UTF8, 0, src, -1, as, length, NULL, NULL);
	return as;
}

PCHAR CommonHelper::mbcs_to_utf8(PCHAR src)
{
	PWCHAR wsrc = mbcs_to_utf16(src);
	PCHAR as = utf16_to_utf8(wsrc);
	free(wsrc);
	return as;
}

PCHAR CommonHelper::utf8_to_mbcs(PCHAR src)
{
	PWCHAR wsrc = utf8_to_utf16(src);
	PCHAR as = utf16_to_mbcs(wsrc);
	free(wsrc);
	return as;
}

void CommonHelper::translate(struct tm *when, stime *date)
{
	when->tm_year = date->year - 1900;
	when->tm_mon = date->month - 1;
	when->tm_mday = date->day;
	when->tm_hour = date->hour;
	when->tm_min = date->minute;
	when->tm_sec = date->second;
}

int CommonHelper::diffTime(stime *t1, stime *t2)
{
	struct tm  when, now;

	translate(&when, t1);
	translate(&now, t2);

	return mktime(&now) - mktime(&when);
}
