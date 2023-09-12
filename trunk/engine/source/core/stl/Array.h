 
/*****************************************************************************************************/
// @author hannibal
// @date   2015/04/29
// @brief  数组，支持动态扩充
/*****************************************************************************************************/
#pragma once

#include "core/BaseType.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
template <class T>
class Array Final : public object
{
	DISALLOW_COPY_ASSIGN(Array);
	
public:
	Array() :_data(0), m_len(0), m_cap(0)
	{
	}
	explicit Array(int size) : _data(0), m_len(0), m_cap(0)
	{
		SetSize(size);
	}
	~Array()
	{
		if (_data)
		{
			DeleteArray(_data, m_cap);
			_data = nullptr;
		}
	}
	void Add(const T & item)
	{
		if (m_len + 1 > m_cap)
			SetCapacity(m_len + 1);
		_data[m_len++] = item;
	}
	void SetSize(int size)
	{
		if (size > m_cap)
			SetCapacity(size);
		m_len = size;
	}
	T & operator[](int i)
	{
		return _data[i];
	}
	T* Begin()
	{
		return _data;
	}
	T* End()
	{
		return _data + m_len;
	}
	const T* Begin() const
	{
		return _data;
	}
	const T* End() const
	{
		return _data + m_len;
	}
	int Size() const
	{
		return m_len;
	}
	T MaxValue()const
	{
		T max_value = std::numeric_limits<T>::min();
		for (int i = 0; i < m_len; ++i)
		{
			if (_data[i] > max_value)
			{
				max_value = _data[i];
			}
		}
		return max_value;
	}
	//最大位数
	int MaxBit()
	{
		int d = 1; //保存最大的位数
		int p = 10;
		for (int i = 0; i < m_len; ++i)
		{
			while (_data[i] >= p)
			{
				p *= 10;
				++d;
			}
		}
		return d;
	}
	const T & operator[](int i) const
	{
		return _data[i];
	}

public://排序
	/*
		冒泡排序
		介绍:
			冒泡排序是一种简单的排序算法。它重复地走访过要排序的数列，一次比较两个元素，如果它们的顺序错误就把它们交换过来。
			走访数列的工作是重复地进行直到没有再需要交换，也就是说该数列已经排序完成
		算法描述:
			1.比较相邻的元素。如果第一个比第二个大，就交换它们两个；
			2.对每一对相邻元素作同样的工作，从开始第一对到结尾的最后一对，这样在最后的元素应该会是最大的数；
			3.针对所有的元素重复以上的步骤，除了最后一个；
			4.重复步骤1~3，直到排序完成。
	*/
	void BubbleSort();
	/*
		选择排序
		介绍:
			选择排序是一种简单直观的排序算法。它的工作原理：首先在未排序序列中找到最小（大）元素，存放到排序序列的起始位置。
			然后，再从剩余未排序元素中继续寻找最小（大）元素，然后放到已排序序列的末尾。以此类推，直到所有元素均排序完毕
		算法描述:
			1.初始状态：无序区为R[1..n]，有序区为空；
			2.第i趟排序(i=1,2,3…n-1)开始时，当前有序区和无序区分别为R[1..i-1]和R(i..n）。该趟排序从当前无序区中-选出关键字最小的记录 R[k]，
			  将它与无序区的第1个记录R交换，使R[1..i]和R[i+1..n)分别变为记录个数增加1个的新有序区和记录个数减少1个的新无序区；
			3.n-1趟结束，数组有序化了。
	*/
	void SelectionSort();
	/*
		插入排序
		介绍:
			插入排序的算法描述是一种简单直观的排序算法。它的工作原理是通过构建有序序列，对于未排序数据，在已排序序列中从后向前扫描，找到相应位置并插入
		算法描述:
			一般来说，插入排序都采用in-place在数组上实现。具体算法描述如下：
			1.从第一个元素开始，该元素可以认为已经被排序；
			2.取出下一个元素，在已经排序的元素序列中从后向前扫描；
			3.如果该元素（已排序）大于新元素，将该元素移到下一位置；
			4.重复步骤3，直到找到已排序的元素小于或者等于新元素的位置；
			5.将新元素插入到该位置后；
			重复步骤2~5
	*/
	void InsertionSort();
	/*
		希尔排序
		介绍:
			1959年Shell发明，第一个突破O(n2)的排序算法，是简单插入排序的改进版。它与插入排序的不同之处在于，它会优先比较距离较远的元素。希尔排序又叫缩小增量排序
		算法描述:
			先将整个待排序的记录序列分割成为若干子序列分别进行直接插入排序，具体算法描述：
			1.选择一个增量序列t1，t2，…，tk，其中ti>tj，tk=1；
			2.按增量序列个数k，对序列进行k 趟排序；
			3.每趟排序，根据对应的增量ti，将待排序列分割成若干长度为m 的子序列，分别对各子表进行直接插入排序。
			  仅增量因子为1 时，整个序列作为一个表来处理，表长度即为整个序列的长度。
	*/
	void ShellSort();
	/*
		快速排序
		介绍:
			快速排序的基本思想：通过一趟排序将待排记录分隔成独立的两部分，其中一部分记录的关键字均比另一部分的关键字小，则可分别对这两部分记录继续进行排序，以达到整个序列有序。
		算法描述:
			1.从数列中挑出一个元素，称为 “基准”（pivot）；
			2.重新排序数列，所有元素比基准值小的摆放在基准前面，所有元素比基准值大的摆在基准的后面（相同的数可以到任一边）。在这个分区退出之后，该基准就处于数列的中间位置。这个称为分区（partition）操作；
			3.递归地（recursive）把小于基准值元素的子数列和大于基准值元素的子数列排序。
	*/
	void QuickSort();
	int  Parition(int low, int high);
	void QuickSort(int low, int high);
	/*
		归并排序
		介绍:
			归并排序是建立在归并操作上的一种有效的排序算法。该算法是采用分治法（Divide and Conquer）的一个非常典型的应用。
			将已有序的子序列合并，得到完全有序的序列；即先使每个子序列有序，再使子序列段间有序。若将两个有序表合并成一个有序表，称为2-路归并。 
		算法描述:
			1.把长度为n的输入序列分成两个长度为n/2的子序列；
			2.对这两个子序列分别采用归并排序；
			3.将两个排序好的子序列合并成一个最终的排序序列。
	*/
	void MergeSort();
	void MergeSort(T* temp_arr, int l, int r);
	void Merge(T* temp_arr, int l, int q, int r);
	/*
		堆排序
		介绍:
			堆排序是指利用堆这种数据结构所设计的一种排序算法。堆积是一个近似完全二叉树的结构，并同时满足堆积的性质：即子结点的键值或索引总是小于（或者大于）它的父节点。
		算法描述:
			1.将初始待排序关键字序列(R1,R2….Rn)构建成大顶堆，此堆为初始的无序区；
			2.将堆顶元素R[1]与最后一个元素R[n]交换，此时得到新的无序区(R1,R2,……Rn-1)和新的有序区(Rn),且满足R[1,2…n-1]<=R[n]；
			3.由于交换后新的堆顶R[1]可能违反堆的性质，因此需要对当前无序区(R1,R2,……Rn-1)调整为新堆，然后再次将R[1]与无序区最后一个元素交换，得到新的无序区(R1,R2….Rn-2)和新的有序区(Rn-1,Rn)。
			  不断重复此过程直到有序区的元素个数为n-1，则整个排序过程完成。
		堆：
			堆是具有以下性质的完全二叉树：每个结点的值都大于或等于其左右孩子结点的值，称为大顶堆；或者每个结点的值都小于或等于其左右孩子结点的值，称为小顶堆。
			如果用数组表示堆，该数组从逻辑上讲就是一个堆结构，我们用简单的公式来描述一下堆的定义就是：
				大顶堆：arr[i] >= arr[2i+1] && arr[i] >= arr[2i+2]
				小顶堆：arr[i] <= arr[2i+1] && arr[i] <= arr[2i+2]
		完全二叉树:
					1
				  /   \
				 2	   3
				/ \   / \
			   4   5  6  7
			  /
			  8
	*/
	void HeapSort();
	void AdjustHeap(int len, int index);
	/*
		计数排序
		介绍:
			计数排序不是基于比较的排序算法，其核心在于将输入的数据值转化为键存储在额外开辟的数组空间中。 作为一种线性时间复杂度的排序，计数排序要求输入的数据必须是有确定范围的整数
		算法描述:
			1.找出待排序的数组中最大和最小的元素；
			2.统计数组中每个值为i的元素出现的次数，存入数组C的第i项；
			3.对所有的计数累加（从C中的第一个元素开始，每一项和前一项相加）；
			4.反向填充目标数组：将每个元素i放在新数组的第C(i)项，每放一个元素就将C(i)减去1。
	*/
	void CountingSort();
	/*
		桶排序
		介绍:
			桶排序是计数排序的升级版。它利用了函数的映射关系，高效与否的关键就在于这个映射函数的确定。
			桶排序的工作的原理：假设输入数据服从均匀分布，将数据分到有限数量的桶里，每个桶再分别排序（有可能再使用别的排序算法或是以递归方式继续使用桶排序进行排）。
		算法描述:
			1.设置一个定量的数组当作空桶；
			2.遍历输入数据，并且把数据一个一个放到对应的桶里去；
			3.对每个不是空的桶进行排序；
			4.从不是空的桶里把排好序的数据拼接起来。
	*/
	void BucketSort();
	/*
		基数排序
		介绍:
			基数排序是按照低位先排序，然后收集；再按照高位排序，然后再收集；依次类推，直到最高位。有时候有些属性是有优先级顺序的，先按低优先级排序，再按高优先级排序。
			最后的次序就是高优先级高的在前，高优先级相同的低优先级高的在前。
		算法描述:
			1.取得数组中的最大数，并取得位数；
			2.arr为原始数组，从最低位开始取每个位组成radix数组；
			3.对radix进行计数排序（利用计数排序适用于小范围数的特点）；
	*/
	void RadixSort();

private:
	void SetCapacity(int cap);
	// 元素交换
	void Swap(int i, int j);

private:
	T*		_data;
	int		m_len;
	int		m_cap;
};
template <class T>
void Array<T>::BubbleSort()
{
	for (int i = 0; i < m_len - 1; i++)
	{
		for (int j = 0; j < m_len - 1 - i; j++)
		{
			if (_data[j] > _data[j + 1])		// 相邻元素两两对比
			{
				Swap(j, j + 1);
			}
		}
	}
}
template <class T>
void Array<T>::SelectionSort()
{
	int min_index = -1;
	for (int i = 0; i < m_len - 1; i++)
	{
		min_index = i;
		for (int j = i + 1; j < m_len; j++)
		{
			if (_data[j] < _data[min_index])	// 寻找最小的数
			{
				min_index = j;					// 将最小数的索引保存
			}
		}
		Swap(i, min_index);
	}
}
template <class T>
void Array<T>::InsertionSort()
{
	T current;
	for (int i = 1; i < m_len; i++)
	{
		int j  = i;
		current = _data[i];
		while (j - 1 >= 0 && _data[j - 1] > current)
		{
			_data[j] = _data[j - 1];
			j--;
		}
		_data[j] = current;
	}
}
template <class T>
void Array<T>::ShellSort()
{
	for (int gap = (int)::floor(m_len / 2); gap > 0; gap = (int)::floor(gap / 2))
	{
		for (int i = gap; i < m_len; i++)
		{
			int j = i;
			T current = _data[i];
			while (j - gap >= 0 && _data[j - gap] > current)
			{
				_data[j] = _data[j - gap];
				j = j - gap;
			}
			_data[j] = current;
		}
	}
}
template <class T>
void Array<T>::QuickSort()
{
	QuickSort(0, m_len - 1);
}
template <class T>
int Array<T>::Parition(int low, int high)
{
	int pivot = _data[high];
	int i = low;
	for (int j = low; j < high; ++j)
	{
		//j指向当前遍历元素，如果大于等于pivot，继续向前
		//如果小于当前元素，则和i指向的元素交换
		if (_data[j] < pivot)
		{
			Swap(j, i);
			i++;
		}
	}
	Swap(i, high);
	return i;

}
template <class T>
void Array<T>::QuickSort(int low, int high)
{
	if (low < high)
	{
		int q = Parition(low, high);
		QuickSort(low, q - 1);
		QuickSort(q + 1, high);
	}
}
template <class T>
void Array<T>::MergeSort()
{
	T* arr = NewArray<T>(m_len);
	MergeSort(arr, 0, m_len - 1);
	DeleteArray(arr, m_len);
}
template <class T>
void Array<T>::MergeSort(T* temp_arr, int l, int r)
{
	if (l == r)return;  //递归基是让数组中的每个数单独成为长度为1的区间
	int q = (l + r) / 2;
	MergeSort(temp_arr, l, q);
	MergeSort(temp_arr, q + 1, r);
	Merge(temp_arr, l, q, r);
}
template <class T>
void Array<T>::Merge(T* temp_arr, int l, int q, int r)
{
	int n = r - l + 1;//临时数组存合并后的有序序列
	int i = 0;
	int left = l;
	int right = q + 1;
	while (left <= q && right <= r)
		temp_arr[i++] = _data[left] <= _data[right] ? _data[left++] : _data[right++];
	while (left <= q)
		temp_arr[i++] = _data[left++];
	while (right <= r)
		temp_arr[i++] = _data[right++];
	for (int j = 0; j < n; ++j)
		_data[l + j] = temp_arr[j];
}
template <class T>
void Array<T>::HeapSort()
{
	// 构建大根堆（从最后一个非叶子节点向上）
	for (int i = m_len / 2 - 1; i >= 0; i--)
	{
		AdjustHeap(m_len, i);
	}

	// 调整大根堆
	for (int i = m_len - 1; i >= 1; i--)
	{
		Swap(0, i);						// 将当前最大的放置到数组末尾
		AdjustHeap(i, 0);				// 将未完成排序的部分继续进行堆排序
	}
}
// 递归方式构建大根堆(len是arr的长度，index是第一个非叶子节点的下标)
template <class T>
void Array<T>::AdjustHeap(int len, int index)
{
	int left = 2 * index + 1;			// index的左子节点
	int right = 2 * index + 2;			// index的右子节点

	int max_idx = index;
	if (left<len && _data[left] > _data[max_idx]) max_idx = left;
	if (right<len && _data[right] > _data[max_idx]) max_idx = right;
	if (max_idx != index)
	{
		Swap(max_idx, index);
		AdjustHeap(len, max_idx);
	}
}
template <class T>
void Array<T>::CountingSort()
{
	T max_value = MaxValue() + 1;
	T* arr = NewArray<T>(max_value);
	::memset(arr, 0, sizeof(T) * max_value);

	int sorted_index = 0;
	for (int i = 0; i < m_len; i++) 
	{
		arr[_data[i]]++;
	}

	for (int j = 0; j < max_value; j++)
	{
		while (arr[j] > 0)
		{
			_data[sorted_index++] = j;
			arr[j]--;
		}
	}

	DeleteArray(arr, max_value);
}
template <class T>
void Array<T>::BucketSort()
{

}
template <class T>
void Array<T>::RadixSort()
{
	T* arr = NewArray<T>(m_len);
	::memset(arr, 0, sizeof(T) * m_len);

	int d = MaxBit();
	int count[10]; //计数器
	int i, j, k;
	int radix = 1;
	for (i = 1; i <= d; i++)					//进行d次排序
	{
		for (j = 0; j < 10; j++)
			count[j] = 0;						//每次分配前清空计数器
		for (j = 0; j < m_len; j++)
		{
			k = (_data[j] / radix) % 10;		//统计每个桶中的记录数
			count[k]++;
		}
		for (j = 1; j < 10; j++)
			count[j] = count[j - 1] + count[j]; //将tmp中的位置依次分配给每个桶
		for (j = m_len - 1; j >= 0; j--)		//将所有桶中记录依次收集到tmp中
		{
			k = (_data[j] / radix) % 10;
			arr[count[k] - 1] = _data[j];
			count[k]--;
		}
		for (j = 0; j < m_len; j++)				//将临时数组的内容复制到data中
			_data[j] = arr[j];
		::memcpy(_data, arr, sizeof(T) * m_len);
		radix = radix * 10;
	}
	DeleteArray(arr, m_len);
}
template <class T>
void Array<T>::SetCapacity(int cap)
{
	++cap;
	if (cap < 8)
		cap = 8;
	else if (cap < m_cap * 2)
		cap = m_cap * 2;
	int old_cap = m_cap;
	m_cap = cap;

	T* data = NewArray<T>(cap);
	for (int i = 0; i < m_len; ++i)
		data[i] = _data[i];
	if (old_cap)DeleteArray(_data, old_cap);
	_data = data;
}
template <class T>
void Array<T>::Swap(int i, int j)
{
	auto temp = _data[j];
	_data[j] = _data[i];
	_data[i] = temp;
}
DC_END_NAMESPACE

/*
	排序方法	时间复杂度(平均)	时间复杂度(最坏)	时间复杂度(最好)	空间复杂度	稳定性
	冒泡排序	O(n^2)				O(n^2)				O(n)				O(1)		稳定
	插入排序	O(n^2)				O(n^2)				O(n)				O(1)		稳定
	希尔排序	O(n^1.3)			O(n^2)				O(n)				O(1)		不稳定
	选择排序	O(n^2)				O(n^2)				O(n^2)				O(1)		不稳定
	快速排序	O(nlogn)			O(n^2)				O(nlogn)			O(nlogn)	不稳定
	堆排序		O(nlogn)			O(nlogn)			O(nlogn)			O(1)		不稳定
	归并排序	O(nlogn)			O(nlogn)			O(nlogn)			O(n)		稳定

	计数排序	O(n+k)				O(n+k)				O(n+k)				O(n+k)		稳定
	桶排序		O(n+k)				O(n^2)				O(n)				O(n+k)		稳定
	基数排序	O(n*k)				O(n*k)				O(n*k)				O(n+k)		稳定
*/

/*
	快排只需分配O(1)的空间，但是递归logn次将使空间复杂度为O（logn），最坏情况递归n次，将需要O(n)的空间复杂度
*/