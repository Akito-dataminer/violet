/*****************************
 * Copyright (c) 2023 Akito-Dataminer
 * This software is distributed under the MIT license.
 *****************************/

#pragma once

#include "violet/util/util.hpp"
#include "violet/util/comparable.hpp"

#include <string>
#include <stdexcept>
#include <cstring>
#include <utility>
#include <type_traits>
#include <algorithm>
#include <ostream>

namespace violet {

using char_cptr = char const *;
using list_type = char_cptr [];
using index_type = std::size_t;
using size_type = std::size_t;

namespace STRING {

consteval index_type Length( char const * const string ) {
  index_type index = 0;
  while ( string[index] != '\0' ) { ++index; }
  return index;
}

template<class T>
concept HasFunctionCall = requires() {
  T();
};

template<HasFunctionCall StringProxy>
consteval index_type Length() {
  index_type index = 0;
  while ( StringProxy()()[index] != '\0' ) { ++index; }
  return index;
}

// N文字だけ比較して、同一文字列だったらtrue、
// 同一文字列でなければfalseを返す。
// ただし、str1とstr2が同じであっても、
// 末尾に'\0'を含んでいたらfalseを返してしまう(仕様と書いてルビはバグ)。
template<typename T, util::if_nullp_c<std::is_pointer_v<T>>* = nullptr>
constexpr bool IsSameN( T const str1, T const str2, size_type const N ) {
  index_type index = 0;

  while ( index < N ) {
    if ( str1[index] != str2[index] || str1[index] == '\0' || str2[index] == '\0' ) { return false; }

    ++index;
  }

  return true;
}

// N is guaranteed greater than 0. It has no meanings str_chars_[0] when compiling.
// Because this array can't be changed it size and value later.
template<typename CharT, size_type N, util::if_nullp_c<( N > 0 )>* = nullptr>
struct ExcludeNULLLiteralImpl : public ::util::COMPARABLE::CompDef<ExcludeNULLLiteralImpl<CharT, N>> {
  using value_type = CharT;
  using const_pointer = CharT const *;
  using reference = CharT &;
  using const_reference = CharT const &;
  using iterator = value_type *;
  using const_iterator = value_type const *;
  using difference_type = std::ptrdiff_t;

  explicit consteval ExcludeNULLLiteralImpl() : str_{ '\0' }, len_( 0 ) {}

  explicit consteval ExcludeNULLLiteralImpl( CharT const ( & literal )[N + 1] ) : len_( N ) { std::copy_n( literal, N, str_ ); }

  template<size_type... INDICES>
  explicit consteval ExcludeNULLLiteralImpl( CharT const * literal_p, std::index_sequence<INDICES...> )
  : str_{ ( INDICES <= N ? literal_p[INDICES] : value_type() ) ... }
    , len_( !( N < Length( literal_p ) ) ? Length( literal_p ) : throw std::length_error( "ExcludeNULLLiteralImpl: literal of argument is too long" ) ) {}

  template<size_type... INDICES, typename... Args>
  explicit consteval ExcludeNULLLiteralImpl( std::index_sequence<INDICES...>, Args&& ... args )
  : str_{ ( INDICES <= sizeof...( Args ) ? args : value_type() ) ... } , len_( !( N < sizeof...( Args ) ) ? sizeof...( Args ) : throw std::length_error( "ExcludeNULLLiteralImpl: literal of argument is too long" ) ) {}

  value_type str_[N];
  decltype( N ) len_;

  constexpr const_pointer get() const noexcept { return str_; }
  constexpr size_type size() const noexcept { return N; }
  constexpr size_type length() const noexcept { return len_; }

  constexpr reference operator[] ( size_type index ) noexcept { return str_[index]; }
  constexpr const_reference operator[] ( size_type index ) const noexcept { return str_[index]; }

  template<typename T>
  constexpr ExcludeNULLLiteralImpl<CharT, N> & operator+= ( T const & literal2 ) {
    append( literal2 );
    return *this;
  }

