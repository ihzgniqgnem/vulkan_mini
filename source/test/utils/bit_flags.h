#pragma once
#include <type_traits>
#include <bit>

#include <gtest/gtest.h>
#include "../../header/utils/bit_flags.h"

namespace vkm_test::utils {
	
	TEST(BitFlags, small_flag) {
		enum class FlagIndex {
			Flag1, Flag2,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		EXPECT_EQ(sizeof(x), 1);
	}
	
	TEST(BitFlags, big_flag) {
		enum class FlagIndex {
			FlagMax = 10000
		};
		vkm::utils::BitFlags<FlagIndex> x;
		EXPECT_EQ(sizeof(x), 1250);
	}
	
	TEST(BitFlags, init) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		unsigned char x[2];
		x[0] = x[1] = 0xff;
		auto* y = new(x) vkm::utils::BitFlags<FlagIndex>;
		EXPECT_EQ(x[0], 0);
		EXPECT_EQ(x[1], 0);
		y->~BitFlags();
	}
	
	TEST(BitFlags, basic_attr) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		using BitFlagsType = vkm::utils::BitFlags<FlagIndex>;

		EXPECT_TRUE(std::is_default_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_copy_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_copy_assignable_v<BitFlagsType>);
		EXPECT_TRUE(std::is_move_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_move_assignable_v<BitFlagsType>);
		EXPECT_TRUE(std::is_destructible_v<BitFlagsType>);

		EXPECT_TRUE(std::is_trivially_copyable_v<BitFlagsType>);

		EXPECT_TRUE(std::is_nothrow_default_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_nothrow_copy_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_nothrow_copy_assignable_v<BitFlagsType>);
		EXPECT_TRUE(std::is_nothrow_move_constructible_v<BitFlagsType>);
		EXPECT_TRUE(std::is_nothrow_move_assignable_v<BitFlagsType>);
		EXPECT_TRUE(std::is_nothrow_destructible_v<BitFlagsType>);

