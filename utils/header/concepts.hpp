#pragma once
#ifndef MCSL_CONCEPTS
#define MCSL_CONCEPTS

#include "MCSL.hpp"

#include <concepts>


//!NOTE: FIX CONTAINER CONCEPTS


namespace mcsl {
   #pragma region checks
   template<typename lhs, typename rhs> concept same_t = std::same_as<lhs,rhs>;
   template<typename child_t, typename parent_t> concept is_t = std::derived_from<child_t,parent_t> || same_t<child_t,parent_t>;
   template<typename orig_t, typename target_t> concept castable_to = std::convertible_to<orig_t,target_t>;

   template<typename T> concept int_t = std::integral<T>;
   template<typename T> concept uint_t = std::unsigned_integral<T>;
   template<typename T> concept sint_t = std::signed_integral<T>;
   template<typename T> concept float_t = std::floating_point<T>;
   template<typename T> concept num_t = mcsl::int_t<T> || mcsl::float_t<T>;
   template<typename T> concept ptr_t = std::is_pointer<T>::value;


   template<typename ...Ts> concept any_num_t   = (  num_t<Ts> || ...);
   template<typename ...Ts> concept any_float_t = (float_t<Ts> || ...);
   template<typename ...Ts> concept any_int_t   = (  int_t<Ts> || ...);
   template<typename ...Ts> concept any_uint_t  = ( uint_t<Ts> || ...);
   template<typename ...Ts> concept any_sint_t  = ( sint_t<Ts> || ...);
   template<typename ...Ts> concept any_ptr_t   = (  ptr_t<Ts> || ...);

   template<typename ...Ts> concept all_num_t   = (  num_t<Ts> && ...);
   template<typename ...Ts> concept all_float_t = (float_t<Ts> && ...);
   template<typename ...Ts> concept all_int_t   = (  int_t<Ts> && ...);
   template<typename ...Ts> concept all_uint_t  = ( uint_t<Ts> && ...);
   template<typename ...Ts> concept all_sint_t  = ( sint_t<Ts> && ...);
   template<typename ...Ts> concept all_ptr_t   = (  ptr_t<Ts> && ...);
   #pragma endregion checks

   #pragma region basic_select
   template<bool b, typename T1, typename T2> using select = std::conditional_t<b,T1,T2>;
   //!TODO: `filter`
   #pragma endregion basic_select

   #pragma region mods
   template<int_t T> using to_sint_t = std::make_signed_t<T>;
   template<int_t T> using to_uint_t = std::make_unsigned_t<T>;
   //!TODO: to_int_t
   template<num_t T> using to_float_t = select<float_t<T>, //!TODO: make this more robust
      T, //T is already a float_t
      select<(sizeof(T) > sizeof(float32)),
         select<(sizeof(T) > sizeof(float64)),
            fext,
            float64
         >,
         float32
      >
   >;
   //!TODO: to_float_precise_t (smallest floating point type precise enough to store any value of the parameter type)
   //!TODO: to_signed (noop if float, else to_sint_t)

   template<typename T> using remove_ptr = std::remove_pointer_t<T>;
   template<typename T> using remove_ref = std::remove_reference_t<T>;

   template<typename T> using remove_const = std::remove_const_t<T>;
   template<typename T> using remove_volatile = std::remove_volatile_t<T>;
   template<typename T> using remove_cv = std::remove_cv_t<T>;
   template<typename T> using remove_cvref = std::remove_cvref_t<T>;
   #pragma endregion mods

   #pragma region selectors
   namespace { //implement smaller_t
      template<typename T1, typename T2> struct __SMALLER;
      template<typename T1> struct __SMALLER<T1, void> { using type = T1; };
      template<typename T2> struct __SMALLER<void, T2> { using type = T2; };
      template<typename T1, typename T2> struct __SMALLER { using type = select<(sizeof(T1) < sizeof(T2)), T1, T2>; };
   };
   namespace { //implement larger_t
      template<typename T1, typename T2> struct __LARGER;
      template<typename T1> struct __LARGER<T1, void> { using type = T1; };
      template<typename T2> struct __LARGER<void, T2> { using type = T2; };
      template<typename T1, typename T2> struct __LARGER { using type = select<(sizeof(T1) > sizeof(T2)), T1, T2>; };
   };
   template<typename T1, typename T2> using smaller_t = __SMALLER<T1,T2>::type;
   template<typename T1, typename T2> using larger_t = __LARGER<T1,T2>::type;