  constexpr void append( CharT const one_char ) {
    if ( ( len_ + 1 ) < N ) {
      str_[len_] = one_char;
      ++len_;
    } else {
      throw std::length_error( "append: literal of argument is too long" );
    }
  }

  constexpr void append( CharT const * str ) {
    size_type len2 = std::strlen(str);

    if ( ( len2 + len_ ) < N ) {
      for ( size_type i = 0; i < len2; ++i ) { str_[len_ + i] = str[i]; }
      len_ += len2;
    } else {
      throw std::length_error( "append: literal of argument is too long" );
    }
  }

  template<size_type N2>
  constexpr void append( ExcludeNULLLiteralImpl<CharT, N2> const & str ) {
    size_type len2 = str.len_;

    if ( ( len2 + this->len_ ) < N ) {
      for ( size_type i = 0; i < len2; ++i ) { this->str_[len_ + i] = str.str_[i]; }
      this->len_ += str.len_;
    } else {
      throw std::length_error( "append: literal of argument is too long" );
    }
  }

protected:
  constexpr iterator makeIterator( size_type const index ) { return ( ( index < ( N + 1 ) ) ? ( str_ + index ) : ( throw std::out_of_range("makeIterator: out of range") ) ); }
  constexpr const_iterator makeConstIterator( size_type const index ) const { return ( ( index < ( N + 1 ) ) ? ( str_ + index ) : ( throw std::out_of_range("makeConstIterator: out of range") ) ); }
};

// 末尾の'\0'を含めないようにするための補助推論(補助推論はC++17から)
template<typename CharT, size_type N>
ExcludeNULLLiteralImpl( CharT const ( & literal )[N], std::make_index_sequence<N>() ) -> ExcludeNULLLiteralImpl<CharT, N - 1>;

////////////////////
// operator<
////////////////////
template<typename CharT, size_type N1, size_type N2>
constexpr bool operator< ( ExcludeNULLLiteralImpl<CharT, N1> const & literal1, ExcludeNULLLiteralImpl<CharT, N2> const & literal2 ) noexcept {
  auto len1 = literal1.len_;
  auto len2 = literal2.len_;

  for ( size_type index = 0; index < std::min( len1, len2 ); ++index ) {
    if ( literal1[index] < literal2[index] ) { return true; }
  }

  return ( len1 < len2 );
}

template<typename CharT, size_type N1, size_type N2>
constexpr bool operator< ( ExcludeNULLLiteralImpl<CharT, N1> const & literal1, CharT const ( & literal2 )[N2] ) noexcept {
  auto len1 = literal1.len_;

  for ( size_t index = 0; index < std::min( len1, N2 ); ++index ) {
    if ( literal1[index] < literal2[index] ) { return true; }
  }

  return ( len1 < ( N2 - 1 ) );
}

template<typename CharT, size_type N1, size_type N2>
constexpr bool operator< ( CharT const ( & literal1 )[N1], ExcludeNULLLiteralImpl<CharT, N2> const & literal2 ) noexcept {
  auto len2 = literal2.len_;

  for ( size_t index = 0; index < std::min( N1, len2 ) ; ++index ) {
    if ( literal1[index] <  literal2[index] ) { return true; }
  }

  return ( ( N1 - 1 ) < len2 );
}

////////////////////
// Literal
////////////////////
template<typename CharT, size_type N, util::if_nullp_c<( N > 0 )>* = nullptr>
struct Literal : public ExcludeNULLLiteralImpl<CharT, N> {
  using impl_type = ExcludeNULLLiteralImpl<CharT, N>;

  explicit consteval Literal() : ExcludeNULLLiteralImpl<CharT, N>() {}
  explicit consteval Literal( CharT const ( & string_literal )[N + 1] ) : ExcludeNULLLiteralImpl<CharT, N>( string_literal, std::make_index_sequence<N>() ) {}
  template<typename... Args>
  explicit consteval Literal( Args&& ... args ) : ExcludeNULLLiteralImpl<CharT, N>( std::make_index_sequence<sizeof...(Args)>(), args... ) {}

