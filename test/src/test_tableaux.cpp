#include "catch.hpp"
#include "Flows.hpp"

TEST_CASE( "Testing tableaux", "[tableaux]" ) {
    auto tab = Flows::CB2;
    REQUIRE( tab('I', 'a', 0, 0) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 0, 1) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 0, 2) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 1, 0) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 1, 1) == 2.0/5.0 );
    REQUIRE( tab('I', 'a', 1, 2) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 2, 0) == 0.0/1.0 );
    REQUIRE( tab('I', 'a', 2, 1) == 5.0/6.0 );
    REQUIRE( tab('I', 'a', 2, 2) == 1.0/6.0 );

    REQUIRE( tab('E', 'a', 0, 0) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 0, 1) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 0, 2) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 1, 0) == 2.0/5.0 );
    REQUIRE( tab('E', 'a', 1, 1) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 1, 2) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 2, 0) == 0.0/1.0 );
    REQUIRE( tab('E', 'a', 2, 1) == 1.0/1.0 );
    REQUIRE( tab('E', 'a', 2, 2) == 0.0/1.0 );
    
    REQUIRE( tab('I', 'b',    0) == 0.0/1.0 );
    REQUIRE( tab('I', 'b',    1) == 5.0/6.0 );
    REQUIRE( tab('I', 'b',    2) == 1.0/6.0 );
    REQUIRE( tab('E', 'b',    0) == 0.0/1.0 );
    REQUIRE( tab('E', 'b',    1) == 5.0/6.0 );
    REQUIRE( tab('E', 'b',    2) == 1.0/6.0 );

    REQUIRE( tab('I', 'c',    0) == 0.0/1.0 );
    REQUIRE( tab('I', 'c',    1) == 2.0/5.0 );
    REQUIRE( tab('I', 'c',    2) == 1.0/1.0 );
    REQUIRE( tab('E', 'c',    0) == 0.0/1.0 );
    REQUIRE( tab('E', 'c',    1) == 2.0/5.0 );
    REQUIRE( tab('E', 'c',    2) == 1.0/1.0 );
}