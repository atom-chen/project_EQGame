#include "NavSearchSystem.h"
#include "Delaunay.h"

#ifdef USING_NAVSAVELOAD
#include "../tinyXml/tinyxml.h"
#include "../Ogre/OgreResourceGroupManager.h"
#endif

namespace SearchSystem
{
	//-------------------------------------------------------------------------
	NavSearchSystem::NavSearchSystem(float epsilon)
		: mEpsilon(epsilon)
	{
		mEdgeBegin = Vector2f(-20.0f, -20.0f);
		mEdgeEnd = Vector2f(20.0f, 20.0f);

		mNavMesh.mEpsilon = mEpsilon;

		//设置cells
		mNavMesh.setCells(&mCells);
	}
	//-------------------------------------------------------------------------
	void NavSearchSystem::setEdge(Vector2f edgeBegin, Vector2f edgeEnd)
	{
		mEdgeBegin = edgeBegin;
		mEdgeEnd = edgeEnd;
	}
	//-------------------------------------------------------------------------
	void NavSearchSystem::build(std::vector<Polygon>& polys)
	{
		//构建Delaunay三角形
		Delaunay delau(mEpsilon);
		const std::vector<Triangle>& trgs = delau.createDelaunay(polys, mEdgeBegin, mEdgeEnd);

		//创建cell
		mCells.clear();
		for(size_t i = 0; i < trgs.size(); ++i)
		{
			Vector2f ab = trgs[i].getPointB() - trgs[i].getPointA();
			Vector2f ac = trgs[i].getPointC() - trgs[i].getPointA();

			assert( ab.crossProduct(ac) <= 0 );
			mCells.push_back(Cell(trgs[i].getPointA(), trgs[i].getPointB(), trgs[i].getPointC(), i));
		}

		//这个行为效率低啊!
		//搜索单元网格的邻接网格，并保存链接数据到网格中，以提供给寻路用.
		for(size_t i = 0; i < mCells.size(); ++i)
		{
			for(size_t j = 0; j < mCells.size(); ++j)
			{
				if ( i != j )
					mCells[i].checkAndLink(mCells[j]);
			}
		}

		////设置三角形中心点直接的距离
		//for(size_t i = 0; i < mCells.size(); ++i)
		//{
		//	for(int j = 0; j < 3; ++j)
		//	{
		//		int l = mCells[i].getLink(j);
		//		if (l == -1)
		//			continue;
		//		if ( mCells[i].getLinkDistance(j) != 0.0f )
		//			continue;
		//		float len = (mCells[i].center - mCells[l].center).length();
		//		mCells[i].setLinkDistance(j, len);
		//		for (int k = 0; k < 3; ++k)
		//		{
		//			if ( mCells[l].getLink(k) == mCells[i].getIndex() )
		//			{
		//				mCells[l].setLinkDistance(k, len);
		//				break;
		//			}
		//		}
		//	}
		//}

		for(size_t i = 0; i < mCells.size(); ++i)
		{
			Line2D l0 = mCells[i].getSide(Triangle::SIDE_AB);
			mCells[i].setDistance(0, (l0.EndPointA()-l0.EndPointB()).length()/2.0f);
			Line2D l1 = mCells[i].getSide(Triangle::SIDE_BC);
			mCells[i].setDistance(1, (l1.EndPointA()-l1.EndPointB()).length()/2.0f);
			Line2D l2 = mCells[i].getSide(Triangle::SIDE_CA);
			mCells[i].setDistance(2, (l2.EndPointA()-l2.EndPointB()).length()/2.0f);
		}
	}
	//-------------------------------------------------------------------------
	void NavSearchSystem::destroy()
	{
		mCells.clear();
		mNavMesh.clear();
	}
#ifdef USING_NAVSAVELOAD
	//-------------------------------------------------------------------------
	void NavSearchSystem::save(const std::string& filename)
	{
		if ( mCells.empty() )
			return;

		TiXmlDocument doc;
		TiXmlElement *ele;
		char buf[256];

		TiXmlDeclaration dec;
		dec.Parse( "<?xml version='1.0' encoding='UTF-8'standalone='yes' ?>", 0, TIXML_ENCODING_UNKNOWN );
		doc.InsertEndChild( dec );

		TiXmlElement* root = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement("NavMesh")));
		sprintf_s(buf, sizeof(buf), "%f %f %f %f", mEdgeBegin.x, mEdgeBegin.y, mEdgeEnd.x, mEdgeEnd.y);
		root->SetAttribute("range", buf);
		//是否还需要版本号?

		TiXmlElement* cellNode = static_cast<TiXmlElement*>(root->InsertEndChild(TiXmlElement("Cells")));

		for ( size_t i = 0; i < mCells.size(); ++i )
		{
			ele = static_cast<TiXmlElement*>(cellNode->InsertEndChild(TiXmlElement("Cell")));
			sprintf_s(buf, sizeof(buf), "%f %f", mCells[i].getPointA().x, mCells[i].getPointA().y);
			ele->SetAttribute("a", buf);
			sprintf_s(buf, sizeof(buf), "%f %f", mCells[i].getPointB().x, mCells[i].getPointB().y);
			ele->SetAttribute("b", buf);
			sprintf_s(buf, sizeof(buf), "%f %f", mCells[i].getPointC().x, mCells[i].getPointC().y);
			ele->SetAttribute("c", buf);

			sprintf_s(buf, sizeof(buf), "%d %d %d", mCells[i].getLink(0), mCells[i].getLink(1), mCells[i].getLink(2));
			ele->SetAttribute("link", buf);
		}

		doc.SaveFile(filename);
	}
	//-------------------------------------------------------------------------
	bool NavSearchSystem::load(const std::string& filename)
	{
		destroy();

		TiXmlElement *ele;
		TiXmlDocument doc;
		if ( !Ogre::ResourceGroupManager::getSingleton().resourceExists(
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, filename) )
			return false;
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
		doc.Parse( stream->getAsString().c_str() );

		TiXmlElement* root = doc.FirstChildElement("NavMesh");
		if ( root == NULL )
			return false;
		std::string str = root->Attribute("range");
		sscanf_s(str.c_str(), "%f %f %f %f", &mEdgeBegin.x, &mEdgeBegin.y, &mEdgeEnd.x, &mEdgeEnd.y);

		TiXmlElement *cellNode = root->FirstChildElement("Cells");
		if ( cellNode == NULL )
			return false;

		ele = cellNode->FirstChildElement("Cell");
		while(ele)
		{
			std::string str;
			float x, y;
			int link[3];
			str =  ele->Attribute("a");	//指针如果是空, 就当机
			sscanf_s(str.c_str(), "%f %f", &x, &y);
			Vector2f a(x,y);
			str =  ele->Attribute("b");
			sscanf_s(str.c_str(), "%f %f", &x, &y);
			Vector2f b(x,y);
			str =  ele->Attribute("c");
			sscanf_s(str.c_str(), "%f %f", &x, &y);
			Vector2f c(x,y);
			str =  ele->Attribute("link");
			sscanf_s(str.c_str(), "%d %d %d", &link[0], &link[1], &link[2]);

			mCells.push_back(Cell(a, b, c, mCells.size(), link));

			//
			ele = ele->NextSiblingElement("Cell");
		}

		////设置三角形中心点直接的距离
		//for(size_t i = 0; i < mCells.size(); ++i)
		//{
		//	for(int j = 0; j < 3; ++j)
		//	{
		//		int l = mCells[i].getLink(j);
		//		if (l == -1)
		//			continue;
		//		if ( mCells[i].getLinkDistance(j) != 0.0f )
		//			continue;
		//		float len = (mCells[i].center - mCells[l].center).length();
		//		mCells[i].setLinkDistance(j, len);
		//		for (int k = 0; k < 3; ++k)
		//		{
		//			if ( mCells[l].getLink(k) == mCells[i].getIndex() )
		//			{
		//				mCells[l].setLinkDistance(k, len);
		//				break;
		//			}
		//		}
		//	}
		//}

		for(size_t i = 0; i < mCells.size(); ++i)
		{
			Line2D l0 = mCells[i].getSide(Triangle::SIDE_AB);
			mCells[i].setDistance(0, (l0.EndPointA()-l0.EndPointB()).length()/2.0f);
			Line2D l1 = mCells[i].getSide(Triangle::SIDE_BC);
			mCells[i].setDistance(1, (l1.EndPointA()-l1.EndPointB()).length()/2.0f);
			Line2D l2 = mCells[i].getSide(Triangle::SIDE_CA);
			mCells[i].setDistance(2, (l2.EndPointA()-l2.EndPointB()).length()/2.0f);
		}
		return true;
	}
#endif
	//-------------------------------------------------------------------------
	bool NavSearchSystem::findPath(const Vector2f& startPointPx, const Vector2f& endPointPx)
	{
		//超出边界
		if ( startPointPx.x < mEdgeBegin.x || startPointPx.y < mEdgeBegin.y
			|| startPointPx.x > mEdgeEnd.x || startPointPx.y > mEdgeEnd.y )
			return false;

		if ( endPointPx.x < mEdgeBegin.x || endPointPx.y < mEdgeBegin.y
			|| endPointPx.x > mEdgeEnd.x || endPointPx.y > mEdgeEnd.y )
			return false;

		return mNavMesh.findPath(startPointPx, endPointPx);
	}
	//-------------------------------------------------------------------------
	void NavSearchSystem::getPath(std::vector<Vector2f>& path)
	{
		mNavMesh.getPath(path);
	}
}