		EXPECT_TRUE(std::is_standard_layout_v<BitFlagsType>);
	}
	
	TEST(BitFlags, op_eq) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x, y;

		EXPECT_TRUE(x == y);
		x[FlagIndex::Flag1] = true;
		EXPECT_TRUE(x != y);
		y[FlagIndex::Flag1] = true;
		EXPECT_TRUE(x == y);
	}
	
	TEST(BitFlags, write) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Single bit write via proxy
		x[FlagIndex::Flag1] = true;
		EXPECT_TRUE(x[FlagIndex::Flag1]);
		
		x[FlagIndex::Flag2] = true;
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		
		x[FlagIndex::Flag8] = true;
		EXPECT_TRUE(x[FlagIndex::Flag8]);
		
		x[FlagIndex::Flag9] = true;
		EXPECT_TRUE(x[FlagIndex::Flag9]);
		
		x[FlagIndex::Flag10] = true;
		EXPECT_TRUE(x[FlagIndex::Flag10]);
		
		// Structured binding with proxy array
		auto [a, b] = x[{FlagIndex::Flag1, FlagIndex::Flag10}];
		a = b = false;
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		EXPECT_TRUE(x[FlagIndex::Flag8]);
		EXPECT_TRUE(x[FlagIndex::Flag9]);
		EXPECT_FALSE(x[FlagIndex::Flag10]);
	}
	
	TEST(BitFlags, batch_write) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Batch write using initializer list
		x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag8, FlagIndex::Flag9, FlagIndex::Flag10}] = { true, true, true, true, true };
		EXPECT_TRUE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		EXPECT_TRUE(x[FlagIndex::Flag8]);
		EXPECT_TRUE(x[FlagIndex::Flag9]);
		EXPECT_TRUE(x[FlagIndex::Flag10]);
		
		// Partial batch write
		x[{FlagIndex::Flag1, FlagIndex::Flag10}] = { false, false };
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		EXPECT_TRUE(x[FlagIndex::Flag8]);
		EXPECT_TRUE(x[FlagIndex::Flag9]);
		EXPECT_FALSE(x[FlagIndex::Flag10]);
	}
	
	TEST(BitFlags, read) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag8, FlagIndex::Flag9, FlagIndex::Flag10}] = { true, true, true, true, true };
		
		const vkm::utils::BitFlags<FlagIndex>& y = const_cast<const vkm::utils::BitFlags<FlagIndex>&>(x);
		
		// Const version returns BoolArray (values)
		auto [a, b] = y[{FlagIndex::Flag1, FlagIndex::Flag3}];
		EXPECT_TRUE(a);
		EXPECT_FALSE(b);
		EXPECT_TRUE((std::is_same_v<std::remove_cvref_t<decltype(a)>, bool>));
		
		// Non-const version returns ProxyArray (proxies)
		auto [c, d] = x[{FlagIndex::Flag1, FlagIndex::Flag3}];
		EXPECT_FALSE((std::is_same_v<std::remove_cvref_t<decltype(c)>, bool>));
	}
	
	template<typename>
	constexpr bool t = false;
	
	template<typename T>
		requires requires{ std::declval<vkm::utils::BitFlags<T>>();}
	constexpr bool t<T> = true;
	
	TEST(BitFlags, compiling_wrong_parameter) {
		class FlagIndex1 {};
		EXPECT_FALSE(t<FlagIndex1>);
		
		enum class FlagIndex2 {
			Flag1
		};
		EXPECT_FALSE(t<FlagIndex2>);
		
		enum class FlagIndex3 {
			FlagMax
		};
		EXPECT_FALSE(t<FlagIndex3>);
		
		enum class FlagIndex4 {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		EXPECT_TRUE(t<FlagIndex4>);
	}
	
	TEST(BitFlags, hash) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		auto a = std::hash<decltype(x)>{}(x);
		
		x[FlagIndex::Flag1] = true;
		auto b = std::hash<decltype(x)>{}(x);
		
		x[FlagIndex::Flag1] = false;
		x[FlagIndex::Flag9] = true;
		auto c = std::hash<decltype(x)>{}(x);
		
		EXPECT_NE(a, b);
		EXPECT_NE(b, c);
		
		auto y = x;
		EXPECT_EQ(std::hash<decltype(x)>{}(x), std::hash<decltype(y)>{}(y));
	}
	
	TEST(BitFlags, proxy_array_assign) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[FlagIndex::Flag1] = true;
		
		// Proxy to Proxy assignment
		x[{FlagIndex::Flag1, FlagIndex::Flag9}] = x[{FlagIndex::Flag9, FlagIndex::Flag1}];
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag9]);
	}
	
	// New tests for enhanced functionality
	
	TEST(BitFlags, set_reset_flip_single) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Test set
		x.set(FlagIndex::Flag1);
		EXPECT_TRUE(x[FlagIndex::Flag1]);
		
		x.set(FlagIndex::Flag2, true);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		
		x.set(FlagIndex::Flag3, false);
		EXPECT_FALSE(x[FlagIndex::Flag3]);
		
		// Test reset
		x.reset(FlagIndex::Flag1);
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		
		// Test flip
		x.flip(FlagIndex::Flag2);
		EXPECT_FALSE(x[FlagIndex::Flag2]);
		
		x.flip(FlagIndex::Flag2);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
	}
	
	TEST(BitFlags, set_reset_flip_all) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Set all
		x.set();
		EXPECT_TRUE(x.all());
		EXPECT_TRUE(x.any());
		EXPECT_FALSE(x.none());
		EXPECT_EQ(x.count(), 8);
		
		// Reset all
		x.reset();
		EXPECT_FALSE(x.all());
		EXPECT_FALSE(x.any());
		EXPECT_TRUE(x.none());
		EXPECT_EQ(x.count(), 0);
		
		// Flip all
		x.flip();
		EXPECT_TRUE(x.all());
		EXPECT_EQ(x.count(), 8);
		
		// Flip again
		x.flip();
		EXPECT_TRUE(x.none());
		EXPECT_EQ(x.count(), 0);
	}
	
	TEST(BitFlags, test_method) {
		enum class FlagIndex {
			Flag1, Flag2,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[FlagIndex::Flag1] = true;
		
		EXPECT_TRUE(x.test(FlagIndex::Flag1));
		EXPECT_FALSE(x.test(FlagIndex::Flag2));
	}
	
	TEST(BitFlags, any_all_none) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Initially all false
		EXPECT_FALSE(x.any());
		EXPECT_FALSE(x.all());
		EXPECT_TRUE(x.none());
		
		// Set one
		x[FlagIndex::Flag1] = true;
		EXPECT_TRUE(x.any());
		EXPECT_FALSE(x.all());
		EXPECT_FALSE(x.none());
		
		// Set all
		x.set();
		EXPECT_TRUE(x.any());
		EXPECT_TRUE(x.all());
		EXPECT_FALSE(x.none());
	}
	
	TEST(BitFlags, count) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		EXPECT_EQ(x.count(), 0);
		
		x[FlagIndex::Flag1] = true;
		x[FlagIndex::Flag3] = true;
		x[FlagIndex::Flag5] = true;
		
		EXPECT_EQ(x.count(), 3);
		
		x.set();
		EXPECT_EQ(x.count(), 5);
	}
	
	TEST(BitFlags, size_methods) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, Flag9, Flag10,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		EXPECT_EQ(x.size(), 10);
		EXPECT_EQ(vkm::utils::BitFlags<FlagIndex>::max_size(), 10);
		EXPECT_FALSE(x.empty());
	}
	
	TEST(BitFlags, bitwise_operations) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> a, b;
		
		a[FlagIndex::Flag1] = true;
		a[FlagIndex::Flag2] = true;
		
		b[FlagIndex::Flag2] = true;
		b[FlagIndex::Flag3] = true;
		
		// AND
		auto c = a & b;
		EXPECT_TRUE(c[FlagIndex::Flag2]);
		EXPECT_FALSE(c[FlagIndex::Flag1]);
		EXPECT_FALSE(c[FlagIndex::Flag3]);
		
		// OR
		auto d = a | b;
		EXPECT_TRUE(d[FlagIndex::Flag1]);
		EXPECT_TRUE(d[FlagIndex::Flag2]);
		EXPECT_TRUE(d[FlagIndex::Flag3]);
		EXPECT_FALSE(d[FlagIndex::Flag4]);
		
		// XOR
		auto e = a ^ b;
		EXPECT_TRUE(e[FlagIndex::Flag1]);
		EXPECT_FALSE(e[FlagIndex::Flag2]);
		EXPECT_TRUE(e[FlagIndex::Flag3]);
		EXPECT_FALSE(e[FlagIndex::Flag4]);
		
		// NOT
		auto f = ~a;
		EXPECT_FALSE(f[FlagIndex::Flag1]);
		EXPECT_FALSE(f[FlagIndex::Flag2]);
		EXPECT_TRUE(f[FlagIndex::Flag3]);
		EXPECT_TRUE(f[FlagIndex::Flag4]);
		
		// Compound assignments
		vkm::utils::BitFlags<FlagIndex> g;
		g |= a;
		EXPECT_TRUE(g[FlagIndex::Flag1]);
		EXPECT_TRUE(g[FlagIndex::Flag2]);
		
		g &= b;
		EXPECT_FALSE(g[FlagIndex::Flag1]);
		EXPECT_TRUE(g[FlagIndex::Flag2]);
		
		g ^= a;
		EXPECT_TRUE(g[FlagIndex::Flag1]);
		EXPECT_FALSE(g[FlagIndex::Flag2]);
	}
	
	TEST(BitFlags, data_ptr_access) {
		enum class FlagIndex {
			Flag1, Flag2,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[FlagIndex::Flag1] = true;
		
		const auto* ptr = x.data_ptr();
		EXPECT_NE(ptr, nullptr);
		EXPECT_EQ(ptr[0], 1);
		
		auto* mutable_ptr = x.data_ptr();
		mutable_ptr[0] = 0;
		EXPECT_FALSE(x[FlagIndex::Flag1]);
	}
	
	TEST(BitFlags, proxy_flip) {
		enum class FlagIndex {
			Flag1,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[FlagIndex::Flag1] = true;
		
		// Get proxy and flip
		auto proxy = x[FlagIndex::Flag1];
		proxy.flip();
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		
		proxy.flip();
		EXPECT_TRUE(x[FlagIndex::Flag1]);
	}
	
	TEST(BitFlags, bool_array_comparison) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[{FlagIndex::Flag1, FlagIndex::Flag2}] = {true, false};
		
		auto arr = x[{FlagIndex::Flag1, FlagIndex::Flag2}];
		auto const_arr = static_cast<const decltype(x)&>(x)[{FlagIndex::Flag1, FlagIndex::Flag2}];
		
		EXPECT_EQ(arr[0], true);
		EXPECT_EQ(arr[1], false);
		EXPECT_EQ(const_arr[0], true);
		EXPECT_EQ(const_arr[1], false);
	}
	
	TEST(BitFlags, cross_boundary_flags) {
		// Test flags that cross byte boundaries
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5, Flag6, Flag7, Flag8, // byte 0
			Flag9, Flag10, Flag11, Flag12, Flag13, Flag14, Flag15, Flag16, // byte 1
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Set flags at byte boundaries
		x[FlagIndex::Flag8] = true;
		x[FlagIndex::Flag9] = true;
		
		EXPECT_TRUE(x[FlagIndex::Flag8]);
		EXPECT_TRUE(x[FlagIndex::Flag9]);
		
		// Set first and last
		x[FlagIndex::Flag1] = true;
		x[FlagIndex::Flag16] = true;
		
		EXPECT_TRUE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag16]);
		
		EXPECT_EQ(x.count(), 4);
	}
	
	TEST(BitFlags, partial_byte_handling) {
		// Test with non-byte-aligned flag count
		enum class FlagIndex {
			Flag1, Flag2, Flag3, Flag4, Flag5,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		x.set();
		// Only 5 flags should be set, not all 8 bits in the byte
		EXPECT_EQ(x.count(), 5);
		EXPECT_TRUE(x.all());
		
		// Verify unused bits are cleared
		const auto* ptr = x.data_ptr();
		EXPECT_EQ(ptr[0], 0b00011111); // Only lower 5 bits set
	}
	
	TEST(BitFlags, structured_binding_modification) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag3}] = {true, false, true};
		
		// Modify through structured binding
		auto [a, b, c] = x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag3}];
		a = false;
		b = true;
		c = false;
		
		EXPECT_FALSE(x[FlagIndex::Flag1]);
		EXPECT_TRUE(x[FlagIndex::Flag2]);
		EXPECT_FALSE(x[FlagIndex::Flag3]);
	}
	
	TEST(BitFlags, proxy_array_bool_array_conversion) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag3}] = {true, false, true};
		
		auto proxy_arr = x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag3}];
		
		// Convert to BoolArray
		vkm_impl::utils::BitFlags::BoolArray<3> bool_arr = proxy_arr;
		
		EXPECT_EQ(bool_arr[0], true);
		EXPECT_EQ(bool_arr[1], false);
		EXPECT_EQ(bool_arr[2], true);
	}
	
	TEST(BitFlags, proxy_array_assignment_from_bool_array) {
		enum class FlagIndex {
			Flag1, Flag2, Flag3,
			FlagMax
		};
		vkm::utils::BitFlags<FlagIndex> x;
		
		// Create BoolArray
		vkm_impl::utils::BitFlags::BoolArray<3> bool_arr{true, false, true};
		
		// Assign to ProxyArray
		x[{FlagIndex::Flag1, FlagIndex::Flag2, FlagIndex::Flag3}] = bool_arr;
		
		EXPECT_TRUE(x[FlagIndex::Flag1]);
		EXPECT_FALSE(x[FlagIndex::Flag2]);
		EXPECT_TRUE(x[FlagIndex::Flag3]);
	}
	
} // namespace vkm_test::utils