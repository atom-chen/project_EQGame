#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "windows.h"

using namespace std;

void ThrowIf(bool b)
{
	if (b)
		throw b;
}

std::wstring GB2312ToUnicode( const char * str )
{
	int textlen = MultiByteToWideChar( 936, 0, str, -1, NULL, 0 ); 	
	std::wstring buf( textlen, 0 );
	
	MultiByteToWideChar( 936, 0, str, -1, const_cast<LPWSTR>( buf.c_str() ) , buf.size() ); 

	return buf.c_str(); 
}

std::string UnicodeToUTF8( const wchar_t * str )
{
	int textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );
	std::string buf( textlen, 0 );

	WideCharToMultiByte( CP_UTF8, 0, str, -1, const_cast<LPSTR>( buf.c_str() ), buf.size(), NULL, NULL );

	return buf.c_str();
}

std::string GB2312ToUTF8( const char * str )
{
	return UnicodeToUTF8( GB2312ToUnicode(str).c_str() );
}


string join_string(vector<string> const & arr, char const ch)
{
	char buf[2] = {0};
	buf[0] = ch;

	string s;
	for (unsigned int i = 0; i < arr.size(); ++i)
		s = s + arr[i] + string(buf);
	
	if (s.size() > 0)
		s.resize(s.size() - 1);

	return s;
}

vector<string> split_string(string const & s, char const ch)
{
	vector<string> arr;
	unsigned int startPos = 0;
	unsigned int endPos = 0;
	for ( ; ; )
	{
		endPos = s.find_first_of(ch, startPos);
		if (endPos < s.size())
		{			
			arr.push_back(string(s.c_str() + startPos, s.c_str() + endPos));
		}
		else
			break;

		startPos = endPos + 1;
		if (startPos == s.size())
			arr.push_back(string());
	}

	if (startPos < s.size())
		arr.push_back(string(s.c_str() + startPos));

	return arr;
}

void test_split()
{
	string s = "a,b,c";
	vector<string> arr = split_string(s, ',');
	
	ThrowIf(arr[0] != "a");
	ThrowIf(arr[1] != "b");
	ThrowIf(arr[2] != "c");

	s = "a,b,";
	arr = split_string(s, ',');
	
	ThrowIf(arr[0] != "a");
	ThrowIf(arr[1] != "b");
	ThrowIf(arr[2] != "");
}
template <typename TStream>
void write_version(TStream & stream)
{
	string s = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
	stream.write(s.c_str(), s.size());
}

template <typename TStream>
void begin_tag(TStream & stream, string const & tag)
{
	string s = "<" + tag + ">";
	stream.write(s.c_str(), s.size());
}

template <typename TStream>
void end_tag(TStream & stream, string const & tag)
{
	string s = "</" + tag + ">";
	stream.write(s.c_str(), s.size());
}

template <typename TStream>
void write_node(TStream & stream, string const & node, string const & val)
{
	begin_tag(stream, node);
	string s = GB2312ToUTF8(val.c_str());
	stream.write(s.c_str(), s.size());
	end_tag(stream, node);
}

template <typename TStream>
void write_new_line(TStream & stream, int nTab)
{
	string s;
	s.resize(nTab + 1, '\t');
	*s.begin() = '\n';
	stream.write(s.c_str(), s.size());
}

vector<string> extract_cols(string const & s)
{
	// 跳过两列
	int const skipCol = 2;	
	vector<string> ret;
	vector<string> arr = split_string(s, ',');
	int iCol = 1;
	for (vector<string>::iterator it = arr.begin();
		it != arr.end();
		++it, ++iCol)
	{
		//if (it->empty())
		//	continue;
		//if (*it == "0")
		//	continue;
		//
		//ret.push_back(*it);
		if (iCol <= skipCol)
		{
			ret.push_back(*it);
			continue;
		}
			 
		if (*it == "C" || *it == "c")
			ret.push_back("1");
		else if (*it == "T" || *it == "t")
			ret.push_back("2");
		else if (*(it->begin()) == 'B' || *(it->begin()) == 'b')
			ret.push_back("3");
		else
			ret.push_back("4");
	}
	// 去掉第二行
	//ret.erase(ret.begin() + 1);
	return ret;
}


