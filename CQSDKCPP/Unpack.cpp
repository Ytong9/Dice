#include "Unpack.h"

#include <iostream>
#include <string>
#include <utility>
#include <cstring>

using namespace std;
//打印内存数据
void show(void* t, const int len) noexcept
{
	const auto p = static_cast<unsigned char*>(t);
	cout << "{";
	for (auto i = 0; i < len; ++i)
	{
		cout << static_cast<int>(p[i]);
		if (i != len - 1)cout << ", ";
	}
	cout << "}" << endl;
}

//内存翻转
unsigned char* Flip(unsigned char* const str, int len) noexcept
{
	auto f = 0;
	--len;
	while (f < len)
	{
		const auto p = str[len];
		str[len] = str[f];
		str[f] = p;
		++f;
		--len;
	}
	return str;
}

//到字节集...
//在原有的数据基础上操作
template <typename ClassType>
unsigned char* toBin(ClassType& i) noexcept
{
	return Flip(reinterpret_cast<unsigned char*>(&i), sizeof(ClassType));
}

Unpack& Unpack::setData(const char* i, const int len) noexcept
{
	buff.assign(i, i + len);
	return *this;
}

Unpack::Unpack() noexcept = default;

Unpack::Unpack(const char* data) noexcept
{
	setData(data, strlen(data));
}

Unpack::Unpack(std::vector<unsigned char> data) noexcept : buff(std::move(data))
{
}

Unpack::Unpack(const std::string& data) noexcept
{
	setData(data.data(), data.size());
}

Unpack& Unpack::clear() noexcept
{
	buff.clear();
	return *this;
}

int Unpack::len() const noexcept
{
	return buff.size();
}

Unpack& Unpack::add(int i) noexcept
{
	const auto t = toBin<int>(i);
	buff.insert(buff.end(), t, t + sizeof(int));
	return *this;
}

int Unpack::getInt() noexcept
{
	const auto len = sizeof(int);
	if (buff.size() < len)return 0;

	const auto ret = *reinterpret_cast<int*>(Flip(&(buff[0]), len));
	buff.erase(buff.begin(), buff.begin() + len);
	return ret;
}

Unpack& Unpack::add(long long i) noexcept
{
	const auto t = toBin<long long>(i);
	buff.insert(buff.end(), t, t + sizeof(long long));
	return *this;
}

long long Unpack::getLong() noexcept
{
	const auto len = sizeof(long long);
	if (buff.size() < len)return 0;

	const auto ret = *reinterpret_cast<long long*>(Flip(&(buff[0]), len));
	buff.erase(buff.begin(), buff.begin() + len);
	return ret;
}

Unpack& Unpack::add(short i) noexcept
{
	const auto t = toBin<short>(i);
	buff.insert(buff.end(), t, t + sizeof(short));
	return *this;
}

short Unpack::getshort() noexcept
{
	const auto len = sizeof(short);
	if (buff.size() < len)return 0;

	const auto ret = *reinterpret_cast<short*>(Flip(&(buff[0]), len));
	buff.erase(buff.begin(), buff.begin() + len);
	return ret;
}

Unpack& Unpack::add(const unsigned char* i, const short len) noexcept
{
	if (len < 0)
		return *this;
	add(len);
	if (len)
		buff.insert(buff.end(), i, i + len);
	return *this;
}

std::vector<unsigned char> Unpack::getchars() noexcept
{
	const auto len = getshort();
	if (buff.size() < static_cast<size_t>(len))return vector<unsigned char>();

	auto tep = vector<unsigned char>(buff.begin(), buff.begin() + len);
	buff.erase(buff.begin(), buff.begin() + len);
	return tep;
}

Unpack& Unpack::add(string i) noexcept
{
	if (i.empty()) //字符串长度为0,直接放入长度0
	{
		add(static_cast<short>(0));
		return *this;
	}
	if (i.size() > 32767) //字符串长度超出限制,
	{
		i = i.substr(0, 32767);
	}

	add(reinterpret_cast<const unsigned char*>(i.data()), static_cast<short>(i.size()));

	return *this;
}

string Unpack::getstring() noexcept
{
	auto tep = getchars();
	if (tep.empty())return "";

	tep.push_back(static_cast<char>(0));
	return string(reinterpret_cast<char*>(&tep[0]));
}

Unpack& Unpack::add(Unpack& i)noexcept
{
	add(i.getAll());
	return *this;
}

Unpack Unpack::getUnpack() noexcept { return Unpack(getchars()); }

std::string Unpack::getAll() noexcept
{
	string ret(buff.begin(), buff.end());
	return ret;
}

void Unpack::show() noexcept
{
	string out;
	auto len = 0;
	for (auto c : buff)
	{
		out.append(to_string(static_cast<int>(c))).append(", ");
		++len;
	}
	out = to_string(len).append("{").append(out.substr(0, out.size() - 2)).append("}");
	cout << out.data() << endl;
}
