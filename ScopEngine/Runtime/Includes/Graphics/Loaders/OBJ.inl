#pragma once
#include <Graphics/Loaders/OBJ.h>

namespace Scop
{
	template<typename T>
	inline std::istream& operator>>(std::istream& in, std::vector<T>& vec)
	{
		T temp;
		if(in >> temp) 
			vec.push_back(temp);
		return in;
	}

	template<typename T>
	inline std::istream& operator>>(std::istream& in, std::set<T>& vec)
	{
		T temp;
		if(in >> temp) 
			vec.insert(temp);
		return in;
	}

	inline std::istream& operator>>(std::istream& in, ObjData::FaceVertex& f)
	{
		std::int32_t val;
		if(in >> f.v)
		{
			if(in.peek() == '/')
			{
				in.get();
				in >> f.t;
				in.clear();
				if(in.peek() == '/')
				{
					in.get();
					in >> f.n;
					in.clear();
				}
			}
			in.clear();
			f.v--;
			f.t--;
			f.n--;
		}
		return in;
	}
}
