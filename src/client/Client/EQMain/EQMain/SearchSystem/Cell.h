/******************************************************************************
* ����: Y3
* ʱ��: 2010-09-19
* ����: 
		�ο���ַ: http://blianchen.blog.163.com/
******************************************************************************/
#ifndef SSystem_Cell_H
#define SSystem_Cell_H

#include "Triangle.h"

namespace SearchSystem
{
	class Cell : public Triangle
	{
	public:
		float		f;
		float		h;
		float		g;

		bool		isOpen;
		int			parent;
		int			sessionId;

		Vector2f	center;

		int			arrivalWall;	//!< the side we arrived through.
		//Vector2f	wallMidpoint[3];//!< ÿ���ߵ��е�
		//float		wallDistance[3];//!< the distances between each wall midpoint of sides (0-1, 1-2, 2-0)

	public:
		Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index);
		Cell(const Vector2f& va, const Vector2f& vb, const Vector2f& vc, int index, int link[3]);

		/// ��鲢���õ�ǰ��������cellB�����ӹ�ϵ��������ͬʱ����cellB��������͵����ӣ�
		void checkAndLink(Cell& cellB);

		/// ��ȡ����������������
		int getLink(int i) {return mLinks[i];}

		/// ��ȡ���������ε�����, ȷ��link��������Чֵ
		int getIndexByLink(int link);

		/// �㵽3���е�ľ���
		void lengthPointToMidLine(Vector2f pt, float outLen[3]);

		/// ���þ���
		void setDistance(int i, float d) {mDistance[i] = d;}

		/// ��ȡ����
		float getDistance(int i) {return mDistance[i];}

		/// ��¼·������һ���ڵ����ýڵ�ıߣ�������յ㿪ʼѰ·��Ϊ�����ߣ�. indexΪ�ϸ��ڵ������
		int setAndGetArrivalWall(int index);

		/// ������ۣ�h��  Compute the A* Heuristic for this cell given a Goal point
		void computeHeuristic(const Vector2f& goal);


		int getIndex() {return mIndex;}

	protected:
		/// ��������������������. ���ȱ�����2��������������, �ŷ���true
		bool requestLink(const Vector2f& pA, const Vector2f& pB, int cellIdx);

	private:
		/// ��������
		Cell();

	protected:
		int			mIndex;			//!< ����ֵ
		int			mLinks[3];		//!< ������������ӵ������������� -1��ʾ�ı�û������
		float		mDistance[3];	//!< ����.
		Vector2f	mMidPoint[3];	//!< �е�

	};
}


#endif