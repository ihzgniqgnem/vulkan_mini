#pragma once
#include <type_traits>
#include <cstring>
#include <string_view>
#include "is_enum_class.h"
#include "min_type.h"

namespace vkm::utils {
	template<typename T, typename U = std::underlying_type_t<T>>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray;
}

namespace vkm_impl::utils::NamedArray {
	using namespace vkm::utils;
	
	template<size_t l, typename U>
	class ConstProxyArray {
		U data[l];
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
		template<size_t, typename>
		friend class ProxyArray;
	public:
		template<size_t index>
		constexpr const U& get() const noexcept {
			return data[index];
		}
		constexpr const U& operator[](size_t index) const noexcept {
			return data[index];
		}
		constexpr size_t size() const noexcept {
			return l;
		}
		constexpr bool operator==(const ConstProxyArray<l, U>& other) const noexcept {
			for (size_t i = 0;i < l;i++) {
				if (data[i] != other.data[i])return false;
			}
			return true;
		}
	};
	
	template<size_t l, typename U>
	class ProxyArray {
		U* data[l];
		template<typename T, typename V>
			requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
		friend class NamedArray;
		template<size_t, typename>
		friend class ConstProxyArray;
	public:
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
		constexpr auto& operator=(const U(&args)[l]) noexcept(std::is_nothrow_copy_constructible_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = args[i];
			}
			return *this;
		}
		constexpr auto& operator=(const ConstProxyArray<l,U>& other) noexcept(std::is_nothrow_copy_constructible_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = other[i];
			}
			return *this;
		}
		constexpr auto& operator=(const ProxyArray<l,U>& other) noexcept(std::is_nothrow_copy_constructible_v<U>) {
			for (size_t i = 0;i < l;i++) {
				*data[i] = *other.data[i];
			}
			return *this;
		}
		constexpr operator ConstProxyArray<l,U>() const noexcept(std::is_nothrow_copy_constructible_v<U>) {
			ConstProxyArray<l,U> res{};
			for (size_t i = 0;i < l;i++) {
				res.data[i] = *data[i];
			}
			return res;
		}
		constexpr bool operator==(const ProxyArray& other) const noexcept {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != *other.data[i])return false;
			}
			return true;
		}
		constexpr bool operator==(const ConstProxyArray<l,U>& other) const noexcept {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != other.data[i])return false;
			}
			return true;
		}
	};
}

namespace vkm::utils {
	template<typename T, typename U>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray {
		using P = UnsignedMinType<(size_t)T::ArrayMax>;
		static constexpr size_t data_length = (size_t)T::ArrayMax;
		U data[data_length]{};
		template<size_t l, typename V>
		friend class vkm_impl::utils::NamedArray::ProxyArray;
		template<size_t l, typename V>
		friend class vkm_impl::utils::NamedArray::ConstProxyArray;
		template<typename>
		friend struct std::hash;
	public:
		constexpr NamedArray<T,U>() noexcept = default;
		constexpr NamedArray<T, U>(const NamedArray<T, U>&) = default;
		constexpr NamedArray<T, U>(NamedArray<T, U>&&) = default;
		constexpr NamedArray<T, U>& operator=(const NamedArray<T, U>&) = default;
		constexpr NamedArray<T, U>& operator=(NamedArray<T, U>&&) = default;
		
		constexpr U& operator[](const T enum_index) noexcept {
			return data[static_cast<P>(enum_index)];
		}
		constexpr const U& operator[](const T enum_index) const noexcept {
			return data[static_cast<P>(enum_index)];
		}
		
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) noexcept {
			vkm_impl::utils::NamedArray::ProxyArray<l, U> res;
			for (size_t i = 0;i < l;i++) {
				res.data[i] = &data[static_cast<P>(enum_indexes[i])];
			}
			return res;
		}
		
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) const noexcept {
			vkm_impl::utils::NamedArray::ConstProxyArray<l, U> res{};
			for (size_t i = 0;i < l;i++) {
				res.data[i] = data[static_cast<P>(enum_indexes[i])];
			}
			return res;
		}
		
		constexpr bool operator==(const NamedArray<T, U>& other) const noexcept {
			for (size_t i = 0; i < data_length; i++) {
				if (data[i] != other.data[i]) return false;
			}
			return true;
		}
		
		constexpr size_t size() const noexcept {
			return data_length;
		}
	};
}

namespace std {
	template<typename T, typename U>
	struct hash<vkm::utils::NamedArray<T, U>> {
	private:
		static constexpr auto string_view_hash = std::hash<std::string_view>{};
	public:
		size_t operator()(const vkm::utils::NamedArray<T, U>& arr) const noexcept {
			std::string_view view{ reinterpret_cast<const char*>(arr.data), sizeof(U) * vkm::utils::NamedArray<T, U>::data_length };
			return string_view_hash(view);
		}
	};
}