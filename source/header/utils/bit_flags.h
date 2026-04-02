#pragma once
#include <type_traits>
#include <cstring>
#include <string_view>
#include <algorithm>
#include <bit>
#include "is_enum_class.h"
#include "min_type.h"

namespace vkm::utils {
	template<typename T>
		requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	class BitFlags;
}

namespace vkm_impl::utils::BitFlags {
	using namespace vkm::utils;
	
	// Proxy class for single bit access - allows read/write like a reference
	class Proxy {
		uint8_t* raw_data;
		uint8_t index;
		
		template<typename T>
			requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
		friend class vkm::utils::BitFlags;
		
		constexpr Proxy(uint8_t* data, uint8_t idx) : raw_data(data), index(idx) {}
		
	public:
		constexpr Proxy() noexcept : raw_data(nullptr), index(0) {}
		constexpr Proxy(const Proxy&) noexcept = default;
		constexpr Proxy(Proxy&&) noexcept = default;
		constexpr Proxy& operator=(const Proxy& other) noexcept = default;
		constexpr Proxy& operator=(Proxy&& other) noexcept = default;
		
		constexpr operator bool() const noexcept {
			return raw_data && ((*raw_data >> index) & 1u);
		}
		
		constexpr bool operator=(bool value) noexcept {
			if (raw_data) {
				*raw_data = (*raw_data & ~(1u << index)) | ((value ? 1u : 0u) << index);
			}
			return value;
		}
		
		constexpr void flip() noexcept {
			if (raw_data) {
				*raw_data ^= (1u << index);
			}
		}
	};
	
	// Const array view for batch reading
	template<size_t L>
	class BoolArray {
		bool data[L];
		
		template<typename T> requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
		friend class BitFlags;
		
	public:
		constexpr BoolArray() noexcept : data{} {}
		
		template<typename... Args>
			requires (std::is_convertible_v<Args, bool> && ...) && (sizeof...(Args) == L)
		constexpr BoolArray(Args... args) noexcept {
			size_t i = 0;
			((data[i++] = static_cast<bool>(args)), ...);
		}
		
		template<size_t Index>
		constexpr bool get() const noexcept {
			static_assert(Index < L, "Index out of bounds");
			return data[Index];
		}
		
		constexpr bool operator[](size_t index) const noexcept {
			return data[index];
		}
		
		constexpr size_t size() const noexcept {
			return L;
		}
		
		constexpr bool operator==(const BoolArray<L>& other) const noexcept {
			for (size_t i = 0; i < L; ++i) {
				if (data[i] != other.data[i]) return false;
			}
			return true;
		}
	};
	
	// Proxy array for batch read/write operations
	template<size_t L>
	class ProxyArray {
		Proxy data[L];
		
		template<typename T> requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
		friend class BitFlags;
		
	public:
		constexpr ProxyArray() noexcept : data{} {}
		
		template<size_t Index>
		constexpr Proxy& get() noexcept {
			static_assert(Index < L, "Index out of bounds");
			return data[Index];
		}
		
		template<size_t Index>
		constexpr bool get() const noexcept {
			static_assert(Index < L, "Index out of bounds");
			return static_cast<bool>(data[Index]);
		}
		
		constexpr Proxy& operator[](size_t index) noexcept {
			return data[index];
		}
		
		constexpr bool operator[](size_t index) const noexcept {
			return static_cast<bool>(data[index]);
		}
		
		constexpr size_t size() const noexcept {
			return L;
		}
		
		// Assignment from C-array of bools
		constexpr ProxyArray& operator=(const bool (&args)[L]) noexcept {
			for (size_t i = 0; i < L; ++i) {
				data[i] = args[i];
			}
			return *this;
		}
		
		// Assignment from BoolArray
		constexpr ProxyArray& operator=(const BoolArray<L>& other) noexcept {
			for (size_t i = 0; i < L; ++i) {
				data[i] = other[i];
			}
			return *this;
		}
		
		// Assignment from another ProxyArray
		constexpr ProxyArray& operator=(const ProxyArray<L>& other) noexcept {
			for (size_t i = 0; i < L; ++i) {
				data[i] = other.data[i];
			}
			return *this;
		}
		
		// Conversion to BoolArray
		constexpr operator BoolArray<L>() const noexcept {
			BoolArray<L> res{};
			for (size_t i = 0; i < L; ++i) {
				res.data[i] = static_cast<bool>(data[i]);
			}
			return res;
		}
		
