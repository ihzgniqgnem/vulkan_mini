#include <gtest/gtest.h>
#include "header/utils/named_array.h"

namespace vkm_test::utils {

	TEST(NamedArray, small_array) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(sizeof(arr), sizeof(int) * 3);
	}

	TEST(NamedArray, big_array) {
		enum class ArrayIndex {
			ArrayMax = 10000
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(sizeof(arr), sizeof(int) * 10000);
	}

	TEST(NamedArray, init) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		for (auto i = ArrayIndex::Index1; i < ArrayIndex::ArrayMax; i = static_cast<ArrayIndex>(static_cast<int>(i) + 1)) {
			EXPECT_EQ(arr[i], 0);
		}
	}

	TEST(NamedArray, basic_attr) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
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

	TEST(NamedArray, op_eq) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> x, y;

		EXPECT_TRUE(x == y);
		x[ArrayIndex::Index1] = 42;
		EXPECT_TRUE(x != y);
		y[ArrayIndex::Index1] = 42;
		EXPECT_TRUE(x == y);
	}

	TEST(NamedArray, write) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		arr[ArrayIndex::Index1] = 1;
		EXPECT_EQ(arr[ArrayIndex::Index1], 1);
		
		arr[ArrayIndex::Index2] = 2;
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		
		arr[ArrayIndex::Index8] = 8;
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		
		arr[ArrayIndex::Index9] = 9;
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		
		arr[ArrayIndex::Index10] = 10;
		EXPECT_EQ(arr[ArrayIndex::Index1], 1);
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		EXPECT_EQ(arr[ArrayIndex::Index10], 10);
	}

	TEST(NamedArray, batch_write) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		int values[] = {1, 2, 8, 9, 10};
		arr[{ArrayIndex::Index1, ArrayIndex::Index2, ArrayIndex::Index8, ArrayIndex::Index9, ArrayIndex::Index10}] = values;
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 1);
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		EXPECT_EQ(arr[ArrayIndex::Index10], 10);
		
		int new_values[] = {0, 0};
		arr[{ArrayIndex::Index1, ArrayIndex::Index10}] = new_values;
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 0);
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		EXPECT_EQ(arr[ArrayIndex::Index10], 0);
	}

	TEST(NamedArray, batch_write_initializer_list) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		arr[{ArrayIndex::Index1, ArrayIndex::Index2, ArrayIndex::Index8, ArrayIndex::Index9, ArrayIndex::Index10}] = {1, 2, 8, 9, 10};
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 1);
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		EXPECT_EQ(arr[ArrayIndex::Index10], 10);
		
		arr[{ArrayIndex::Index1, ArrayIndex::Index10}] = {0, 0};
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 0);
		EXPECT_EQ(arr[ArrayIndex::Index2], 2);
		EXPECT_EQ(arr[ArrayIndex::Index8], 8);
		EXPECT_EQ(arr[ArrayIndex::Index9], 9);
		EXPECT_EQ(arr[ArrayIndex::Index10], 0);
	}

	TEST(NamedArray, read) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		arr[{ArrayIndex::Index1, ArrayIndex::Index2, ArrayIndex::Index8, ArrayIndex::Index9, ArrayIndex::Index10}] = {1, 2, 8, 9, 10};
		
		const vkm::utils::NamedArray<ArrayIndex, int>& const_arr = arr;
		
		auto val1 = const_arr[ArrayIndex::Index1];
		auto val3 = const_arr[ArrayIndex::Index3];
		
		EXPECT_EQ(val1, 1);
		EXPECT_EQ(val3, 0);
		
		// Verify const access returns value type (not proxy)
		EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(val1)>, int>));
		
		// Non-const access returns proxy
		auto& ref = arr[ArrayIndex::Index1];
		EXPECT_FALSE((std::is_same_v<std::remove_cvref_t<decltype(ref)>, int>));
	}

	TEST(NamedArray, proxy_array_assign) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		arr[ArrayIndex::Index1] = 1;
		arr[ArrayIndex::Index9] = 9;
		
		arr[{ArrayIndex::Index1, ArrayIndex::Index9}] = arr[{ArrayIndex::Index9, ArrayIndex::Index1}];
		
		EXPECT_EQ(arr[ArrayIndex::Index1], 9);
		EXPECT_EQ(arr[ArrayIndex::Index9], 1);
	}

	TEST(NamedArray, hash) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5, Index6, Index7, Index8, Index9, Index10,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		auto hash1 = std::hash<decltype(arr)>{}(arr);
		
		arr[ArrayIndex::Index1] = 42;
		auto hash2 = std::hash<decltype(arr)>{}(arr);
		
		arr[ArrayIndex::Index1] = 0;
		arr[ArrayIndex::Index9] = 42;
		auto hash3 = std::hash<decltype(arr)>{}(arr);
		
		EXPECT_NE(hash1, hash2);
		EXPECT_NE(hash2, hash3);
		
		auto arr2 = arr;
		EXPECT_EQ(std::hash<decltype(arr)>{}(arr), std::hash<decltype(arr2)>{}(arr2));
	}

	TEST(NamedArray, size_accessor) {
		enum class ArrayIndex {
			Index1, Index2, Index3, Index4, Index5,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		EXPECT_EQ(arr.size(), 5u);
		EXPECT_EQ(vkm::utils::NamedArray<ArrayIndex, int>::size(), 5u);
	}

	TEST(NamedArray, different_types) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		
		// Test with double
		vkm::utils::NamedArray<ArrayIndex, double> double_arr;
		double_arr[ArrayIndex::Index1] = 3.14;
		EXPECT_DOUBLE_EQ(double_arr[ArrayIndex::Index1], 3.14);
		
		// Test with float
		vkm::utils::NamedArray<ArrayIndex, float> float_arr;
		float_arr[ArrayIndex::Index2] = 2.71f;
		EXPECT_FLOAT_EQ(float_arr[ArrayIndex::Index2], 2.71f);
		
		// Test with bool
		vkm::utils::NamedArray<ArrayIndex, bool> bool_arr;
		bool_arr[ArrayIndex::Index3] = true;
		EXPECT_TRUE(bool_arr[ArrayIndex::Index3]);
	}

	template<typename>
	constexpr bool t = false;
	
	template<typename T>
		requires requires{ std::declval<vkm::utils::NamedArray<T, int>>();}
	constexpr bool t<T> = true;
	
	TEST(NamedArray, compiling_wrong_parameter) {
		class ArrayIndex1 {};
		EXPECT_FALSE(t<ArrayIndex1>);
		
		enum class ArrayIndex2 {
			Index1
		};
		EXPECT_FALSE(t<ArrayIndex2>);
		
		enum class ArrayIndex3 {
			ArrayMax
		};
		EXPECT_FALSE(t<ArrayIndex3>);
		
		enum class ArrayIndex4 {
			Index1, Index2, Index3,
			ArrayMax
		};
		EXPECT_TRUE(t<ArrayIndex4>);
	}

	TEST(NamedArray, proxy_operations) {
		enum class ArrayIndex {
			Index1, Index2,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		
		// Test proxy assignment
		auto proxy = arr[ArrayIndex::Index1];
		proxy = 42;
		EXPECT_EQ(arr[ArrayIndex::Index1], 42);
		
		// Test proxy conversion to value
		int val = arr[ArrayIndex::Index1];
		EXPECT_EQ(val, 42);
		
		// Test proxy get method
		EXPECT_EQ(arr[ArrayIndex::Index1].get(), 42);
	}

	TEST(NamedArray, const_proxy_array) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr;
		arr[ArrayIndex::Index1] = 1;
		arr[ArrayIndex::Index2] = 2;
		arr[ArrayIndex::Index3] = 3;
		
		const auto& const_arr = arr;
		auto batch = const_arr[{ArrayIndex::Index1, ArrayIndex::Index2, ArrayIndex::Index3}];
		
		EXPECT_EQ(batch[0], 1);
		EXPECT_EQ(batch[1], 2);
		EXPECT_EQ(batch[2], 3);
		EXPECT_EQ(batch.size(), 3);
	}

	TEST(NamedArray, proxy_array_comparison) {
		enum class ArrayIndex {
			Index1, Index2, Index3,
			ArrayMax
		};
		vkm::utils::NamedArray<ArrayIndex, int> arr1, arr2;
		
		arr1[{ArrayIndex::Index1, ArrayIndex::Index2}] = {1, 2};
		arr2[{ArrayIndex::Index1, ArrayIndex::Index2}] = {1, 2};
		
		auto batch1 = arr1[{ArrayIndex::Index1, ArrayIndex::Index2}];
		auto batch2 = arr2[{ArrayIndex::Index1, ArrayIndex::Index2}];
		EXPECT_EQ(batch1, batch2);
		
		arr2[ArrayIndex::Index1] = 3;
		auto batch3 = arr2[{ArrayIndex::Index1, ArrayIndex::Index2}];
		EXPECT_NE(batch1, batch3);
	}

} // namespace vkm_test::utils

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
