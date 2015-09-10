#pragma once

#include "../Common/Common.hpp"

namespace nl
{
	namespace Impl
	{
		template<typename TKey, typename TValue> 
		struct BMPair
		{
			using Key = TKey;
			using Value = TValue;
		};

		template<typename... TPairs> 
		struct CTBimap
		{
			using PairList = ssvu::MPL::List<TPairs...>;
			template<typename T> using Add = CTBimap<T, TPairs...>;
			static constexpr bool unique{true}; // TODO
		};

		template<typename, typename> struct BMByKeyHelper;
		template<typename TKey, typename TValue, typename... Ts> struct BMByKeyHelper<TKey, ssvu::MPL::List<BMPair<TKey, TValue>, Ts...>>
		{
			using Type = TValue;
		};
		template<typename TKey, typename T, typename... Ts> struct BMByKeyHelper<TKey, ssvu::MPL::List<T, Ts...>>
		{
			using Type = typename BMByKeyHelper<TKey, ssvu::MPL::List<Ts...>>::Type;
		};

		template<typename, typename> struct BMByValueHelper;
		template<typename TValue, typename TKey, typename... Ts> struct BMByValueHelper<TValue, ssvu::MPL::List<BMPair<TKey, TValue>, Ts...>>
		{
			using Type = TKey;
		};
		template<typename TValue, typename T, typename... Ts> struct BMByValueHelper<TValue, ssvu::MPL::List<T, Ts...>>
		{
			using Type = typename BMByValueHelper<TValue, ssvu::MPL::List<Ts...>>::Type;
		};

		template<typename TBimap, typename TKey> using LookupByKey = typename BMByKeyHelper<TKey, typename TBimap::PairList>::Type;
		template<typename TBimap, typename TValue> using LookupByValue = typename BMByValueHelper<TValue, typename TBimap::PairList>::Type;
		template<typename TBimap, typename TKey, typename TValue> using Add = typename TBimap::template Add<BMPair<TKey, TValue>>;

		using TestBimap = CTBimap
		<
			BMPair<int, const int>,
			BMPair<short, const short>,
			BMPair<long, const long>
		>;

		SSVU_ASSERT_STATIC_NM(ssvu::isSame<LookupByKey<TestBimap, int>, const int>());
		SSVU_ASSERT_STATIC_NM(ssvu::isSame<LookupByValue<TestBimap, const int>, int>());
	}
}