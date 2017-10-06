#include "Polygon.h"



namespace SearchSystem
{
	/**
	* r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)�Ĳ�� 
	* r>0:ep��ʸ��opsp����ʱ�뷽�� 
	* r=0��opspep���㹲�ߣ� 
	* r<0:ep��ʸ��opsp��˳ʱ�뷽�� 
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

		//���ϣ�y��С������x��С���㣬 �϶���һ��͹��
		//Ѱ�����ϵ�
		Vector2f topPt = mVertexs[0];
		int topPtId = 0;	//�������
		for ( size_t i = 1; i < (int)mVertexs.size(); ++i )
		{
			if ( topPt.y > mVertexs[i].y )
			{
				topPt = mVertexs[i];
				topPtId = i;
			}
			else if ( topPt.y == mVertexs[i].y ) //y���ʱȡx��С
			{
				if ( topPt.x > mVertexs[i].x )
				{
					topPt = mVertexs[i];
					topPtId = i;
				}
			}
		}

		//͹����ڵ�
		int lastId = topPtId-1 >= 0 ? topPtId-1 : mVertexs.size()-1;
		int nextId = topPtId+1 >= (int)mVertexs.size() ? 0 : topPtId+1;
		Vector2f last = mVertexs[lastId];
		Vector2f next = mVertexs[nextId];

		//�ж�
		float r = multiply(last, next, topPt);
		if ( r < 0 )
			return true;
		//else if ( r == 0 )
		//{
		//	//���㹲����������ڣ������㹲����˵������һ���y��б�ߣ���x��ˮƽ�ߣ�С��topPt
		//}

		return false;
	}
	//-------------------------------------------------------------------------
	bool Polygon::isCCW() const
	{
		if ( mVertexs.size() < 3 )
			return false;

		//���ϣ�y��С������x��С���㣬 �϶���һ��͹��
		//Ѱ�����ϵ�
		Vector2f topPt = mVertexs[0];
		int topPtId = 0;	//�������
		for ( size_t i = 1; i < (int)mVertexs.size(); ++i )
		{
			if ( topPt.y > mVertexs[i].y )
			{
				topPt = mVertexs[i];
				topPtId = i;
			}
			else if ( topPt.y == mVertexs[i].y ) //y���ʱȡx��С
			{
				if ( topPt.x > mVertexs[i].x )
				{
					topPt = mVertexs[i];
					topPtId = i;
				}
			}
		}

		//͹����ڵ�
		int lastId = topPtId-1 >= 0 ? topPtId-1 : mVertexs.size()-1;
		int nextId = topPtId+1 >= (int)mVertexs.size() ? 0 : topPtId+1;
		Vector2f last = mVertexs[lastId];
		Vector2f next = mVertexs[nextId];

		//�ж�
		float r = multiply(last, next, topPt);
		if ( r > 0 )
			return true;
		//else if ( r == 0 )
		//{
		//	//���㹲����������ڣ������㹲����˵������һ���y��б�ߣ���x��ˮƽ�ߣ�С��topPt
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