/******************************************************************************
* ����: Y3
* ʱ��: 2010-09-15
* ����: Delaunay���ǻ���
*		�ο���ַ: http://blianchen.blog.163.com/blog/static/131056299201037102315211/
******************************************************************************/
#ifndef SSystem_Delaunay_H
#define SSystem_Delaunay_H

#include "Triangle.h"
#include "Polygon.h"

namespace SearchSystem
{
	class Delaunay
	{
		class Circle;
		class Rectangle;
	public:
		Delaunay(float epsilon = 0.000001f)
			: mEpsilon(epsilon)
		{
		}

		const std::vector<Triangle>& createDelaunay(std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd);

		const std::vector<Triangle>& createDelaunay(Polygon& poly);

	private:
		/// ��ʼ������
		void initData(const std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd);

		/// ��ȡ��ʼ��߽�
		Line2D getInitOutEdge();

		/// ���� DT ��
		std::pair<bool, Vector2f> findDT(const Line2D& line);

		/// �жϵ�vec�Ƿ�Ϊline�Ŀɼ���
		bool isVisiblePointOfLine(const Vector2f vec, Line2D line);

		/// ��pa��pb�Ƿ�ɼ�(pa��pb���ɵ��߶β����κ�Լ�����ཻ������������)
		bool isVisibleIn2Point(const Vector2f& pa, const Vector2f& pb);

		/// ���������ε����Բ
		Circle circumCircle(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3);

		/// ����Բ�İ�Χ��
		Rectangle circleBounds(const Circle& c);

		/// ���ض�����o�㣬��ʼ��Ϊos����ֹ��Ϊoe�ļн�, ����soe (��λ������);  �Ƕ�С��pi��������ֵ;   �Ƕȴ���pi�����ظ�ֵ 
		float lineAngle(const Vector2f& s, const Vector2f& o, const Vector2f& e); 

		/// �ж��߶��Ƿ���Լ����. �����߶ε����������û���ҵ�������-1
		int indexOfVector(const Line2D& line, const std::vector<Line2D>& vectors);

		/// ����
		void calc();

	private:
		float					mEpsilon;		//����

		std::vector<Vector2f>	mVertexs;		//���ж����б�, ǰoutEdgeVecNmu��Ϊ��߽綥��
		std::vector<Line2D>		mEdges;			//����Լ����

		//int						mOutEdgeVecNmu;	//������߽綥����
		Line2D					mBeginLine;

		std::vector<Line2D>		mLines;			//�߶ζ�ջ

		std::vector<Triangle>	mTriangles;		//���ɵ�Delaunay������

	};
}

#endif