		constexpr bool operator==(const ProxyArray<L>& other) const noexcept {
			for (size_t i = 0; i < L; ++i) {
				if (static_cast<bool>(data[i]) != static_cast<bool>(other.data[i])) {
					return false;
				}
			}
			return true;
		}
		
		constexpr bool operator==(const BoolArray<L>& other) const noexcept {
			for (size_t i = 0; i < L; ++i) {
				if (static_cast<bool>(data[i]) != other.data[i]) {
					return false;
				}
			}
			return true;
		}
	};
}

namespace vkm::utils {
	template<typename T>
		requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	class BitFlags {
		using P = UnsignedMinType<(size_t)T::FlagMax>;
		static constexpr size_t DATA_LENGTH = (size_t)T::FlagMax;
		static constexpr size_t ITEM_NUM = (DATA_LENGTH + 7) >> 3;
		
		uint8_t data[ITEM_NUM];
		
		template<size_t L>
		friend class vkm_impl::utils::BitFlags::ProxyArray;
		template<size_t L>
		friend class vkm_impl::utils::BitFlags::BoolArray;
		template<typename>
		friend struct std::hash;
		
	public:
		// Constructors and assignment operators
		constexpr BitFlags() noexcept : data{} {}
		constexpr BitFlags(const BitFlags&) = default;
		constexpr BitFlags(BitFlags&&) = default;
		constexpr BitFlags& operator=(const BitFlags&) = default;
		constexpr BitFlags& operator=(BitFlags&&) = default;
		
		// Single bit access
		constexpr bool operator[](T enum_index) const noexcept {
			const P index = static_cast<P>(enum_index);
			return (data[index >> 3] >> (index & 7)) & 1u;
		}
		
		constexpr vkm_impl::utils::BitFlags::Proxy operator[](T enum_index) noexcept {
			const P index = static_cast<P>(enum_index);
			return vkm_impl::utils::BitFlags::Proxy(&data[index >> 3], index & 7);
		}
		
		// Batch access - non-const version returns ProxyArray
		template<size_t L>
		constexpr auto operator[](const T (&enum_indexes)[L]) noexcept {
			vkm_impl::utils::BitFlags::ProxyArray<L> res;
			for (size_t i = 0; i < L; ++i) {
				const P index = static_cast<P>(enum_indexes[i]);
				res.data[i] = vkm_impl::utils::BitFlags::Proxy(&data[index >> 3], index & 7);
			}
			return res;
		}
		
		// Batch access - const version returns BoolArray
		template<size_t L>
		constexpr auto operator[](const T (&enum_indexes)[L]) const noexcept {
			vkm_impl::utils::BitFlags::BoolArray<L> res{};
			for (size_t i = 0; i < L; ++i) {
				const P index = static_cast<P>(enum_indexes[i]);
				res.data[i] = (data[index >> 3] >> (index & 7)) & 1u;
			}
			return res;
		}
		
		// Comparison operators
		constexpr bool operator==(const BitFlags& other) const noexcept {
			return std::memcmp(data, other.data, ITEM_NUM) == 0;
		}
		
		constexpr bool operator!=(const BitFlags& other) const noexcept {
			return !(*this == other);
		}
		
		// Utility methods
		constexpr void set(T enum_index, bool value = true) noexcept {
			const P index = static_cast<P>(enum_index);
			if (value) {
				data[index >> 3] |= (1u << (index & 7));
			} else {
				data[index >> 3] &= ~(1u << (index & 7));
			}
		}
		
		constexpr void reset(T enum_index) noexcept {
			set(enum_index, false);
		}
		
		constexpr void flip(T enum_index) noexcept {
			const P index = static_cast<P>(enum_index);
			data[index >> 3] ^= (1u << (index & 7));
		}
		
		constexpr void set() noexcept {
			std::memset(data, 0xFF, ITEM_NUM);
			// Clear unused bits in last byte
			constexpr size_t used_bits = DATA_LENGTH & 7;
			if constexpr (used_bits != 0) {
				data[ITEM_NUM - 1] &= ((1u << used_bits) - 1);
			}
		}
		
		constexpr void reset() noexcept {
			std::memset(data, 0, ITEM_NUM);
		}
		
		constexpr void flip() noexcept {
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				data[i] = ~data[i];
			}
			// Clear unused bits in last byte
			constexpr size_t used_bits = DATA_LENGTH & 7;
			if constexpr (used_bits != 0) {
				data[ITEM_NUM - 1] &= ((1u << used_bits) - 1);
			}
		}
		