   namespace { //implement largest_[a-z]+_t
      namespace { //num
         template<typename T = void, typename ...Ts> struct __LARGEST_NUM;
         template<> struct __LARGEST_NUM<void> { using type = void; };
         template<typename T, typename ...Ts> struct __LARGEST_NUM {
            using type = select<num_t<T>,
               select<sizeof...(Ts) != 0, //T is an num
                  larger_t<T, typename __LARGEST_NUM<Ts...>::type>,
                  T>,
               select<sizeof...(Ts) != 0, //T is not an num
                  typename __LARGEST_NUM<Ts...>::type,
                  void>>;
         };
      };
      namespace { //float
         template<typename T = void, typename ...Ts> struct __LARGEST_FLOAT;
         template<> struct __LARGEST_FLOAT<void> { using type = void; };
         template<typename T, typename ...Ts> struct __LARGEST_FLOAT {
            using type = select<float_t<T>,
               select<sizeof...(Ts) != 0, //T is a float
                  larger_t<T, typename __LARGEST_FLOAT<Ts...>::type>,
                  T>,
               select<sizeof...(Ts) != 0, //T is not a float
                  typename __LARGEST_FLOAT<Ts...>::type,
                  void>>;
         };
      };
      namespace { //int
         template<typename T = void, typename ...Ts> struct __LARGEST_INT;
         template<> struct __LARGEST_INT<void> { using type = void; };
         template<typename T, typename ...Ts> struct __LARGEST_INT {
            using type = select<int_t<T>,
               select<sizeof...(Ts) != 0, //T is an int
                  larger_t<T, typename __LARGEST_INT<Ts...>::type>,
                  T>,
               select<sizeof...(Ts) != 0, //T is not an int
                  typename __LARGEST_INT<Ts...>::type,
                  void>>;
         };
      };
      namespace { //uint
         template<typename T = void, typename ...Ts> struct __LARGEST_UINT;
         template<> struct __LARGEST_UINT<void> { using type = void; };
         template<typename T, typename ...Ts> struct __LARGEST_UINT {
            using type = select<uint_t<T>,
               select<sizeof...(Ts) != 0, //T is an uint
                  larger_t<T, typename __LARGEST_UINT<Ts...>::type>,
                  T>,
               select<sizeof...(Ts) != 0, //T is not an uint
                  typename __LARGEST_UINT<Ts...>::type,
                  void>>;
         };
      };
      namespace { //sint
         template<typename T = void, typename ...Ts> struct __LARGEST_SINT;
         template<> struct __LARGEST_SINT<void> { using type = void; };
         template<typename T, typename ...Ts> struct __LARGEST_SINT {
            using type = select<sint_t<T>,
               select<sizeof...(Ts) != 0, //T is an sint
                  larger_t<T, typename __LARGEST_SINT<Ts...>::type>,
                  T>,
               select<sizeof...(Ts) != 0, //T is not an sint
                  typename __LARGEST_SINT<Ts...>::type,
                  void>>;
         };
      };
   };
   
   template<typename ...Ts> using largest_num_t   = __LARGEST_NUM<Ts...>::type;
   template<typename ...Ts> using largest_float_t = __LARGEST_FLOAT<Ts...>::type;
   template<typename ...Ts> using largest_int_t   = __LARGEST_INT<Ts...>::type;
   template<typename ...Ts> using largest_uint_t  = __LARGEST_UINT<Ts...>::type;
   template<typename ...Ts> using largest_sint_t  = __LARGEST_SINT<Ts...>::type;

   template<num_t ...Ts> using most_precise_t = select<any_float_t<Ts...>,
      largest_float_t<Ts...>, //has a floating type -> largest floating type
      select<any_sint_t<Ts...>,
         to_sint_t<largest_int_t<Ts...>>, //has a signed type (and no floating type) -> signed version of largest int type
         largest_uint_t<Ts...>>>; //has no signed or floating types -> largest uint type
   #pragma endregion selectors
   
   #pragma region containers
   template<typename arr_t, typename T> concept contig_container_t = requires(arr_t a) {
      { a.size()  } -> int_t;
      { a.data()  } -> same_t<T*>;
      { a.begin() } -> same_t<T*>;
      { a.end()   } -> same_t<T*>;
      { a[0]      } -> same_t<T&>;
   };
   template<typename T> concept container_t = requires(T a) {
      { a.begin() } -> same_t<decltype(a.end())>;
   };

   template<typename arr_t, typename T> concept contig_t = contig_container_t<arr_t, T> && arr_t::is_contig;

   template<typename string_t> concept str_t = contig_t<string_t, char>;

   template<typename set_t, typename T> concept assoc_t = requires (set_t set, T obj) {
      { set.contains(obj) } -> same_t<bool>;
   } && set_t::is_assoc;
   #pragma endregion containers

   #pragma region utils
   template<typename T, typename func> concept hash_t = requires (T obj, func f) {
      { f(obj) } -> int_t;
   };
   template<typename T, typename func> concept cmp_t = requires (T lhs, T rhs, func f) {
      { f(lhs,rhs) } -> same_t<bool>;
   };

   template<typename func_t, typename ...Args> concept callable_t = requires (func_t f, Args... args) {
      f(args...);
   };

   template<typename T, typename ...Args> concept valid_ctor = requires (Args... args) {
      T{args...};
   };
   
   #pragma endregion utils
};

#endif //MCSL_CONCEPTS