  Literal<CharT, N> ( Literal<CharT, N>  const & ) = default;
  Literal<CharT, N> & operator=( Literal<CharT, N> const & ) = default;
  Literal<CharT, N> ( Literal<CharT, N> && rhs ) = default;
  Literal<CharT, N> & operator=( Literal<CharT, N> && ) = default;

  constexpr typename impl_type::iterator begin() noexcept { return impl_type::makeIterator( 0 ); }
  constexpr typename impl_type::iterator end() noexcept { return impl_type::makeIterator( impl_type::len_ ); }
  constexpr typename impl_type::const_iterator cbegin() const noexcept { return impl_type::makeConstIterator( 0 ); }
  constexpr typename impl_type::const_iterator cend() const noexcept { return impl_type::makeConstIterator( impl_type::len_ ); }
};

// ポインタ対応のための特殊化
template<typename CharT, size_type N, util::if_nullp_c<( N > 0 )>* NPTR>
struct Literal<CharT const *, N, NPTR> : public ExcludeNULLLiteralImpl<CharT, N> {
  using impl_type = ExcludeNULLLiteralImpl<CharT, N>;
  explicit consteval Literal( CharT const * string_literal_p ) : ExcludeNULLLiteralImpl<CharT, N>( string_literal_p, std::make_index_sequence<N>() ) {}
};

////////////////////
// operator<<
////////////////////
template<typename CharT, size_type N>
inline STATIC_CONSTEXPR std::basic_ostream<CharT>& operator << ( std::basic_ostream<CharT> & lhs, Literal<CharT, N> const & rhs ) {
  return lhs << rhs.get();
}

////////////////////
// operator+
////////////////////
template<typename CharT, size_type N1, size_type N2, size_type... INDICES>
inline consteval auto ConcateImpl( ExcludeNULLLiteralImpl<CharT, N1> const & literal1, size_type len1, ExcludeNULLLiteralImpl<CharT, N2> const & literal2, size_type len2, std::index_sequence<INDICES...> ) {
  return Literal<CharT, N1 + N2>( ( INDICES < len1 ? literal1[INDICES] : INDICES < ( len1 + len2 ) ? literal2[INDICES - len1] : CharT() ) ... );
}

template<typename CharT, size_type N1, size_type N2>
inline consteval auto Concate( Literal<CharT, N1> const & literal1, Literal<CharT, N2> const & literal2 ) {
  return ConcateImpl( literal1, literal1.length(), literal2, literal2.length(), std::make_index_sequence<N1 + N2>() );
}

template<typename CharT, size_type N1, size_type N2>
inline consteval auto operator+ ( Literal<CharT, N1> const & literal1, Literal<CharT, N2> const & literal2 ) {
  return Concate( literal1, literal2 );
}

// 末尾の'\0'を含めないようにするための補助推論(補助推論はC++17から)
template<typename CharT, size_type N>
Literal( CharT const ( & literal )[N] ) -> Literal<CharT, N - 1>;

template<HasFunctionCall StringProxy>
consteval auto MakeString() {
  return Literal<decltype( StringProxy()() ), Length<StringProxy>()>( StringProxy()() );
}

// クラス型をテンプレート引数に指定できるようになるのはC++20以降
// ただし、クラスをテンプレート引数に指定できるためには、
// テンプレート引数に指定されているクラスがいくつかの条件を充たしている必要がある。
template<Literal LITERAL>
constexpr bool IsSame( char_cptr const str ) {
  constexpr auto length = LITERAL.size();
  constexpr auto ptr = LITERAL.get();

  return ( IsSameN( ptr, str, length ) == true ) ? true : false;
}

// ポインタ型からStringLiteral型に変換する。
template<typename CharT, CharT const * Ptr, size_type length = STRING::Length( Ptr )>
consteval auto ToStringLiteral () {
  return STRING::Literal<CharT const *, length>( Ptr );
}

} // STRING

} // violet
