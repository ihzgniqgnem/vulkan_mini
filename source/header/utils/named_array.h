#pragma once
#include <type_traits>
#include <cstring>
#include <string_view>
#include "is_enum_class.h"
#include "min_type.h"

namespace vkm::utils {
	template<typename T, typename U>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray;
}

namespace vkm_impl::utils::BitFlags {
	using namespace vkm::utils;
	template<size_t l,typename U>
	class ConstProxyArray {
		U data[l];
		template<typename T>
			requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
		friend class NamedArray;
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
		constexpr bool operator==(const ConstProxyArray<l, U>& other) const noexcept(std::declval<U>() != std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (data[i] != other.data[i])return false;
			}
			return true;
		}
	};
	template<size_t l,typename U>
	class ProxyArray {
		U* data[l];
		template<typename T>
			requires is_enum_class_v<T>&& requires{T::FlagMax;} && ((size_t)T::FlagMax > 0)
		friend class NamedArray;
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
			auto temp_other = (ConstProxyArray<l,U>)other;
			for (size_t i = 0;i < l;i++) {
				*data[i] = temp_other[i];
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
		constexpr bool operator==(const ProxyArray& other) const noexcept(std::declval<U>() != std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != *other.data[i])return false;
			}
			return true;
		}
		constexpr bool operator==(const ConstProxyArray<l,U>& other) const noexcept(std::declval<U>()!=std::declval<U>()) {
			for (size_t i = 0;i < l;i++) {
				if (*data[i] != other.data[i])return false;
			}
			return true;
		}
	};
}
namespace vkm::utils {
	template<typename T,typename U>
		requires is_enum_class_v<T>&& requires{T::ArrayMax;} && ((size_t)T::ArrayMax > 0)
	class NamedArray {
		using P = UnsignedMinType<T::ArrayMax>;
		static constexpr size_t data_length = (size_t)T::ArrayMax;
		U data[data_length];
		friend struct std::hash<NamedArray<T,U>>;
	public:
		constexpr NamedArray<T,U>() noexcept = default;
		constexpr NamedArray<T, U>(const NamedArray<T, U>&) = default;
		constexpr NamedArray<T, U>(NamedArray<T, U>&&) = default;
		constexpr NamedArray<T, U>& operator=(const NamedArray<T, U>&) = default;
		constexpr NamedArray<T, U>& operator=(NamedArray<T, U>&&) = default;
		constexpr U operator[](const T enum_index) noexcept {
			return data[static_cast<P>(enum_index)];
		}
		constexpr const U operator[](const T enum_index) const noexcept {
			return data[static_cast<P>(enum_index)];
		}
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) noexcept {
			vkm_impl::utils::NamedArray::ProxyArray<l> res;
			U index;
			for (size_t i = 0;i < l;i++) {
				index = static_cast<U>(enum_indexes[i]);
				res.data[i] = vkm_impl::utils::NamedArray::Proxy(data[index >> 3], index & 7);
			}
			return res;
		}
		template<size_t l>
		constexpr auto operator[](const T(&enum_indexes)[l]) const noexcept {
			vkm_impl::utils::NamedArray::BoolArray<l> res;
			U index;
			for (size_t i = 0;i < l;i++) {
				index = static_cast<U>(enum_indexes[i]);
				res.data[i] = (data[index >> 3] >> (index & 7)) & 1u;
			}
			return res;
		}
		constexpr bool operator==(NamedArray<T> other) const noexcept {
			return !std::memcmp(data, other.data, item_num);
		}
	};
}
namespace std {
	template<typename T,typename U>
	struct hash<vkm::utils::NamedArray<T,U>> {
	private:
		static constexpr auto string_view_hash = std::hash<std::string_view>{};
	public:
		size_t operator()(const vkm::utils::NamedArray<T>& flags) const noexcept {
			std::string_view view{ reinterpret_cast<const char*>(flags.data), vkm::utils::NamedArray<T>::item_num };
			return string_view_hash(view);
		}
	};
}