 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  可变数组
/*****************************************************************************************************/
#pragma once

#include "core/memory/STLAllocate.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template<class V>
class Vector
{
	DECLARE_ALLOCATOR;
public:
	typedef std::function<bool(const V&, const V&)> SortFunc;
	typedef typename std::vector<V, STLAlloc<V>>::iterator iterator;
	typedef typename std::vector<V, STLAlloc<V>>::reverse_iterator reverse_iterator;
	typedef typename std::vector<V, STLAlloc<V>>::const_iterator const_iterator;
	typedef typename std::vector<V, STLAlloc<V>>::const_reverse_iterator const_reverse_iterator;

public:
	Vector() { }
	Vector(int size) :m_vector(size) {}
	Vector(int size, const V& v) :m_vector(size, v) {}
	Vector(const Vector& from) :m_vector(from.m_vector) {}
	Vector(std::initializer_list<V> list):m_vector(list) {}
	Vector(const_iterator begin, const_iterator end):m_vector(begin, end) { }

	V& operator [](int index) { return m_vector[index]; }
	const V& operator [](int index) const { return m_vector[index]; }
	Vector& operator =(const Vector& from);
	V* Data()const { return const_cast<V*>(m_vector.data()); }
	const V* CData()const { return m_vector.data(); }

	void Add(const V& v) { m_vector.push_back(v); }
	void AddRange(const V* vs, int count);
	void AddRange(std::initializer_list<V> list) { m_vector.insert(m_vector.end(), list.begin(), list.end()); }
	void AddRange(const Vector<V>& vs) { m_vector.insert(m_vector.end(), vs.begin(), vs.end()); }
	void AddRange(const_iterator begin, const_iterator end) { return m_vector.insert(m_vector.end(), begin, end); }
	byte* Bytes(int index = 0) const { return (byte*)&m_vector[index]; }
	int Capacity()const { return (int)m_vector.capacity(); }
	bool Contains(const V& v) const;
	void Clear() { m_vector.clear(); }
	V& First() { return m_vector.front(); }
	const V& First() const { return m_vector.front(); }
	void ForEach(std::function<void(V)> fun);//使用方式：vec.ForEach([](int val)->void {Debuger::Log("%d", val);});
	void ForEachTrue(std::function<bool(V)> fun);
	int IndexOf(const V& v) const;
	int IndexOf(const V& v, int index) const;
	int IndexOf(const V& v, int index, int count) const;
	iterator Insert(const_iterator it, const V& v) { return m_vector.insert(it, v); }
	bool IsEmpty() const { return m_vector.empty(); }
	V& Last() { return m_vector.back(); }
	const V& Last() const { return m_vector.back(); }
	int LastIndexOf(const V& v) const;
	int LastIndexOf(const V& v, int index) const;
	int LastIndexOf(const V& v, int index, int count) const;
	void Reserve(int size) { m_vector.reserve(size); }
	void Resize(int size) { m_vector.resize(size); }
	void Resize(int size, const V& v) { m_vector.resize(size, v); }
	iterator Remove(const_iterator it) { return m_vector.erase(it); }
	bool Remove(const V& v);
	void RemoveAt(int index) { m_vector.erase(m_vector.begin() + index); }
	void RemoveRange(int index, int count) { m_vector.erase(m_vector.begin() + index, m_vector.begin() + index + count); }
	void Reverse() { std::reverse(m_vector.begin(), m_vector.end()); }
	void Reverse(int index, int count);
	int Size() const { return (int)m_vector.size(); }
	int SizeInBytes() const { return sizeof(V) * Size(); }
	void Sort() { std::sort(m_vector.begin(), m_vector.end()); }//不稳定排序
	void Sort(SortFunc func) { std::sort(m_vector.begin(), m_vector.end(), func); }
	void Sort(int index, int count);
	void Sort(int index, int count, SortFunc func);//如果存在相等元素，则比较算法必须实现严格严格弱排序(在运算符<和>中实现的"小于"或"大于")，否则会报错
	void StableSort() { std::stable_sort(m_vector.begin(), m_vector.end()); }
	void StableSort(SortFunc func) { std::stable_sort(m_vector.begin(), m_vector.end(), func); }
	void StableSort(int index, int count);
	void StableSort(int index, int count, SortFunc func);

