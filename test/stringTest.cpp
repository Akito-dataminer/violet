/*****************************
 * Copyright (c) 2023 Akito-Dataminer
 * This software is distributed under the MIT license.
 *****************************/

#include "violet/string.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/interface.hpp>
#include <boost/test/tools/old/interface.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <utility>

#define OPTION_LIST { "directory", "help", "version" }

BOOST_AUTO_TEST_SUITE( test_violet )

BOOST_AUTO_TEST_CASE( test_meta_func_ArraySize ) {
  constexpr char const * option_list[] = OPTION_LIST;

  static_assert( violet::util::ArraySize( option_list ) == 3, "violet::util::ArraySize( option_list ) != 3" );

  constexpr char const option_str[] = "directory";
  // constexpr char const * option_str = "directory"; // this is errored.
  static_assert( violet::util::ArraySize( option_str ) == 10, "violet::util::ArraySize( option_str[] ) != 10" );

  static_assert( violet::util::ArraySize( "directory" ) == 10, "violet::util::ArraySize( \"directory\" ) != 10");

  // Below code is errored.
  // static_assert( violet::util::ArraySize<decltype( option_list ), 1>( option_list ) == 0, "violet::util::ArraySize( option_list ) != 0" );
  //
  // constexpr int i = 10;
  // static_assert( violet::util::ArraySize( i ) == 0 );
}

BOOST_AUTO_TEST_CASE( test_meta_func_Length ) {
  constexpr char const option_str[] = "directory";
  static_assert( violet::STRING::Length( option_str ) == 9, "violet::OPTION::STRING::Length( option_list[0] ) != 9" );

  STATIC_CONSTEXPR char const str[] = "directory";

  struct SendString {
    constexpr char const * operator()() { return str; }
  };
  static_assert( violet::STRING::Length<SendString>() == 9 );
}

BOOST_AUTO_TEST_CASE( test_make_literal_func ) {
  using namespace violet::STRING;
  STATIC_CONSTEXPR char const str[] = "directory";

  struct SendString {
    constexpr char const * operator()() { return str; }
  };

  constexpr auto literal = MakeString<SendString>();
  static_assert( literal.size() == 9 );
  static_assert( literal.get()[0] == 'd' );
}

BOOST_AUTO_TEST_CASE( test_meta_func_IsSameN ) {
  constexpr char const str1[] = "directory";
  constexpr char const str2[] = "directory";
  constexpr char const str3[] = "help";

  static_assert( violet::STRING::IsSameN( str1, str2, 9 ) == true, "violet::OPTION::STRING::IsSameN( str1, str2, 9 ) != true" );
  static_assert( violet::STRING::IsSameN( str1, str3, 9 ) == false, "violet::OPTION::STRING::IsSameN( str1, str3, 9 ) != false" );

  bool is_same_1_2 = violet::STRING::IsSameN( str1, str2, 9 );
  bool is_same_1_3 = violet::STRING::IsSameN( str1, str3, 9 );

  BOOST_CHECK( is_same_1_2 == true );
  BOOST_CHECK( is_same_1_3 == false );
}

BOOST_AUTO_TEST_CASE( constexpr_func_IsSame ) {
  using namespace violet::STRING;

  // This is error.
  // Literal<char, 0> literal0( "" );
  STATIC_CONSTEXPR Literal literal1( "directory" );

  static_assert( IsSame<literal1>( "directory" ) == true );

  char const * arg_option = "directory";

  bool const is_same = IsSame<literal1>( arg_option );

  BOOST_CHECK( is_same == true );
}

BOOST_AUTO_TEST_CASE( test_class_template_StringLiteral ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR char const * str1 = "directory";
  STATIC_CONSTEXPR Literal<char const *, Length(str1)> literal1( str1 );

  STATIC_CONSTEXPR char const * options[] = { "directory", "help" };
  constexpr Literal<char const *, Length(options[0])> literal2( options[0] );

  BOOST_CHECK( IsSame<literal1>( str1 ) == true );
  BOOST_CHECK( IsSame<literal2>( options[0] ) == true );
}

