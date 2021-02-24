#include <iostream>

template <char ...cs>
struct String{};

template <char... cs>
void Print(String<cs...>)
{
	using Expand = int[];
	(void)(Expand){((std::cout << cs), 0)...};
}

template <typename T, typename D>
struct State{
	using TRX = T;
	using DRX = D;
};

struct TRX_TAG{};
struct DRX_TAG{};

template <typename LHS, typename RHS>
struct MOV
{
	template <typename STATE>
	static auto Do()
	{
		if constexpr(std::is_same_v<LHS, RHS>)
			return STATE{};
		else if constexpr(std::is_same_v<TRX_TAG, RHS>)
			return State<typename STATE::TRX, typename STATE::TRX>{};
		else if constexpr(std::is_same_v<DRX_TAG, RHS>)
			return State<typename STATE::DRX, typename STATE::DRX>{};

		// Now we know that RHS is a string
		else if constexpr(std::is_same_v<DRX_TAG, LHS>)
			return State<typename STATE::TRX, RHS>{};
		else if constexpr(std::is_same_v<TRX_TAG, LHS>)
			return State<RHS, typename STATE::DRX>{};
	}
};

template <typename T, typename U>
struct reverse_string_impl;

template <char ...cs, char ...ds>
struct reverse_string_impl<String<cs...>, String<ds...>>;

template <char ...ds>
struct reverse_string_impl<String<>, String<ds...>>
{
	using type = String<ds...>;
};

template <char c, char ...cs, char ...ds>
struct reverse_string_impl<String<c, cs...>, String<ds...>> : reverse_string_impl<String<cs...>, String<c, ds...>>
{
};

template <typename T>
struct reverse_string;

template <char ...cs>
struct reverse_string<String<cs...>>
{
	using type = typename reverse_string_impl<String<cs...>, String<>>::type;
};


template <typename T, typename U>
struct xor_string;

template <char... cs, char... ds>
struct xor_string<String<cs...>, String<ds...>>
{
	using type = String<cs ^ ds...>;
};

constexpr bool gt(int a, int b)
{
	return a > b;
}

template <int pad, typename T>
struct add_padding;

template <char ...cs>
struct add_padding<0, String<cs...>>
{
	using type = String<cs...>;
};

template <int pad, char ...cs>
struct add_padding<pad, String<cs...>> : add_padding<pad - 1, String<cs..., 0>>
{
};

template <typename Num>
constexpr Num max(Num a, Num b)
{
	return a > b ? a : b;
}

template <typename T, typename U>
struct xor_string_pad;

template <char ...cs, char ...ds>
struct xor_string_pad<String<cs...>, String<ds...>>
{
	constexpr static int result_size()
	{
		return max(sizeof...(cs), sizeof...(ds));
	}
	using type = typename xor_string<
			typename add_padding<result_size() - sizeof...(cs), String<cs...>>::type,
			typename add_padding<result_size() - sizeof...(ds), String<ds...>>::type
				>::type;
};

template <typename LHS, typename RHS>
struct XOR
{
	template <typename STATE>
	static auto Do()
	{
		if constexpr(std::is_same_v<TRX_TAG, RHS>)
		{
			if constexpr(std::is_same_v<TRX_TAG, LHS>)
				return State<
						typename xor_string<typename STATE::TRX, typename STATE::TRX>::type, typename STATE::DRX>{};
			else if constexpr(std::is_same_v<DRX_TAG, LHS>)
				return State<
					typename STATE::TRX,
					typename xor_string_pad<typename STATE::TRX,typename STATE::DRX>::type
					>{};
		}
		else if constexpr(std::is_same_v<DRX_TAG, RHS>)
		{
			if constexpr(std::is_same_v<DRX_TAG, LHS>)
				return State<
						typename STATE::TRX,
						typename xor_string<typename STATE::DRX, typename STATE::DRX>::type>{};
			else if constexpr(std::is_same_v<TRX_TAG, LHS>)
				return State<
					typename xor_string_pad<typename STATE::TRX, typename STATE::DRX>::type,
					typename STATE::DRX
					>{};
		}
		else
		{
			// String literal case
			if constexpr(std::is_same_v<TRX_TAG, LHS>)
				return State<
					typename xor_string_pad<typename STATE::TRX, RHS>::type,
					typename STATE::DRX
					>{};
			else
				return State<
					typename STATE::TRX,
					typename xor_string_pad<typename STATE::DRX, RHS>::type
					>{};
		}
	}

};

template <typename ARG>
struct REVERSE
{
	template <typename STATE>
	static auto Do()
	{
		if constexpr(std::is_same_v<TRX_TAG, ARG>)
			return State<typename reverse_string<typename STATE::TRX>::type, typename STATE::DRX>{};
		else if constexpr(std::is_same_v<DRX_TAG, ARG>)
			return State<typename STATE::TRX, typename reverse_string<typename STATE::DRX>::type>{};
	}
};

template <typename STATE, typename... Instructions>
struct Emulator;

template <typename TRX, typename DRX>
struct Emulator<State<TRX, DRX>>
{
	Emulator()
	{
		Print(TRX{});
	}
};

template <typename STATE, typename Instruction, typename... Instructions>
struct Emulator<STATE, Instruction, Instructions...> : Emulator<decltype(Instruction::template Do<STATE>()), Instructions...>
{
};

using TRX_INIT = String<'G','E','D','\x03','h','G','\x15','&','K','a',' ','=',';','\x0c','\x1a','3','1','o','*','5','M'>;

using DRX_INIT = String<>;

int main()
{
	Emulator<
		State<TRX_INIT, DRX_INIT>,
		#include "preprocessed.h"
			> emulator;

	std::cout << std::endl;
	return 0;
}
