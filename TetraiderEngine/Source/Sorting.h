#pragma once

#ifndef SORTING_H
#define SORTING_H

using namespace std;

namespace Sorting {
	template <typename Element, typename CompareFunc>
	void InsertionSort(vector<Element>& _array, CompareFunc _compareFunc) {
		int length = _array.size();
		int j;
		Element temp;

		for (int i = 0; i < length; ++i) {
			j = i;

			while (j > 0 && _compareFunc(_array[j], _array[j - 1])) {
				temp = _array[j];
				_array[j] = _array[j - 1];
				_array[j - 1] = temp;
				j--;
			}
		}
	}


	template <typename Element, typename CompareFunc>
	void TopDownMergeSort(vector<Element>& _array, CompareFunc _compareFunc) {
		vector<Element> workArray = vector<Element>(_array);
		_TopDownSplitMerge(workArray, 0, _array.size(), _array, _compareFunc);
	}

	template <typename Element, typename CompareFunc>
	static void _TopDownSplitMerge(vector<Element>& b, int begin, int end, vector<Element>& a, CompareFunc _compareFunc) {
		if (end - begin < 2)	
			return;

		int middle = (end + begin) / 2;

		_TopDownSplitMerge(a, begin, middle, b, _compareFunc);
		_TopDownSplitMerge(a, middle, end, b, _compareFunc);

		_TopDownMerge(b, begin, middle, end, a, _compareFunc);
	}

	template <typename Element, typename CompareFunc>
	static void _TopDownMerge(vector<Element>& a, int begin, int middle, int end, vector<Element>& b, CompareFunc _compareFunc)
	{
		int i = begin, j = middle;

		for (int k = begin; k < end; ++k) {
			
			if (i < middle && (j >= end || _compareFunc(a[i], a[j]))) {
				b[k] = a[i];
				++i;
			}
			else {
				b[k] = a[j];
				++j;
			}
		}
	}
}

#endif