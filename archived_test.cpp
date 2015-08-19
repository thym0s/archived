#include "archived.h"

#include <vector>
#include <numeric>
#include <iostream>

bool check_equal( int a , int b , const std::string & msg )
{
  std::cout << msg << ' '
            << "First Value: " << a << ", "
            << "Second Value: " << b << ". ";
  if( a == b )
  {
    std::cout << "OK. \n";
    return true;
  } else {
    std::cout << "Error. \n";
    return false;
  }
}

int main ( int argc , const char ** argv )
{
  // provide the test data
  std::vector<int> test_data = { 3 , 4 , 7 , 9 , 4 , 5 , 7 , 94 };
  int initial_value = 13;

  //Test constructor
  archived<int> tested_object_1( initial_value );

  //Check if initial value is correct
  if( !check_equal( initial_value ,
                    tested_object_1.value() ,
                    "Value after construction." ) )
  {
    return 1;
  }

  //Test the increment and calculation of diff;
  const auto test_data_begin = test_data.begin(),
             test_data_end   = test_data.end();

  std::vector< archived<int>::version > version_vector;
  std::vector<int> control_values;

  version_vector.push_back( tested_object_1.current() );
  control_values.push_back( initial_value );

  auto test_data_iterator = test_data_begin;

  //First Run

  std::cout << "Increment, First Run. \n";
  std::cout.flush();

  for( ; test_data_iterator != test_data_end ; ++test_data_iterator )
  {  
    auto new_version = tested_object_1.increment_by( *test_data_iterator );
    version_vector.push_back( new_version );

    auto new_control_value = *control_values.rbegin() + *test_data_iterator;
    control_values.push_back( new_control_value );
  }

  //Check

  std::cout << "Increment Check, First Run. \n";
  std::cout.flush();

  const auto version_begin = version_vector.begin(),
             version_end   = version_vector.end();

  auto version_iterator = version_begin;
  auto control_iterator = control_values.begin();

  auto final_value = *control_values.rbegin();

  for( ; version_iterator != version_end ; ++version_iterator ,
                                           ++control_iterator)
  {  
    int should_be = final_value - *control_iterator;
    
    if( !check_equal( should_be ,
                      diff_to_current( *version_iterator ) ,
                      "Diffs to Current, First Run." ) ) 
    {
      return 1;
    }
  }

  return 0;
}