BOOST_AUTO_TEST_CASE( test_ToStringLiteral ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR char const option1[] = "directory";
  constexpr auto literal1 = ToStringLiteral<char, option1>();

  static_assert( literal1.size() == 9 );
}

BOOST_AUTO_TEST_CASE( test_chars_ctor ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal<char, 10> literal( 'd', 'i', 'r', 'e', 'c', 't', 'o', 'r', 'y' );

  static_assert( literal.length() == 9 );
  BOOST_CHECK( literal.length() == 9 );
}

BOOST_AUTO_TEST_CASE( test_append ) {
  using namespace violet::STRING;

  STATIC_CONSTINIT Literal<char, 40> literal;
  BOOST_CHECK( literal.begin() == literal.end() );

  STATIC_CONSTEXPR Literal literal_append( "directory" );

  literal.append( literal_append );
  BOOST_CHECK( literal.length() == literal_append.length() );
  BOOST_CHECK( literal == literal_append );

  literal.append( "cucumber" );
  STATIC_CONSTINIT Literal literal_test1( "directorycucumber" );
  BOOST_CHECK( literal.length() == literal_test1.length() );
  BOOST_CHECK( literal == literal_test1 );

  literal.append( '1' );
  std::cerr << "literal: " << literal << std::endl;
  STATIC_CONSTINIT Literal literal_test2( "directorycucumber1" );
  BOOST_CHECK( literal.length() == literal_test2.length() );
  BOOST_CHECK( literal == literal_test2 );
}

BOOST_AUTO_TEST_CASE( test_operator_lt ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal literal1( "directory" );
  STATIC_CONSTEXPR Literal literal2( "directory" );
  STATIC_CONSTEXPR Literal literal3( "eirectory" );

  static_assert( ( literal1 < literal2 ) == false );
  static_assert( ( literal1 < literal3 ) == true );
  static_assert( ( literal3 < literal1 ) == false );

  static_assert( ( literal1 < "directory" ) == false );
  static_assert( ( "directory" < literal1 ) == false );
  static_assert( ( literal1 < "eirectory" ) == true );
  static_assert( ( "eirectory" < literal1 ) == false );

  static_assert( ( literal3 < "directory" ) == false );
  static_assert( ( "directory" < literal3 ) == true );
  static_assert( ( literal3 < "eirectory" ) == false );
  static_assert( ( "eirectory" < literal3 ) == false );

  BOOST_CHECK( ( literal1 < literal2 ) == false );
  BOOST_CHECK( ( literal1 < literal3 ) == true );
  BOOST_CHECK( ( literal3 < literal1 ) == false );

  BOOST_CHECK( ( literal1 < "directory" ) == false );
  BOOST_CHECK( ( "directory" < literal1 ) == false );
  BOOST_CHECK( ( literal1 < "eirectory" ) == true );
  BOOST_CHECK( ( "eirectory" < literal1 ) == false );

  BOOST_CHECK( ( literal3 < "directory" ) == false );
  BOOST_CHECK( ( "directory" < literal3 ) == true );
  BOOST_CHECK( ( literal3 < "eirectory" ) == false );
  BOOST_CHECK( ( "eirectory" < literal3 ) == false );
}

BOOST_AUTO_TEST_CASE( test_operator_plassign ) {
  using namespace violet::STRING;

  STATIC_CONSTINIT Literal<char, 40> literal;
  BOOST_CHECK( literal.begin() == literal.end() );

  STATIC_CONSTEXPR Literal literal_append( "directory" );

  literal += literal_append;
  BOOST_CHECK( literal.length() == literal_append.length() );
  BOOST_CHECK( literal == literal_append );

  literal += "cucumber";
  STATIC_CONSTINIT Literal literal_test1( "directorycucumber" );
  BOOST_CHECK( literal.length() == literal_test1.length() );
  BOOST_CHECK( literal == literal_test1 );

  literal += '1';
  std::cerr << "literal: " << literal << std::endl;
  STATIC_CONSTINIT Literal literal_test2( "directorycucumber1" );
  BOOST_CHECK( literal.length() == literal_test2.length() );
  BOOST_CHECK( literal == literal_test2 );
}

