#pragma once
#include <type_traits>
#include <cstring>
#include <string_view>
#include <cstdint>
#include "is_enum_class.h"
#include "min_type.h"

namespace vkm::utils {
	template<typename T, typename U>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray;
}

namespace vkm_impl::utils {
	using namespace vkm::utils;
	
	// ConstProxy for single element access (read-only)
	template<typename U>
	class NamedArrayConstProxy {
		const U& ref;
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
		constexpr NamedArrayConstProxy(const U& r) : ref(r) {}
	public:
		constexpr operator const U&() const noexcept { return ref; }
		constexpr const U& get() const noexcept { return ref; }
	};

	// Proxy for single element access (read-write)
	template<typename U>
	class NamedArrayProxy {
		U& ref;
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
	public:
		constexpr NamedArrayProxy(U& r) : ref(r) {}
		constexpr operator U&() noexcept { return ref; }
		constexpr operator const U&() const noexcept { return ref; }
		constexpr U& get() noexcept { return ref; }
		constexpr const U& get() const noexcept { return ref; }
		constexpr NamedArrayProxy& operator=(const U& val) noexcept(std::is_nothrow_copy_assignable_v<U>) {
			ref = val;
			return *this;
		}
		constexpr NamedArrayProxy& operator=(U&& val) noexcept(std::is_nothrow_move_assignable_v<U>) {
			ref = std::move(val);
			return *this;
		}
	};

	// ConstProxyArray for batch element access (read-only)
	template<size_t l, typename U>
	class NamedArrayConstProxyArray {
		std::array<const U*, l> data;
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
	public:
		constexpr NamedArrayConstProxyArray() = default;
		
		template<size_t index>
		constexpr const U& get() const noexcept {
			return *data[index];
		}
		constexpr const U& operator[](size_t index) const noexcept {
			return *data[index];
		}
		constexpr size_t size() const noexcept {
			return l;
		}
		
		// Structured binding support
		constexpr auto as_tuple() const {
			if constexpr (l == 0) {
				return std::make_tuple();
			} else if constexpr (l == 1) {
				return std::make_tuple((*data[0]));
			} else if constexpr (l == 2) {
				return std::make_tuple((*data[0]), (*data[1]));
			} else if constexpr (l == 3) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]));
			} else if constexpr (l == 4) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]), (*data[3]));
			} else if constexpr (l == 5) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]), (*data[3]), (*data[4]));
			} else if constexpr (l == 6) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]), (*data[3]), (*data[4]), (*data[5]));
			} else if constexpr (l == 7) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]), (*data[3]), (*data[4]), (*data[5]), (*data[6]));
			} else if constexpr (l == 8) {
				return std::make_tuple((*data[0]), (*data[1]), (*data[2]), (*data[3]), (*data[4]), (*data[5]), (*data[6]), (*data[7]));
			} else {
				static_assert(l <= 8, "Structured binding support up to 8 elements");
			}
		}
		
		constexpr bool operator==(const NamedArrayConstProxyArray<l, U>& other) const noexcept(std::declval<U>() != std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != *other.data[i])return false;
			}
			return true;
		}
		
		// Helper for NamedArray to set internal data pointers
		constexpr void _set_data_ptr(size_t idx, const U* ptr) {
			data[idx] = ptr;
		}
	};
	
	// ProxyArray for batch element access (read-write)
	template<size_t l, typename U>
	class NamedArrayProxyArray {
		std::array<U*, l> data;
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
	public:
		constexpr NamedArrayProxyArray() = default;
		
		template<size_t index>
		constexpr U& get() noexcept {
			return *data[index];
		}
		constexpr U& operator[](size_t index) noexcept {
			return *data[index];
		}
		template<size_t index>
		constexpr const U& get() const noexcept {
			return *data[index];
		}
		constexpr const U& operator[](size_t index) const noexcept {
			return *data[index];
		}
		constexpr size_t size() const noexcept {
			return l;
		}
		
		// Assignment from initializer list
		constexpr NamedArrayProxyArray& operator=(std::initializer_list<U> args) noexcept(std::is_nothrow_copy_assignable_v<U>) {
			size_t i = 0;
			for (const auto& arg : args) {
				if (i < l) {
					*data[i++] = arg;
				}
			}
			return *this;
		}
		
		// Assignment from array
		constexpr NamedArrayProxyArray& operator=(const U(&args)[l]) noexcept(std::is_nothrow_copy_assignable_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = args[i];
			}
			return *this;
		}
		
		// Assignment from const proxy array
		constexpr NamedArrayProxyArray& operator=(const NamedArrayConstProxyArray<l,U>& other) noexcept(std::is_nothrow_copy_assignable_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = other[i];
			}
			return *this;
		}
		
		// Assignment from proxy array
		constexpr NamedArrayProxyArray& operator=(const NamedArrayProxyArray<l,U>& other) noexcept(std::is_nothrow_copy_assignable_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = other[i];
			}
			return *this;
		}
		
		// Conversion to const proxy array
		constexpr operator NamedArrayConstProxyArray<l,U>() const noexcept(std::is_nothrow_copy_constructible_v<U>) {
			NamedArrayConstProxyArray<l,U> res{};
			for (size_t i = 0;i < l;i++) {
				res.data[i] = data[i];
			}
			return res;
		}
		
		constexpr bool operator==(const NamedArrayProxyArray& other) const noexcept(std::declval<U>() != std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != *other.data[i])return false;
			}
			return true;
		}
		constexpr bool operator==(const NamedArrayConstProxyArray<l,U>& other) const noexcept(std::declval<U>()!=std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != *other.data[i])return false;
			}
			return true;
		}
		
		// Helper for NamedArray to set internal data pointers
		constexpr void _set_data_ptr(size_t idx, U* ptr) {
			data[idx] = ptr;
		}
	};
}

