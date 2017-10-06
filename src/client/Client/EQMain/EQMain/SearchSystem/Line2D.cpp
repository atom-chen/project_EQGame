#include "Line2D.h"


namespace SearchSystem
{
	Line2D::Line2D()
	{
		m_bNormalCalculated = false;
	}

	Line2D::Line2D(const Line2D& kIn)
	{
		*this = kIn;
	}

	Line2D::Line2D( const Vector2f& kPointA, const Vector2f& kPointB)
	{
		m_bNormalCalculated = false;
		m_PointA = kPointA;
		m_PointB = kPointB;
	}

	Line2D::~Line2D()
	{

	}

	Line2D& Line2D::operator=( const Line2D& kIn)
	{
		m_PointA = kIn.m_PointA;
		m_PointB = kIn.m_PointB;
		m_bNormalCalculated = false;
		return (*this);
	}

	void Line2D::SentEndPointA( const Vector2f& kPoint)
	{
		m_bNormalCalculated = false;
		m_PointA = kPoint;
	}


	void Line2D::SentEndPointB( const Vector2f& kPoint)
	{
		m_bNormalCalculated = false;
		m_PointB = kPoint;
	}


	void Line2D::SetPoints( const Vector2f& kPointA, const Vector2f& kPointB)
	{
		m_bNormalCalculated = false;
		m_PointA = kPointA;
		m_PointB = kPointB;
	}

	void Line2D::SetPoints( float fAX, float fAY, float fBX, float fBY)
	{
		m_bNormalCalculated = false;
		m_PointA.x = fAX;
		m_PointA.y = fAY;
		m_PointB.x = fBX;
		m_PointB.y = fBY;
	}

	float Line2D::SignedDistance(const Vector2f& kPoint)
	{
		if (!m_bNormalCalculated)
		{
			ComputeNormal();
		}

		Vector2f kTestVector(kPoint - m_PointA);

		return kTestVector.dotProduct(m_kNormal);
	}

	Line2D::POINT_CLASSIFICATION Line2D::ClassifyPoint(const Vector2f& kPoint, float fEpsilon) 
	{
		POINT_CLASSIFICATION	kResult = ON_LINE;

		float fDistance = SignedDistance(kPoint);

		if (fDistance > fEpsilon)
		{
			kResult = RIGHT_SIDE;
		}
		else if (fDistance < -fEpsilon)
		{
			kResult = LEFT_SIDE;
		}

		return(kResult);
	}

	void Line2D::ComputeNormal()
	{
		m_kNormal = (m_PointB - m_PointA);
		m_kNormal.normalise();

		// 修改, 改为+90, 应该说直线的左右侧的定义发生了改变. A->B方向为人的朝向  --Y3
		//
		// Rotate by +90 degrees to get normal of line
		//

		float OldYValue = m_kNormal.y;
		m_kNormal.y = m_kNormal.x;
		m_kNormal.x = -OldYValue;
		m_bNormalCalculated = true;
	}

	Vector2f Line2D::GetNormal()
	{
		if (!m_bNormalCalculated)
		{
			ComputeNormal();
		}

		return m_kNormal;
	}

	Vector2f Line2D::GetMidPoint()
	{
		//这个写法也太搓了吧 --Y3
		//Vector2f kCenter = (m_PointB - m_PointA);
		//kCenter = kCenter * 0.5 + m_PointA;

		//return kCenter;

		return (m_PointB + m_PointA) * 0.5;
	}

	const Vector2f& Line2D::EndPointA() const
	{
		return m_PointA;
	}

	const Vector2f& Line2D::EndPointB() const
	{
		return m_PointB;

	}

	Line2D::LINE_CLASSIFICATION Line2D::Intersection(Line2D& Line, Vector2f* pIntersectPoint)
	{
		//精度改为double
		double Ay_minus_Cy = m_PointA.y - Line.EndPointA().y;	
		double Dx_minus_Cx = Line.EndPointB().x - Line.EndPointA().x;	
		double Ax_minus_Cx = m_PointA.x - Line.EndPointA().x;	
		double Dy_minus_Cy = Line.EndPointB().y - Line.EndPointA().y;	
		double Bx_minus_Ax = m_PointB.x - m_PointA.x;	
		double By_minus_Ay = m_PointB.y - m_PointA.y;	

		double Numerator = (Ay_minus_Cy * Dx_minus_Cx) - (Ax_minus_Cx * Dy_minus_Cy);
		double Denominator = (Bx_minus_Ax * Dy_minus_Cy) - (By_minus_Ay * Dx_minus_Cx);

		// if lines do not intersect, return now
		if (!Denominator)
		{
			if (!Numerator)
			{
				return COLLINEAR;
			}

			return PARALELL;
		}

		double FactorAB = Numerator / Denominator;
		double FactorCD = ((Ay_minus_Cy * Bx_minus_Ax) - (Ax_minus_Cx * By_minus_Ay)) / Denominator;

		// if an interection point was provided, fill it in now
		if (pIntersectPoint)
		{
			pIntersectPoint->x = (float)(m_PointA.x + (FactorAB * Bx_minus_Ax));
			pIntersectPoint->y = (float)(m_PointA.y + (FactorAB * By_minus_Ay));
		}

		// now determine the type of intersection
		if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f) && (FactorCD >= 0.0f) && (FactorCD <= 1.0f))
		{
			return SEGMENTS_INTERSECT;
		}
		else if ((FactorCD >= 0.0f) && (FactorCD <= 1.0f))
		{
			return (A_BISECTS_B);
		}
		else if ((FactorAB >= 0.0f) && (FactorAB <= 1.0f))
		{
			return (B_BISECTS_A);
		}

		return LINES_INTERSECT;
	}
}