BOOST_AUTO_TEST_CASE( test_Concate ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal literal1( "directory" );
  STATIC_CONSTEXPR Literal literal2( "cucumber" );
  STATIC_CONSTEXPR auto literal3 = Concate( literal1, literal2 );
  STATIC_CONSTEXPR Literal literal_test( "directorycucumber" );

  static_assert( literal3.length() == 17 );
  static_assert( literal3.size() == 17 );
  static_assert( literal3 == literal_test );

  BOOST_CHECK( literal3.length() == 17 );
  BOOST_CHECK( literal3.size() == 17 );
  BOOST_CHECK( literal3 == literal_test );
}

BOOST_AUTO_TEST_CASE( test_operator_pl ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal literal1( "directory" );
  STATIC_CONSTEXPR Literal literal2( "cucumber" );
  STATIC_CONSTEXPR auto literal3 = literal1 + literal2;
  STATIC_CONSTEXPR Literal literal_test( "directorycucumber" );

  static_assert( literal3.length() == 17 );
  static_assert( literal3.size() == 17 );
  static_assert( literal3 == literal_test );

  BOOST_CHECK( literal3.length() == 17 );
  BOOST_CHECK( literal3.size() == 17 );
  BOOST_CHECK( literal3 == literal_test );
}

BOOST_AUTO_TEST_CASE( test_operator_eq ) {
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal literal1( "directory" );
  STATIC_CONSTEXPR Literal literal2( "directory" );
  STATIC_CONSTEXPR Literal literal3( "eirectory" );

  static_assert( ( literal1 == literal2 ) == true );
  static_assert( ( literal1 == literal3 ) == false );
  static_assert( ( literal3 == literal1 ) == false );

  // static_assert( ( literal1 == "directory" ) == false );
  // static_assert( ( "directory" == literal1 ) == false );
  // static_assert( ( literal1 == "eirectory" ) == true );
  // static_assert( ( "eirectory" == literal1 ) == false );

  // static_assert( ( literal3 == "directory" ) == false );
  // static_assert( ( "directory" == literal3 ) == true );
  // static_assert( ( literal3 == "eirectory" ) == false );
  // static_assert( ( "eirectory" == literal3 ) == false );

  BOOST_CHECK( ( literal1 == literal2 ) == true );
  BOOST_CHECK( ( literal1 == literal3 ) == false );
}

BOOST_AUTO_TEST_CASE( test_output_stream ) {
  using namespace violet;
  using namespace violet::STRING;

  STATIC_CONSTEXPR Literal literal( "directory" );

  std::cout << "literal(test_output_stream): " << literal << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( test_violet_iterator )

BOOST_AUTO_TEST_CASE( test_requires ) {
  using namespace violet::STRING;

  static_assert( std::input_or_output_iterator<ExcludeNULLLiteralImpl<char, 1>::iterator> );
  static_assert( std::contiguous_iterator<ExcludeNULLLiteralImpl<char, 2>::iterator> );
}

BOOST_AUTO_TEST_CASE( test_begin ) {
  using namespace violet::STRING;

  STATIC_CONSTINIT Literal literal( "directory" );
  STATIC_CONSTINIT Literal literal_original( "directory" );
  STATIC_CONSTINIT Literal literal_last( "darectory" );

  auto itr = literal.begin();
  BOOST_CHECK( *itr == 'd' );

  ++itr;
  BOOST_CHECK( *itr == 'i' );

  *itr = 'a';
  BOOST_CHECK( ( literal == literal_original ) == false );
  BOOST_CHECK( ( literal == literal_last ) == true );
}

BOOST_AUTO_TEST_CASE( test_end ) {
  using namespace violet::STRING;

  STATIC_CONSTINIT Literal literal( "directory" );
  auto itr = literal.end();
  --itr;
  BOOST_CHECK( *itr == 'y' );
}

BOOST_AUTO_TEST_SUITE_END()