namespace vkm::utils {
	template<typename T, typename U>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray {
		using IndexType = UnsignedMinType<(size_t)T::ArrayMax>;
		static constexpr size_t data_length = (size_t)T::ArrayMax;
		U data[data_length]{};
		template<typename, typename>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
		friend struct std::hash<NamedArray<T,U>>;
	public:
		constexpr NamedArray() noexcept = default;
		constexpr NamedArray(const NamedArray&) = default;
		constexpr NamedArray(NamedArray&&) = default;
		constexpr NamedArray& operator=(const NamedArray&) = default;
		constexpr NamedArray& operator=(NamedArray&&) = default;
		
		// Single element access
		constexpr auto operator[](const T enum_index) noexcept {
			return vkm_impl::utils::NamedArrayProxy<U>(data[static_cast<IndexType>(enum_index)]);
		}
		constexpr auto operator[](const T enum_index) const noexcept {
			return vkm_impl::utils::NamedArrayConstProxy<U>(data[static_cast<IndexType>(enum_index)]);
		}
		
		// Batch element access - non-const version
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) noexcept {
			vkm_impl::utils::NamedArrayProxyArray<l, U> res;
			for (size_t i = 0;i < l;i++) {
				res._set_data_ptr(i, &data[static_cast<IndexType>(enum_indexes[i])]);
			}
			return res;
		}
		
		// Batch element access - const version
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) const noexcept {
			vkm_impl::utils::NamedArrayConstProxyArray<l, U> res;
			for (size_t i = 0;i < l;i++) {
				res._set_data_ptr(i, &data[static_cast<IndexType>(enum_indexes[i])]);
			}
			return res;
		}
		
		// Size accessor
		constexpr static size_t size() noexcept {
			return data_length;
		}
		
		// Equality comparison
		constexpr bool operator==(const NamedArray<T, U>& other) const noexcept {
			if constexpr (std::is_trivially_copyable_v<U>) {
				return std::memcmp(data, other.data, sizeof(data)) == 0;
			} else {
				for (size_t i = 0; i < data_length; i++) {
					if (data[i] != other.data[i]) return false;
				}
				return true;
			}
		}
	};
}

namespace std {
	template<typename T, typename U>
	struct hash<vkm::utils::NamedArray<T, U>> {
		size_t operator()(const vkm::utils::NamedArray<T, U>& arr) const noexcept {
			if constexpr (std::is_trivially_copyable_v<U>) {
				std::string_view view{ reinterpret_cast<const char*>(arr.data), sizeof(arr.data) };
				return std::hash<std::string_view>{}(view);
			} else {
				size_t result = 0;
				for (size_t i = 0; i < vkm::utils::NamedArray<T, U>::size(); i++) {
					result ^= std::hash<U>{}(arr.data[i]) + 0x9e3779b9 + (result << 6) + (result >> 2);
				}
				return result;
			}
		}
	};
}