vector<string> extract_error_cols(string const & s)
{
	// 跳过两列
	int const skipCol = 2;	
	vector<string> ret;
	vector<string> arr = split_string(s, ',');
	int iCol = 1;
	for (vector<string>::iterator it = arr.begin();
		it != arr.end();
		++it, ++iCol)
	{
		//if (it->empty())
		//	continue;
		//if (*it == "0")
		//	continue;
		//
		//ret.push_back(*it);
		if (iCol <= skipCol)
		{
			ret.push_back(*it);
			continue;
		}
		
		// 取错误类型
		if (it->size() > 0)
		{
			if (*(it->begin()) == 'B' || *(it->begin()) == 'b')
			{
				if (it->size() == 1)
				{
					ret.push_back("2"); // 默认报错类型为2
				}
				else
				{
					string sType = std::string(it->begin() + 1, it->end()); 
					ret.push_back(sType);
				}
			}
			else
			{
				ret.push_back("0");
			}
		}
		else
		{
			ret.push_back("0");
		}
	}
	// 去掉第二行
	//ret.erase(ret.begin() + 1);
	return ret;
}


void rotate_string_table(vector<vector<string> > & table)
{	
	unsigned int n = table.size();
	for (unsigned int y = 0; y < n; ++y)
	{
		if (table[y].size() != n)
			cout << " 行数" << n << "不等于列数" << table[y].size() << ",y=" << y << endl;
		for (unsigned int x = 0; x <= y; ++x)
		{
			swap(table[x][y], table[y][x]);
		}
	}
}

void allow_actions_csv_to_string_table(string const & filenameCSV, vector<vector<string> > & table)
{
	table.clear();

	ifstream sr;
	sr.open(filenameCSV.c_str());

	unsigned int startPos = filenameCSV.find_last_of(":/\\");
	unsigned int endPos = filenameCSV.find_last_of('.');

	string filename = string(filenameCSV.c_str() + startPos + 1, filenameCSV.c_str() + endPos); 

	int const bufLen = 1024;
	char buf[bufLen] = {0};

	for (; sr.getline(buf, bufLen); )
	{
		table.push_back(split_string(buf, ','));
	}
}

template <typename TOutStream>
void string_table_to_xml(string const & filenameCSV, vector<vector<string> > const & table, TOutStream & sw)
{
	unsigned int startPos = filenameCSV.find_last_of(":/\\");
	unsigned int endPos = filenameCSV.find_last_of('.');

	string filename = string(filenameCSV.c_str() + startPos + 1, filenameCSV.c_str() + endPos); 
	int nTab = 0;
	write_version(sw);

	write_new_line(sw, nTab++);
	begin_tag(sw, "root");
	// 跳过两行
	int const skipLine = 2;	
	for (int row = 1; row <= (int)table.size(); ++row)
	{
		if (row <= skipLine)
			continue;
		write_new_line(sw, nTab++);
		begin_tag(sw, filename);

		vector<string> arr = extract_cols(join_string(table[row-1], ','));

		write_new_line(sw, nTab);
		write_node(sw, "id", arr.front());
		write_new_line(sw, nTab);
		string name = *(arr.begin() + 1);
		//name.
		if (name.find("开始") == 0)
		{
			name = name.c_str() + 4;
		}

		/*if (name.find("使用") == 0)
		{
			name = name.c_str() + 4;
		}*/

		if (name.find("中") == name.size() - 2)
		{
			name = name.substr(0, name.size() - 2);
		}
		

		write_node(sw, "name", name);

		arr.erase(arr.begin());
		arr.erase(arr.begin());
		string val = join_string(arr, ',');

		write_new_line(sw, nTab);
		write_node(sw, "allow_actions", val);

		write_new_line(sw, --nTab);
		end_tag(sw, filename);
	}

	write_new_line(sw, --nTab);
	end_tag(sw, "root");
}

template <typename TOutStream>
void string_table_to_error_xml(string const & filenameCSV, vector<vector<string> > const & table, TOutStream & sw)
{
	unsigned int startPos = filenameCSV.find_last_of(":/\\");
	unsigned int endPos = filenameCSV.find_last_of('.');

	string filename = string(filenameCSV.c_str() + startPos + 1, filenameCSV.c_str() + endPos); 
	int nTab = 0;
	write_version(sw);

	write_new_line(sw, nTab++);
	begin_tag(sw, "root");
	// 跳过两行
	int const skipLine = 2;	
	for (int row = 1; row <= (int)table.size(); ++row)
	{
		if (row <= skipLine)
			continue;
		write_new_line(sw, nTab++);
		begin_tag(sw, filename);

		vector<string> arr = extract_error_cols(join_string(table[row-1], ','));

		write_new_line(sw, nTab);
		write_node(sw, "id", arr.front());
		write_new_line(sw, nTab);
		string name = *(arr.begin() + 1);
		//name.
		if (name.find("开始") == 0)
		{
			name = name.c_str() + 4;
		}

		/*if (name.find("使用") == 0)
		{
			name = name.c_str() + 4;
		}*/

		if (name.find("中") == name.size() - 2)
		{
			name = name.substr(0, name.size() - 2);
		}
		

		write_node(sw, "name", name);

		arr.erase(arr.begin());
		arr.erase(arr.begin());
		string val = join_string(arr, ',');

		write_new_line(sw, nTab);
		write_node(sw, "allow_actions", val);

		write_new_line(sw, --nTab);
		end_tag(sw, filename);
	}

	write_new_line(sw, --nTab);
	end_tag(sw, "root");
}