		// Query methods
		constexpr bool test(T enum_index) const noexcept {
			return (*this)[enum_index];
		}
		
		constexpr bool all() const noexcept {
			constexpr size_t used_bits = DATA_LENGTH & 7;
			for (size_t i = 0; i < ITEM_NUM - 1; ++i) {
				if (data[i] != 0xFF) return false;
			}
			if constexpr (used_bits != 0) {
				return data[ITEM_NUM - 1] == ((1u << used_bits) - 1);
			} else {
				return data[ITEM_NUM - 1] == 0xFF;
			}
		}
		
		constexpr bool any() const noexcept {
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				if (data[i] != 0) return true;
			}
			return false;
		}
		
		constexpr bool none() const noexcept {
			return !any();
		}
		
		constexpr size_t count() const noexcept {
			size_t cnt = 0;
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				cnt += std::popcount(static_cast<unsigned int>(data[i]));
			}
			return cnt;
		}
		
		constexpr size_t size() const noexcept {
			return DATA_LENGTH;
		}
		
		constexpr static size_t max_size() noexcept {
			return DATA_LENGTH;
		}
		
		constexpr bool empty() const noexcept {
			return DATA_LENGTH == 0;
		}
		
		// Bitwise operations
		constexpr BitFlags& operator&=(const BitFlags& other) noexcept {
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				data[i] &= other.data[i];
			}
			return *this;
		}
		
		constexpr BitFlags& operator|=(const BitFlags& other) noexcept {
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				data[i] |= other.data[i];
			}
			return *this;
		}
		
		constexpr BitFlags& operator^=(const BitFlags& other) noexcept {
			for (size_t i = 0; i < ITEM_NUM; ++i) {
				data[i] ^= other.data[i];
			}
			return *this;
		}
		
		constexpr BitFlags operator~() const noexcept {
			BitFlags result = *this;
			result.flip();
			return result;
		}
		
		// Data access
		constexpr const uint8_t* data_ptr() const noexcept {
			return data;
		}
		
		constexpr uint8_t* data_ptr() noexcept {
			return data;
		}
	};
	
	// Free functions for bitwise operations
	template<typename T>
		requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	constexpr BitFlags<T> operator&(BitFlags<T> lhs, const BitFlags<T>& rhs) noexcept {
		lhs &= rhs;
		return lhs;
	}
	
	template<typename T>
		requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	constexpr BitFlags<T> operator|(BitFlags<T> lhs, const BitFlags<T>& rhs) noexcept {
		lhs |= rhs;
		return lhs;
	}
	
	template<typename T>
		requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	constexpr BitFlags<T> operator^(BitFlags<T> lhs, const BitFlags<T>& rhs) noexcept {
		lhs ^= rhs;
		return lhs;
	}
}

namespace std {
	// Tuple support for structured binding with ProxyArray
	template<size_t L>
	struct tuple_size<vkm_impl::utils::BitFlags::ProxyArray<L>> 
		: public integral_constant<size_t, L> {};
	
	template<size_t I, size_t L>
	struct tuple_element<I, vkm_impl::utils::BitFlags::ProxyArray<L>> {
		using type = vkm_impl::utils::BitFlags::Proxy;
	};
	
	template<size_t I, size_t L>
	struct tuple_element<I, const vkm_impl::utils::BitFlags::ProxyArray<L>> {
		using type = bool;
	};
	
	// Tuple support for BoolArray
	template<size_t L>
	struct tuple_size<vkm_impl::utils::BitFlags::BoolArray<L>> 
		: public integral_constant<size_t, L> {};
	
	template<size_t I, size_t L>
	struct tuple_element<I, vkm_impl::utils::BitFlags::BoolArray<L>> {
		using type = bool;
	};
	
	// Hash specialization
	template<typename T>
		requires vkm::utils::is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
	struct hash<vkm::utils::BitFlags<T>> {
		size_t operator()(const vkm::utils::BitFlags<T>& flags) const noexcept {
			std::string_view view{ 
				reinterpret_cast<const char*>(flags.data), 
				vkm::utils::BitFlags<T>::ITEM_NUM 
			};
			return std::hash<std::string_view>{}(view);
		}
	};
}