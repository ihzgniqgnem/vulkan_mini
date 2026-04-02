#pragma once
#include <type_traits>

#include <gtest/gtest.h>
#include "../../header/utils/named_array.h"

namespace vkm_test::utils {
	TEST(NamedArray, basic_construction) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(arr.size(), 5);
	}
	
	TEST(NamedArray, default_value_init) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(arr[ArrayIndex::Index1], 0);
		EXPECT_EQ(arr[ArrayIndex::Index2], 0);
		EXPECT_EQ(arr[ArrayIndex::Index3], 0);
	}
	
	TEST(NamedArray, write_and_read) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		arr[ArrayIndex::Index1] = 10;
		arr[ArrayIndex::Index2] = 20;
		arr[ArrayIndex::Index3] = 30;
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 10);
		EXPECT_EQ(arr[ArrayIndex::Index2], 20);
		EXPECT_EQ(arr[ArrayIndex::Index3], 30);
	}
	
	TEST(NamedArray, copy_operations) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr1;
		arr1[ArrayIndex::Index1] = 100;
		arr1[ArrayIndex::Index2] = 200;
		
		vkm::utils::NamedArray<ArrayIndex, int> arr2(arr1);
		EXPECT_EQ(arr2[ArrayIndex::Index1], 100);
		EXPECT_EQ(arr2[ArrayIndex::Index2], 200);
		
		vkm::utils::NamedArray<ArrayIndex, int> arr3;
		arr3 = arr1;
		EXPECT_EQ(arr3[ArrayIndex::Index1], 100);
		EXPECT_EQ(arr3[ArrayIndex::Index2], 200);
	}
	
	TEST(NamedArray, move_operations) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr1;
		arr1[ArrayIndex::Index1] = 100;
		arr1[ArrayIndex::Index2] = 200;
		
		vkm::utils::NamedArray<ArrayIndex, int> arr2(std::move(arr1));
		EXPECT_EQ(arr2[ArrayIndex::Index1], 100);
		EXPECT_EQ(arr2[ArrayIndex::Index2], 200);
		
		vkm::utils::NamedArray<ArrayIndex, int> arr3;
		arr3 = std::move(arr2);
		EXPECT_EQ(arr3[ArrayIndex::Index1], 100);
		EXPECT_EQ(arr3[ArrayIndex::Index2], 200);
	}
	
	TEST(NamedArray, equality_operator) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr1;
		vkm::utils::NamedArray<ArrayIndex, int> arr2;
		
		EXPECT_TRUE(arr1 == arr2);
		
		arr1[ArrayIndex::Index1] = 10;
		EXPECT_FALSE(arr1 == arr2);
		
		arr2[ArrayIndex::Index1] = 10;
		EXPECT_TRUE(arr1 == arr2);
	}
	
	TEST(NamedArray, const_access) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		arr[ArrayIndex::Index1] = 42;
		
		const auto& const_arr = arr;
		EXPECT_EQ(const_arr[ArrayIndex::Index1], 42);
	}
	
	TEST(NamedArray, batch_read_const) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		arr[ArrayIndex::Index1] = 1;
		arr[ArrayIndex::Index2] = 2;
		arr[ArrayIndex::Index3] = 3;
		
		const auto& const_arr = arr;
		auto batch = const_arr[{ArrayIndex::Index1, ArrayIndex::Index3}];
		
		EXPECT_EQ(batch[0], 1);
		EXPECT_EQ(batch[1], 3);
		EXPECT_EQ(batch.size(), 2);
	}
	
	TEST(NamedArray, batch_write) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		auto batch = arr[{ArrayIndex::Index1, ArrayIndex::Index2, ArrayIndex::Index3}];
		batch = {10, 20, 30};
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 10);
		EXPECT_EQ(arr[ArrayIndex::Index2], 20);
		EXPECT_EQ(arr[ArrayIndex::Index3], 30);
	}
	
	TEST(NamedArray, proxy_array_assignment) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr1;
		arr1[ArrayIndex::Index1] = 100;
		arr1[ArrayIndex::Index2] = 200;
		
		vkm::utils::NamedArray<ArrayIndex, int> arr2;
		arr2[{ArrayIndex::Index1, ArrayIndex::Index2}] = arr1[{ArrayIndex::Index1, ArrayIndex::Index2}];
		
		EXPECT_EQ(arr2[ArrayIndex::Index1], 100);
		EXPECT_EQ(arr2[ArrayIndex::Index2], 200);
	}
	
	TEST(NamedArray, type_traits) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		using NamedArrayType = vkm::utils::NamedArray<ArrayIndex, int>;
		
		EXPECT_TRUE(std::is_default_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_copy_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_copy_assignable_v<NamedArrayType>);
		EXPECT_TRUE(std::is_move_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_move_assignable_v<NamedArrayType>);
		EXPECT_TRUE(std::is_destructible_v<NamedArrayType>);
		
		EXPECT_TRUE(std::is_trivially_copyable_v<NamedArrayType>);
		
		EXPECT_TRUE(std::is_nothrow_default_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_nothrow_copy_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_nothrow_copy_assignable_v<NamedArrayType>);
		EXPECT_TRUE(std::is_nothrow_move_constructible_v<NamedArrayType>);
		EXPECT_TRUE(std::is_nothrow_move_assignable_v<NamedArrayType>);
		EXPECT_TRUE(std::is_nothrow_destructible_v<NamedArrayType>);
		
		EXPECT_TRUE(std::is_standard_layout_v<NamedArrayType>);
	}
	
	TEST(NamedArray, hash) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr1;
		auto hash1 = std::hash<decltype(arr1)>{}(arr1);
		
		arr1[ArrayIndex::Index1] = 42;
		auto hash2 = std::hash<decltype(arr1)>{}(arr1);
		
		arr1[ArrayIndex::Index1] = 0;
		auto hash3 = std::hash<decltype(arr1)>{}(arr1);
		
		EXPECT_NE(hash1, hash2);
		EXPECT_EQ(hash1, hash3);
		
		vkm::utils::NamedArray<ArrayIndex, int> arr2;
		EXPECT_EQ(std::hash<decltype(arr1)>{}(arr1), std::hash<decltype(arr2)>{}(arr2));
	}
	
	TEST(NamedArray, different_types) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		vkm::utils::NamedArray<ArrayIndex, double> arr_double;
		arr_double[ArrayIndex::Index1] = 3.14;
		arr_double[ArrayIndex::Index2] = 2.71;
		
		EXPECT_DOUBLE_EQ(arr_double[ArrayIndex::Index1], 3.14);
		EXPECT_DOUBLE_EQ(arr_double[ArrayIndex::Index2], 2.71);
		
		vkm::utils::NamedArray<ArrayIndex, char> arr_char;
		arr_char[ArrayIndex::Index1] = 'A';
		arr_char[ArrayIndex::Index2] = 'B';
		
		EXPECT_EQ(arr_char[ArrayIndex::Index1], 'A');
		EXPECT_EQ(arr_char[ArrayIndex::Index2], 'B');
	}
	
	TEST(NamedArray, large_array) {
		enum class ArrayIndex {
			ArrayMax = 1000
		};
		
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(arr.size(), 1000);
		
		arr[static_cast<ArrayIndex>(0)] = 1;
		arr[static_cast<ArrayIndex>(999)] = 1000;
		
		EXPECT_EQ(arr[static_cast<ArrayIndex>(0)], 1);
		EXPECT_EQ(arr[static_cast<ArrayIndex>(999)], 1000);
	}
	
	template<typename T>
	concept has_valid_named_array = requires{ std::declval<vkm::utils::NamedArray<T, int>>();};
	
	TEST(NamedArray, compiling_wrong_parameter) {
		class ArrayIndex1 {};
		EXPECT_FALSE(has_valid_named_array<ArrayIndex1>);
		
		enum class ArrayIndex2 {
			Index1
		};
		EXPECT_FALSE(has_valid_named_array<ArrayIndex2>);
		
		enum class ArrayIndex3 {
			ArrayMax
		};
		EXPECT_FALSE(has_valid_named_array<ArrayIndex3>);
		
		enum class ArrayIndex4 {
			Index1, Index2,
			ArrayMax
		};
		EXPECT_TRUE(has_valid_named_array<ArrayIndex4>);
	}
}
