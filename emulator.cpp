#include <type_traits>


template <typename T, typename D>
struct State
{
	using TRX = T;
	using DRX = D;
};

struct TRX_TAG{};
struct DRX_TAG{};

template <char ...cs>
struct String{};


// == Instructions ==

namespace instructions
{
    // == REVERSE Implementation Details ==

    namespace reverse_impl
    {
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
    }


    // == XOR Implementation Details ==

    namespace xor_impl
    {
        template <typename T, typename U>
        struct xor_string;

        template <char... cs, char... ds>
        struct xor_string<String<cs...>, String<ds...>>
        {
            using type = String<cs ^ ds...>;
        };

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
        struct xor_string_pad
        {
            using type = void;
        };

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
    }

    using namespace reverse_impl;
    using namespace xor_impl;

    template <typename LHS, typename RHS>
    struct MOV
    {
        template <typename STATE>
        using Do =
            // If LHS == RHS, return STATE unmodified
            std::conditional_t<std::is_same_v<LHS, RHS>,
                STATE,

            // If we're moving from TRX, we'll be moving into DRX, so return State<TRX,TRX>
            std::conditional_t<std::is_same_v<TRX_TAG, RHS>,
                State<typename STATE::TRX, typename STATE::TRX>,

            // Same as above but for DRX
            std::conditional_t<std::is_same_v<DRX_TAG, RHS>,
                State<typename STATE::DRX, typename STATE::DRX>,

            // If we're here, RHS must be a string.

            // If LHS is DRX...
            std::conditional_t<std::is_same_v<DRX_TAG, LHS>,
                // Move the string into DRX
                State<typename STATE::TRX, RHS>,

            // If LHS is TRX...
            std::conditional_t<std::is_same_v<TRX_TAG, LHS>,
                // Move the string into TRX
                State<RHS, typename STATE::DRX>,

            // Should not happen...
            void>>>>>;
    };

    template <typename REGISTER>
    struct REVERSE
    {
        template <typename STATE>
        using Do =
            // If register to reverse is TRX...
            std::conditional_t<std::is_same_v<TRX_TAG, REGISTER>,
                // Reverse TRX, leave DRX the same.
                State<typename reverse_string<typename STATE::TRX>::type,
                      typename STATE::DRX>,

            // Else, if register to reverse is DRX...
            std::conditional_t<std::is_same_v<DRX_TAG, REGISTER>,
                // Reverse DRX, leave TRX the same.
                State<typename STATE::TRX,
                      typename reverse_string<typename STATE::DRX>::type>,

            // Should not happen...
            void>>;
    };

    template <typename LHS, typename RHS>
    struct XOR
    {
        template <typename STATE>
        using Do =
            // If RHS is our TRX register...
            std::conditional_t<std::is_same_v<TRX_TAG, RHS>,

                // If LHS is TRX...
                std::conditional_t<std::is_same_v<TRX_TAG, LHS>,
                    // XOR TRX with itself, leave DRX the same.
                    State<typename xor_string<typename STATE::TRX, typename STATE::TRX>::type,
                          typename STATE::DRX>,

                // If LHS is DRX...
                std::conditional_t<std::is_same_v<DRX_TAG, LHS>,
                    // XOR TRX value into DRX register, leave TRX register the same.
                    State<typename STATE::TRX,
                          typename xor_string_pad<typename STATE::TRX, typename STATE::DRX>::type>,

                // Should not happen...
                void>>,

            // If RHS is our DRX register...
            std::conditional_t<std::is_same_v<DRX_TAG, RHS>,

                // If LHS is DRX...
                std::conditional_t<std::is_same_v<DRX_TAG, LHS>,
                    // XOR DRX with itself, leave TRX the same.
                    State<typename STATE::TRX,
                          typename xor_string<typename STATE::DRX, typename STATE::DRX>::type>,

                // If LHS is TRX...
                std::conditional_t<std::is_same_v<TRX_TAG, LHS>,
                    // XOR DRX value into TRX register, leave DRX register the same.
                    State<typename xor_string_pad<typename STATE::TRX, typename STATE::DRX>::type,
                          typename STATE::DRX>,

                // Should not happen...
                void>>,

            // If we're here, RHS must be a string literal.

            // If LHS is TRX...
            std::conditional_t<std::is_same_v<TRX_TAG, LHS>,
                // XOR the string literal into TRX
                State<typename xor_string_pad<typename STATE::TRX, RHS>::type,
                      typename STATE::DRX>,

            // If LHS is DRX...
            std::conditional_t<std::is_same_v<DRX_TAG, LHS>,
                // XOR the string literal into DRX
                State<typename STATE::TRX,
                      typename xor_string_pad<typename STATE::DRX, RHS>::type>,

            // Should not happen...
            void>>>>;
    };
}


// == 'Print' ==

// Base case for Print recursion
void Print(String<>)
{
}

template <char c, char...cs>
void Print(String<c, cs...>)
{
    // Static char[1] guarantees we'll keep a single char in our executable
    // somewhere to be moved into ch when/if Print is executed if we've linked
    // against Print somewhere, for each set of template parameters.
	static char ch[1];
	ch[0] = c;

    // Recurse to store remaining characters...
	Print(String<cs...>{});
}


// == Emulator ==

template <typename STATE, typename... Instructions>
struct Emulator;

template <typename TRX, typename DRX>
struct Emulator<State<TRX, DRX>>
{
    Emulator()
    {
        // Print the string stored in TRX
        Print(TRX{});
    }
};


// Emulation loop
template <typename STATE, typename Instruction, typename... Instructions>
struct Emulator<STATE, Instruction, Instructions...> : Emulator<typename Instruction::template Do<STATE>, Instructions...>
{
};


// == Main ==

void _()
{
    using namespace instructions;

    // Initial register state
    using TRX_INIT = String<'G','E','D','\x03','h','G','\x15','&','K','a',' ','=',';','\x0c','\x1a','3','1','o','*','5','M'>;
    using DRX_INIT = String<>;

    // Run emulator
    Emulator<State<TRX_INIT, DRX_INIT>,
		#include "preprocessed.h"
	> emulator{};
}