	iterator begin() { return m_vector.begin(); }
	iterator end() { return m_vector.end(); }
	reverse_iterator rbegin() { return m_vector.rbegin(); }
	reverse_iterator rend() { return m_vector.rend(); }
	const_iterator begin() const { return (const_iterator)m_vector.begin(); }
	const_iterator end() const { return (const_iterator)m_vector.end(); }
	const_reverse_iterator rbegin() const { return (const_reverse_iterator)m_vector.rbegin(); }
	const_reverse_iterator rend() const { return (const_reverse_iterator)m_vector.rend(); }

private:
	int ValidCount(int index, int count)const;

private:
	std::vector<V, STLAlloc<V>> m_vector;
};
template<class V>
Vector<V>& Vector<V>::operator =(const Vector<V>& from)
{
	m_vector.clear();
	AddRange(from);
	return *this;
}
template<class V>
bool Vector<V>::Contains(const V& v) const
{
	for (int i = 0; i < this->Size(); ++i)
	{
		if (m_vector[i] == v)
		{
			return true;
		}
	}
	return false;
}
template<class V>
int Vector<V>::IndexOf(const V& v) const
{
	for (int i = 0; i < this->Size(); ++i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
int Vector<V>::IndexOf(const V& v, int index) const
{
	AssertEx(index >= 0, "");
	if (index >= this->Size())return -1;
	for (int i = index; i < this->Size(); ++i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
int Vector<V>::IndexOf(const V& v, int index, int count) const
{
	AssertEx(index >= 0 && count >= 0, "");
	if (index >= this->Size())return -1;
	count = ValidCount(index, count);
	for (int i = index; i < index + count && i < this->Size(); ++i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
int Vector<V>::LastIndexOf(const V& v) const
{
	for (int i = this->Size() - 1; i >= 0; --i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
int Vector<V>::LastIndexOf(const V& v, int index) const
{
	AssertEx(index >= 0, "");
	if (index >= this->Size())return -1;
	for (int i = this->Size() - 1; i >= index; --i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
int Vector<V>::LastIndexOf(const V& v, int index, int count) const
{
	AssertEx(index >= 0 && count >= 0, "");
	if (index >= this->Size())return -1;
	count = ValidCount(index, count);
	int end_index = std::min(index + count, this->Size());
	for (int i = end_index - 1; i >= index; --i)
	{
		if (m_vector[i] == v)
		{
			return i;
		}
	}
	return -1;
}
template<class V>
void Vector<V>::ForEach(std::function<void(V)> fun)
{
	for (int i = 0; i < this->Size(); ++i)
	{
		fun(m_vector[i]);
	}
}
template<class V>
void Vector<V>::ForEachTrue(std::function<bool(V)> fun)
{
	for (int i = 0; i < this->Size(); ++i)
	{
		if (!fun(m_vector[i]))break;
	}
}
template<class V>
void Vector<V>::AddRange(const V* vs, int count)
{
	if (count > 0)
	{
		auto old_size = m_vector.size();
		m_vector.resize(old_size + count);

		for (int i = 0; i < count; ++i)
		{
			m_vector[old_size + i] = vs[i];
		}
	}
}
template<class V>
bool Vector<V>::Remove(const V& v)
{
	for (int i = 0; i < this->Size(); ++i)
	{
		if (m_vector[i] == v)
		{
			this->RemoveAt(i);
			return true;
		}
	}

	return false;
}
template<class V>
void Vector<V>::Reverse(int index, int count)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = m_vector.begin(); std::advance(it1, index);
	iterator it2 = m_vector.begin(); std::advance(it2, index + count);
	std::reverse(it1, it2);
}
template<class V>
void Vector<V>::Sort(int index, int count, SortFunc func)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = m_vector.begin(); std::advance(it1, index);
	iterator it2 = m_vector.begin(); std::advance(it2, index + count);
	std::sort(it1, it2, func);
}
template<class V>
void Vector<V>::Sort(int index, int count)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = m_vector.begin(); std::advance(it1, index);
	iterator it2 = m_vector.begin(); std::advance(it2, index + count);
	std::sort(it1, it2);
}
template<class V>
void Vector<V>::StableSort(int index, int count, SortFunc func)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = m_vector.begin(); std::advance(it1, index);
	iterator it2 = m_vector.begin(); std::advance(it2, index + count);
	std::stable_sort(it1, it2, func);
}
template<class V>
void Vector<V>::StableSort(int index, int count)
{
	CHECK_RETURN_PTR_VOID(index >= 0 && count >= 0);
	count = ValidCount(index, count);
	iterator it1 = m_vector.begin(); std::advance(it1, index);
	iterator it2 = m_vector.begin(); std::advance(it2, index + count);
	std::stable_sort(it1, it2);
}
template<class V>
int Vector<V>::ValidCount(int index, int count)const
{
	if (index + count > this->Size())
	{
		count = this->Size() - index;
		if (count < 0)count = 0;
	}
	return count;
}
DC_END_NAMESPACE
/*快速排序算法
通过多次比较和交换来实现排序，其排序流程如下：
(1)首先设定一个分界值，通过该分界值将数组分成左右两部分。
(2)将大于或等于分界值的数据集中到数组右边，小于分界值的数据集中到数组的左边。此时，左边部分中各元素都小于或等于分界值，而右边部分中各元素都大于或等于分界值。
(3)然后，左边和右边的数据可以独立排序。对于左侧的数组数据，又可以取一个分界值，将该部分数据分成左右两部分，同样在左边放置较小值，右边放置较大值。右侧的数组数据也可以做类似处理。
(4)重复上述过程，可以看出，这是一个递归定义。通过递归将左侧部分排好序后，再递归排好右侧部分的顺序。当左、右两个部分各数据排序完成后，整个数组的排序也就完成了

排序演示
假设一开始序列{xi}是：5，3，7，6，4，1，0，2，9，10，8。
此时，ref=5，i=1，j=11，从后往前找，第一个比5小的数是x8=2，	因此序列为：2，3，7，6，4，1，0，5，9，10，8。
此时，i=1，j=8，从前往后找，第一个比5大的数是x3=7，			因此序列为：2，3，5，6，4，1，0，7，9，10，8。
此时，i=3，j=8，从第8位往前找，第一个比5小的数是x7=0，		因此序列为：2，3，0，6，4，1，5，7，9，10，8。
此时，i=3，j=7，从第3位往后找，第一个比5大的数是x4=6，		因此序列为：2，3，0，5，4，1，6，7，9，10，8。
此时，i=4，j=7，从第7位往前找，第一个比5小的数是x6=1，		因此序列为：2，3，0，1，4，5，6，7，9，10，8。
此时，i=4，j=6，从第4位往后找，直到第6位才有比5大的数，这时，i=j=6，ref成为一条分界线，它之前的数都比它小，之后的数都比它大，对于前后两部分数，可以采用同样的方法来排序
*/