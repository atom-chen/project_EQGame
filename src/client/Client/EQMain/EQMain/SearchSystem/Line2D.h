/******************************************************************************
* ����: Y3
* ʱ��: 2010-09-15
* ����: 2dֱ��
*		��������http://www.koders.com/cpp/fidD62B6464E9EB4FEBF114F2ADEE85FEEAEDBB7E63.aspx?s=huffman
******************************************************************************/
#ifndef SSystem_Line2D_H
#define SSystem_Line2D_H

#include "Vector2f.h"

namespace SearchSystem
{
	class Line2D
	{
	public:
		enum POINT_CLASSIFICATION
		{
			ON_LINE,				// The point is on the line.
			LEFT_SIDE,				// Looking from A to B the point is on the right side.
			RIGHT_SIDE				// Looking from A to B the point is on the left side.
		};

		enum LINE_CLASSIFICATION
		{
			COLLINEAR,				// 
			PARALELL,				// The lines are paralell.
			LINES_INTERSECT,	
			SEGMENTS_INTERSECT,	
			A_BISECTS_B,		
			B_BISECTS_A,	
		};

		// Constructors
		Line2D();
		Line2D( const Line2D& kIn);
		Line2D( const Vector2f& kPointA, const Vector2f& kPointB );
		~Line2D();

		// Operators
		Line2D& operator=( const Line2D& kIn);

		// Methods
		void SentEndPointA( const Vector2f& kPoint);
		void SentEndPointB( const Vector2f& kPoint);
		void SetPoints( const Vector2f& kPointA, const Vector2f& kPointB);
		void SetPoints( float fAX, float fAY, float fBX, float fBY);

		float SignedDistance(const Vector2f& kPoint);
		POINT_CLASSIFICATION ClassifyPoint(const Vector2f& kPoint, float fEpsilon = 0.0f);
		//���������Ȼû���ٽ������ж�, ���߾����ݴ�, ��ĳЩ���������п��ܳ����, ����������2�����ж�, ���ڸ����������, �п��ܳ��ֲ��ཻ�����.  --Y3
		LINE_CLASSIFICATION Intersection(Line2D& Line, Vector2f* pIntersectPoint);

		// Accessors 
		Vector2f GetNormal();
		Vector2f GetMidPoint();
		const Vector2f& EndPointA() const;
		const Vector2f& EndPointB() const;

	private:
		// Data
		Vector2f	m_PointA;	// Endpoint A.
		Vector2f	m_PointB;	// Endpoint B

		Vector2f	m_kNormal;	
		bool		m_bNormalCalculated; 

		void ComputeNormal();
	};
}

#endif