#include "Delaunay.h"

extern "C" 
{ 
#include "gpc.h"
}


namespace SearchSystem
{
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

	/// Բ
	class Delaunay::Circle
	{
	public:
		Vector2f center;        //Բ��
		float r;				//�뾶

		Circle(Vector2f cen, float rd) {
			center = cen;
			r = rd;
		}
	};

	/// ����
	class Delaunay::Rectangle
	{
	public:
		Vector2f pos;
		Vector2f side;

		Rectangle(float x, float y, float xL, float yL) {
			pos.x = x; pos.y = y;
			side.x = xL; side.y = yL;
		}

		bool contains(float x, float y) {
			if ( x >= pos.x && x <= pos.x+side.x
				&& y >= pos.y && y <= pos.y+side.y )
				return true;
			return false;
		}
	};

	//-------------------------------------------------------------------------
	void Delaunay::initData(const std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd)
	{
		mVertexs.clear();
		mLines.clear();

		//������ֹµ�, �ͺ�����. ��ʵ�µ�����union֮���ж�, ������Щ���Ϳ���
		gpc_polygon gpc = {0};

		//�ϲ����ж����
		for ( size_t i = 0; i < polys.size(); ++i )
		{
			size_t c = polys[i].getVertexs().size();
			if ( c < 3 )	//���Ե����Ƕ����
				continue;

			gpc_vertex* v = new gpc_vertex[c];
			for ( size_t n = 0; n < c; ++n )
			{
				v[n].x = polys[i].getVertexs()[n].x;
				v[n].y = polys[i].getVertexs()[n].y;
			}
			gpc_vertex_list gvl = {c, v};
			gpc_polygon gpcOut = {0};
			gpc_polygon gpcDest = {0};
			gpc_add_contour(&gpcDest, &gvl, 0);
			gpc_polygon_clip(GPC_UNION, &gpc, &gpcDest, &gpcOut);
			gpc_free_polygon(&gpcDest);
			gpc_free_polygon(&gpc);
			gpc = gpcOut;

			delete[] v;
		}

		//ȥ���߽���Ĳ���
		{
			gpc_vertex vv[4] = {{edgeBegin.x,edgeBegin.y}, {edgeBegin.x,edgeEnd.y}, {edgeEnd.x,edgeEnd.y}, {edgeEnd.x,edgeBegin.y}};
			gpc_vertex_list gvl = {4, vv};
			gpc_polygon gpcOut = {0};
			gpc_polygon gpcDest = {0};
			gpc_add_contour(&gpcDest, &gvl, 0);
			gpc_polygon_clip(GPC_INT, &gpc, &gpcDest, &gpcOut);
			gpc_free_polygon(&gpcDest);
			gpc_free_polygon(&gpc);
			gpc = gpcOut;
		}

		//ȥ���µ�
		{
			gpc_polygon gpcOut = {0};
			for ( int i = 0; i < gpc.num_contours; ++i )
			{
				if ( gpc.hole[i] == 0 )	//�ǹµ�
				{
					gpc_add_contour(&gpcOut, &gpc.contour[i], 0);
				}
			}
			gpc_free_polygon(&gpc);
			gpc = gpcOut;
		}

		//����μ���ȡ�߽練��, �õ�һ�����߶�������
		{
			gpc_vertex vv[4] = {{edgeBegin.x,edgeBegin.y}, {edgeBegin.x,edgeEnd.y}, {edgeEnd.x,edgeEnd.y}, {edgeEnd.x,edgeBegin.y}};
			gpc_vertex_list gvl = {4, vv};
			gpc_polygon gpcOut = {0};
			gpc_polygon gpcDest = {0};
			gpc_add_contour(&gpcDest, &gvl, 0);
			gpc_polygon_clip(GPC_XOR, &gpc, &gpcDest, &gpcOut);
			gpc_free_polygon(&gpcDest);
			gpc_free_polygon(&gpc);
			gpc = gpcOut;
		}

		//��ӱ�
		std::vector<Polygon> newPolys;
		bool bl = false;
		for ( int i = 0; i < gpc.num_contours; ++i )
		{
			//�п��ܳ��ֶ��holeΪ0, �����ܳ��ֶ��Ѱ·ϵͳ, ����Ŀǰ��Ѱ·ϵͳ�ǲ������, 
			//�������ڱ߽��޳���������ķָ�, ����Ŀ�����. ����ֻȡ��һ��hole��ʼ��Ѱ·�߽���
			//ò��ĿǰholeΪ0�ıߵķ�������ȷ��
			gpc_vertex_list& gvl = gpc.contour[i];

			if ( !bl && gpc.hole[i] == 0 )
			{
				assert(gvl.num_vertices>=2);
				Vector2f a((float)gvl.vertex[0].x, (float)gvl.vertex[0].y);
				Vector2f b((float)gvl.vertex[1].x, (float)gvl.vertex[1].y);
				mBeginLine = Line2D(a, b);
				bl = true;
			}

			Polygon pl;
			for ( int j = 0; j < gvl.num_vertices; ++j )
				pl.addVertex(Vector2f((float)gvl.vertex[j].x, (float)gvl.vertex[j].y));
			newPolys.push_back(pl);
		}
		assert(bl);

		gpc_free_polygon(&gpc);


		//--------------------
		for(std::vector<Polygon>::const_iterator it = newPolys.begin(); it != newPolys.end(); ++it)
		{
			const Polygon* poly = &(*it);
			const std::vector<Vector2f>& vertexs = poly->getVertexs();

			//��Ӷ���
			for(std::vector<Vector2f>::const_iterator it = vertexs.begin(); it != vertexs.end(); ++it)
				mVertexs.push_back(*it);

			//��ӱ�
			if ( vertexs.size() >= 3 )	//������һ�������
			{
				Vector2f p1 = vertexs[0];
				Vector2f p2;
				for(size_t i = 1; i < vertexs.size(); ++i)
				{
					p2 = vertexs[i];
					mEdges.push_back(Line2D(p1,p2));
					p1 = p2;
				}
				p2 = vertexs[0];
				mEdges.push_back(Line2D(p1,p2));
			}
		}

		//mOutEdgeVecNmu = newPolys[0].getVertexs().size();
	}
	//-------------------------------------------------------------------------
	Line2D Delaunay::getInitOutEdge()
	{
#if 1
		return mBeginLine;
#else
		Line2D initEdge = mEdges[0];

		//����Ƿ��ж���p�ڸñ��ϣ��������һ����߽�
		bool loopSign;
		int loopIdx = 0;
		do 
		{
			loopSign = false;
			loopIdx++;
			for(size_t i=0; i<mVertexs.size(); ++i)
			{
				if ( mVertexs[i] == initEdge.EndPointA() || mVertexs[i] == initEdge.EndPointB() )
					continue;
				if ( initEdge.ClassifyPoint(mVertexs[i],mEpsilon) == Line2D::ON_LINE )
				{
					loopSign = true;
					initEdge = mEdges[loopIdx];
					break;
				}
			}
		} while (loopSign && loopIdx < mOutEdgeVecNmu-1); //ֻȡ��߽�
		return initEdge;
#endif

	}
	//-------------------------------------------------------------------------
	std::pair<bool, Vector2f> Delaunay::findDT(const Line2D& line)
	{
		Vector2f p1 = line.EndPointA();
		Vector2f p2 = line.EndPointB();

		//�������пɼ���             TODO ��y�����������߶��յ�����ĵ�
		std::vector<Vector2f> allVPoint;	// line�����пɼ���
		for(size_t i=0; i<mVertexs.size(); ++i)
		{
			if(isVisiblePointOfLine(mVertexs[i], line))
				allVPoint.push_back(mVertexs[i]);
		}

		if ( allVPoint.size() == 0 )
			return std::pair<bool, Vector2f>(false, Vector2f());

		Vector2f p3 = allVPoint[0];
		bool loopSign = false;
		do 
		{
			loopSign = false;

			//Step1. ���� ��p1p2p3 �����Բ C��p1��p2��p3�����������Χ�� B��C��p1��p2��p3����
			Circle circle = circumCircle(p1, p2, p3);
			Rectangle boundsBox = circleBounds(circle);

			//Step2. ���η��������Χ���ڵ�ÿ������Ԫ��
			//       ��ĳ������Ԫ�д��ڿɼ��� p, ���� ��p1pp2 > ��p1p3p2������ p3=p��תStep1������תStep3.
			float angle132 = fabs(lineAngle(p1, p3, p2));	// ��p1p3p2
			for ( size_t i = 0; i < allVPoint.size(); ++i )
			{
				Vector2f& vec = allVPoint[i];
				if ( vec == p1 || vec == p2 || vec == p3 )
					continue;

				//���ڰ�Χ����
				if (boundsBox.contains(vec.x, vec.y) == false)
					continue;

				//�н�
				float a1 = fabs(lineAngle(p1, vec, p2));
				if(a1 > angle132)
				{
					//תStep1
					p3 = vec;
					loopSign = true;
					break;
				}
			}

			//תStep3
		} while (loopSign);


		//Step3. ����ǰ�����Χ������������Ԫ���ѱ������꣬
		//       Ҳ��C��p1��p2��p3�����޿ɼ��㣬�� p3 Ϊ�� p1p2 �� DT ��
		return std::pair<bool, Vector2f>(true, p3);
	}
	//-------------------------------------------------------------------------
	bool Delaunay::isVisiblePointOfLine(const Vector2f vec, Line2D line)
	{
		if(vec==line.EndPointA() || vec==line.EndPointB())
			return false;

		//��1�� p3 �ڱ� p1p2 ���Ҳ� (����ζ���˳��Ϊ˳ʱ��)��		//��������Ҫ����, �����, �����ڶ�����ڲ�   --Y3���������Line2D::RIGHT_SIDE->Line2D::LEFT_SIDE  mEpsilon�����ֵ, ���������Ҫ�����
		if(line.ClassifyPoint(vec, mEpsilon) != Line2D::LEFT_SIDE)
			return false;

		//��2�� p3 �� p1 �ɼ����� p1p3 �����κ�һ��Լ�����ཻ��
		if (isVisibleIn2Point(line.EndPointA(), vec) == false) 
			return false;

		//��3�� p3 �� p2 �ɼ�
		if (isVisibleIn2Point(line.EndPointB(), vec) == false)
			return false;

		return true;
	}
	//-------------------------------------------------------------------------
	bool Delaunay::isVisibleIn2Point(const Vector2f& pa, const Vector2f& pb)
	{
		Line2D linepapb(pa, pb);
		Vector2f interscetVector;	//�߶ν���
		for(size_t i = 0; i < mEdges.size(); ++i)
		{
			//���߶��ཻ�ж�, ����Intersection���ܲ�������, ���ܳ��ֹ��ö˵��ཻ�ж���׼ȷ������, ����������2�ߵĹ��õ�, ���Է�2����ִ�� --Y3
			if(pa == mEdges[i].EndPointA() || pa == mEdges[i].EndPointB()
				|| pb == mEdges[i].EndPointA() || pb == mEdges[i].EndPointB())		//���ö���, �������Ƕ˵�, ����
			{
				continue;
			}
			else if(linepapb.Intersection(mEdges[i], &interscetVector) == Line2D::SEGMENTS_INTERSECT)	//���߶��ཻ, 
			{
				//�����ǲ��Ƕ˵�, ���ڸ�������������, �������Ż�
				float e = fabs(interscetVector.x) > fabs(interscetVector.y) ? fabs(interscetVector.x) : fabs(interscetVector.y);
				e *= 0.000001f;
				if(!pa.equals(interscetVector, mEpsilon+e) && !pb.equals(interscetVector, mEpsilon+e))
					return false;
			}
		}
		return true;
	}
	//-------------------------------------------------------------------------
	Delaunay::Circle Delaunay::circumCircle(const Vector2f& p1, const Vector2f& p2, const Vector2f& p3)
	{
		float m1, m2, mx1, mx2, my1, my2;
		float dx, dy, rsqr/*, drsqr*/;
		float xc, yc, r;

		/* Check for coincident points */
		const float epsilon = 0.000001f;
		assert(fabs(p1.y-p2.y) > epsilon || fabs(p2.y-p3.y) > epsilon );

		m1 = -(p2.x - p1.x) / (p2.y - p1.y);
		m2 = -(p3.x-p2.x) / (p3.y-p2.y);
		mx1 = (p1.x + p2.x) / 2.0f;
		mx2 = (p2.x + p3.x) / 2.0f;
		my1 = (p1.y + p2.y) / 2.0f;
		my2 = (p2.y + p3.y) / 2.0f;

		if ( fabs(p2.y-p1.y) < epsilon )
		{
			xc = (p2.x + p1.x) / 2.0f;
			yc = m2 * (xc - mx2) + my2;
		}
		else if (fabs(p3.y - p2.y) < epsilon)
		{
			xc = (p3.x + p2.x) / 2.0f;
			yc = m1 * (xc - mx1) + my1;   
		}
		else 
		{
			xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
			yc = m1 * (xc - mx1) + my1;
		}

		dx = p2.x - xc;
		dy = p2.y - yc;
		rsqr = dx*dx + dy*dy;
		r = sqrt(rsqr);

		return Delaunay::Circle(Vector2f(xc, yc), r);
	}
	//-------------------------------------------------------------------------
	Delaunay::Rectangle Delaunay::circleBounds(const Circle& c)
	{
		return Delaunay::Rectangle(c.center.x-c.r, c.center.y-c.r, c.r*2, c.r*2);
	}
	//-------------------------------------------------------------------------
	float Delaunay::lineAngle(const Vector2f& s, const Vector2f& o, const Vector2f& e)
	{
		float cosfi, fi, norm;
		float dsx = s.x - o.x;
		float dsy = s.y - o.y;
		float dex = e.x - o.x;
		float dey = e.y - o.y;

		cosfi = dsx*dex + dsy*dey;
		norm = (dsx*dsx + dsy*dsy) * (dex*dex + dey*dey);
		cosfi /= sqrt( norm );

		if (cosfi >=  1.0 ) return 0;
		if (cosfi <= -1.0 ) return -(float)M_PI;

		fi = acos(cosfi);
		if (dsx*dey - dsy*dex > 0) return fi;      // ˵��ʸ��os ��ʸ�� oe��˳ʱ�뷽��
		return -fi; 
	}
	//-------------------------------------------------------------------------
	int Delaunay::indexOfVector(const Line2D& line, const std::vector<Line2D>& vectors)
	{
		for (size_t i = 0; i < vectors.size(); ++i)
		{
			const Line2D& lt = vectors[i];
			if(lt.EndPointA()==line.EndPointA() && lt.EndPointB()==line.EndPointB()
				|| lt.EndPointA()==line.EndPointB() && lt.EndPointB()==line.EndPointA())
				return i;
		}
		return -1;
	}
	//-------------------------------------------------------------------------
	const std::vector<Triangle>& Delaunay::createDelaunay(std::vector<Polygon>& polys, const Vector2f& edgeBegin, const Vector2f& edgeEnd)
	{
		//Step1.    ������Ԫ��СΪ E*E �ľ������񣬲�������εĶ���ͱ߷�������.
		//          ���� E=sqrt(w*h/n)��w �� h �ֱ�Ϊ��������Χ�еĿ�ȡ��߶ȣ�n Ϊ�������Ķ����� .
		initData(polys, edgeBegin, edgeEnd);

		//Step2 - Step5
		calc();

		return mTriangles;
	}
	//-------------------------------------------------------------------------
	const std::vector<Triangle>& Delaunay::createDelaunay(Polygon& poly)
	{
		if ( poly.getVertexs().size() < 3 )
		{
			mTriangles.clear();
			return mTriangles;
		}

		mVertexs.clear();
		mLines.clear();

		//Step1.	������εĶ���ͱ߷�������
		//
		const std::vector<Vector2f>& vertexs = poly.getVertexs();

		//��Ӷ���
		for(std::vector<Vector2f>::const_iterator it = vertexs.begin(); it != vertexs.end(); ++it)
			mVertexs.push_back(*it);

		//��ӱ�
		if ( vertexs.size() >= 3 )	//������һ�������
		{
			Vector2f p1 = vertexs[0];
			Vector2f p2;
			for(size_t i = 1; i < vertexs.size(); ++i)
			{
				p2 = vertexs[i];
				mEdges.push_back(Line2D(p1,p2));
				p1 = p2;
			}
			p2 = vertexs[0];
			mEdges.push_back(Line2D(p1,p2));
		}

		//������ʼ��
		mBeginLine.SentEndPointA(mVertexs[0]);
		mBeginLine.SentEndPointB(mVertexs[1]);

		//Step2 - Step5
		calc();

		return mTriangles;
	}
	//-------------------------------------------------------------------------
	void Delaunay::calc()
	{
		mTriangles.clear();
		mLines.clear();

		//Step2.    ȡ����һ����߽�� p1p2 .
		Line2D initEdge = getInitOutEdge();
		mLines.push_back(initEdge);

		Line2D edge;
		do 
		{
			//Step3.���� DT �� p3������Լ�� Delaunay ������ ��p1p2p3 .
			edge = mLines.back();
			mLines.pop_back();
			std::pair<bool,Vector2f> deret = findDT(edge);
			if(!deret.first)
				continue;
			Vector2f p3 = deret.second;

			Line2D line13(edge.EndPointA(), p3);
			Line2D line32(p3, edge.EndPointB());
			//Delaunay�����η����������
			Triangle trg(edge.EndPointA(), edge.EndPointB(), p3);

			mTriangles.push_back(trg);

			//Step4.��������ɵı� p1p3 ����Լ���ߣ����Ѿ��ڶ�ջ�У�
			//      �������ɾ�������򣬽�������ջ�����Ƶأ��ɴ��� p3p2 .
			int index;
			if(indexOfVector(line13, mEdges) < 0)
			{
				index = indexOfVector(line13, mLines);
				if(index > -1)
					mLines.erase(mLines.begin()+index);
				else
					mLines.push_back(line13);
			}
			if(indexOfVector(line32, mEdges) < 0)
			{
				index = indexOfVector(line32, mLines);
				if(index > -1)
					mLines.erase(mLines.begin()+index);
				else
					mLines.push_back(line32);
			}

			//Step5.����ջ���գ������ȡ��һ���ߣ�תStep3�������㷨ֹͣ .
		} while (mLines.size() > 0);

	}
}