//
//template <typename TOutStream>
//void allow_actions_csv_to_xml(string const & filenameCSV, TOutStream & sw)
//{
//	ifstream sr;
//	sr.open(filenameCSV.c_str());
//
//	unsigned int startPos = filenameCSV.find_last_of(":/\\");
//	unsigned int endPos = filenameCSV.find_last_of('.');
//
//	string filename = string(filenameCSV.c_str() + startPos + 1, filenameCSV.c_str() + endPos); 
//
//	int const bufLen = 1024;
//	char buf[bufLen] = {0};
//
//	int nTab = 0;
//	write_version(sw);
//
//	write_new_line(sw, nTab++);
//	begin_tag(sw, "root");
//	// 跳过两行
//	int const skipLine = 2;	
//	for (int row = 1; sr.getline(buf, bufLen); ++row)
//	{
//		if (row <= skipLine)
//			continue;
//		write_new_line(sw, nTab++);
//		begin_tag(sw, filename);
//
//		vector<string> arr = extract_cols(buf);
//
//		write_new_line(sw, nTab);
//		write_node(sw, "id", arr.front());
//		write_new_line(sw, nTab);
//
//		string name = *(arr.begin() + 1);
//		//name.
//		if (name.find("开始") == 0)
//		{
//			name = name.c_str() + 4;
//		}
//
//		if (name.find("中") == name.size() - 2)
//		{
//			name = name.substr(0, name.size() - 2);
//		}
//
//		write_node(sw, "name", name);
//
//		arr.erase(arr.begin());
//		arr.erase(arr.begin());
//		string val = join_string(arr, ',');
//
//		write_new_line(sw, nTab);
//		write_node(sw, "allow_actions", val);
//
//		write_new_line(sw, --nTab);
//		end_tag(sw, filename);
//	}
//
//	write_new_line(sw, --nTab);
//	end_tag(sw, "root");
//}

void test_rotate()
{
	vector<vector<string> > table, table2;
	vector<string> arr;

	arr.clear();
	arr.push_back("1");
	arr.push_back("2");
	arr.push_back("3");
	table.push_back(arr);

	arr.clear();
	arr.push_back("1");
	arr.push_back("2");
	arr.push_back("3");
	table.push_back(arr);

	arr.clear();
	arr.push_back("1");
	arr.push_back("2");
	arr.push_back("3");
	table.push_back(arr);

	rotate_string_table(table);

	arr.clear();
	arr.push_back("1");
	arr.push_back("1");
	arr.push_back("1");
	table2.push_back(arr);
	arr.clear();
	arr.push_back("2");
	arr.push_back("2");
	arr.push_back("2");
	table2.push_back(arr);
	arr.clear();
	arr.push_back("3");
	arr.push_back("3");
	arr.push_back("3");
	table2.push_back(arr);

	ThrowIf(table != table2);
}

void main(int argc, char * argv[])
{
	// 测试代码
	test_split();
	test_rotate();


	// 转换csv ==> xml
	string filename = "mutex_actions";
	if (argc > 2)
		filename = argv[1];
	cout << filename << endl;

	string filenameCSV = filename + ".csv";
	string filenameXML = "mutex_actions.xml";
	string errorXML = "mutex_actions_sysmsg.xml";
	
	typedef vector<vector<string> > StringTable;
	StringTable table;
	allow_actions_csv_to_string_table(filenameCSV, table);

	{
		ofstream fout;
		fout.open(filenameXML.c_str());
		//rotate_string_table(table);
		string_table_to_xml(filenameCSV, table, fout);
		//allow_actions_csv_to_xml(filenameCSV, fout);
	}

	{
		// 报错消息提取
		ofstream fMsg;
		fMsg.open(errorXML.c_str());
		string_table_to_error_xml(filenameCSV, table, fMsg);
	}

	cout << endl << "convert ok." << endl;
	cin.get();

}