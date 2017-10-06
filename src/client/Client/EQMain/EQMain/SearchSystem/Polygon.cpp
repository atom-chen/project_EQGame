#include "Polygon.h"



namespace SearchSystem
{
	/**
	* r=multiply(sp,ep,op),得到(sp-op)*(ep-op)的叉积 
	* r>0:ep在矢量opsp的逆时针方向； 
	* r=0：opspep三点共线； 
	* r<0:ep在矢量opsp的顺时针方向 
	* @param sp 
	* @param ep 
	* @param op 
	* @return 
	*/		
	static float multiply(const Vector2f& sp, const Vector2f& ep, const Vector2f& op)
	{ 
		return((sp.x-op.x)*(ep.y-op.y)-(ep.x-op.x)*(sp.y-op.y)); 
	} 
	//-------------------------------------------------------------------------
	bool Polygon::isCW() const
	{
		if ( mVertexs.size() < 3 )
			return false;

		//最上（y最小）最左（x最小）点， 肯定是一个凸点
		//寻找最上点
		Vector2f topPt = mVertexs[0];
		int topPtId = 0;	//点的索引
		for ( size_t i = 1; i < (int)mVertexs.size(); ++i )
		{
			if ( topPt.y > mVertexs[i].y )
			{
				topPt = mVertexs[i];
				topPtId = i;
			}
			else if ( topPt.y == mVertexs[i].y ) //y相等时取x最小
			{
				if ( topPt.x > mVertexs[i].x )
				{
					topPt = mVertexs[i];
					topPtId = i;
				}
			}
		}

		//凸点的邻点
		int lastId = topPtId-1 >= 0 ? topPtId-1 : mVertexs.size()-1;
		int nextId = topPtId+1 >= (int)mVertexs.size() ? 0 : topPtId+1;
		Vector2f last = mVertexs[lastId];
		Vector2f next = mVertexs[nextId];

		//判断
		float r = multiply(last, next, topPt);
		if ( r < 0 )
			return true;
		//else if ( r == 0 )
		//{
		//	//三点共线情况不存在，若三点共线则说明必有一点的y（斜线）或x（水平线）小于topPt
		//}

		return false;
	}
	//-------------------------------------------------------------------------
	bool Polygon::isCCW() const
	{
		if ( mVertexs.size() < 3 )
			return false;

		//最上（y最小）最左（x最小）点， 肯定是一个凸点
		//寻找最上点
		Vector2f topPt = mVertexs[0];
		int topPtId = 0;	//点的索引
		for ( size_t i = 1; i < (int)mVertexs.size(); ++i )
		{
			if ( topPt.y > mVertexs[i].y )
			{
				topPt = mVertexs[i];
				topPtId = i;
			}
			else if ( topPt.y == mVertexs[i].y ) //y相等时取x最小
			{
				if ( topPt.x > mVertexs[i].x )
				{
					topPt = mVertexs[i];
					topPtId = i;
				}
			}
		}

		//凸点的邻点
		int lastId = topPtId-1 >= 0 ? topPtId-1 : mVertexs.size()-1;
		int nextId = topPtId+1 >= (int)mVertexs.size() ? 0 : topPtId+1;
		Vector2f last = mVertexs[lastId];
		Vector2f next = mVertexs[nextId];

		//判断
		float r = multiply(last, next, topPt);
		if ( r > 0 )
			return true;
		//else if ( r == 0 )
		//{
		//	//三点共线情况不存在，若三点共线则说明必有一点的y（斜线）或x（水平线）小于topPt
		//}

		return false;
	}
	//-------------------------------------------------------------------------
	void Polygon::CW()
	{
		if ( isCW() )
			return;

		std::vector<Vector2f> v;
		v.reserve(mVertexs.size());
		for( int i = (int)mVertexs.size()-1; i >= 0; --i )
			v.push_back(mVertexs[i]);

		mVertexs = v;
	}
	//-------------------------------------------------------------------------
	void Polygon::CCW()
	{
		if ( isCCW() )
			return;

		std::vector<Vector2f> v;
		v.reserve(mVertexs.size());
		for( int i = (int)mVertexs.size()-1; i >= 0; --i )
			v.push_back(mVertexs[i]);

		mVertexs = v;

	}
}