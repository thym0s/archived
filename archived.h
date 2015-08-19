#include <forward_list>
/** @file */

/**
 @brief Computes the difference of old and current value.

 This function computes the accumulated increments of the 
 archived<> associated with old from old's creation time
 to present.

 @return The value difference between old and current version.
*/
template< class Version >
 typename Version::value_type
  diff_to_current
  (
    const Version &
     old /**< An old Version. */
  );

/**
 @brief A class to keep track of an incrementally updated variable.

 # Overview

 The class archived<Value> keeps track of a variable of
 type Value that is incrementally upgraded.

 It is possible to retrieve intermediate versions,
 which later on allow a computation of the difference
 between that version and the current version.


 ## Versions:

 An archived<> provides the concept of a version.
 A version is associated with the archived<> that created it.
 A version has a validity, it is either valid or invalid.

 A version is only valid as long as the associated
 archived<> exists.

 Valid versions can be used to compute the difference between 
 the value of the associated archived<> at creation time of the version
 and the current one.

 A version is valid when acquired via an archived<>, but can be invalidated
 by certain actions on the associated archived<>. 
*/
template< class Value >
class archived
{
 public:
  class version;

  typedef Value value_type; /**< @brief The archived value's type. */
  typedef version version_type; /**< @brief The type of versions provided. */

 private:
  class commit;

  typedef commit commit_type; /**< @internal @brief Commit type. 
                                   A commit is an atomic diff
                                   and points to the successing commit. */
  typedef std::forward_list< commit_type > storage_type;
                              /**< @internal @brief The container for commits.
                                   A forward list, because iterators shall
                                   continue to be valid if a new commit
                                   is added. */
  typedef typename storage_type::iterator iterator_type;
                            /**< @internal @brief Iterator type for storage. */

  friend version_type; /**< @internal */

  mutable storage_type storage_; /**< @internal @brief Storage for commits. 
                                  Should always begin with a head_commit.  */
  version_type last_; /**< @internal @brief Version of the initial commit. */
  
  /**
   @internal @brief Modifies the commit referenced by old
   to contain a diff to the current value.
   The succesor of *old is set to the head_commit
  */
  static void compute_diff_to_current
  (
    const iterator_type &
     old /**< The old commit to be updated */
  );

  /**
   @internal @brief Creates a commit with zero diff
   that is its own successor and stores it at the beginnin of storage.
  */ 
  void create_head_commit();

  /**
   @internal @brief Checks whether an iterator references the head_commit
   The head_commit can be recognized as being its own succesor.
  */ 
  static bool is_head_commit
  (
    const iterator_type &
     iterator_to_commit /**< The iterator to the commit that is checked */
  );

  friend value_type
   diff_to_current<version_type>( const version_type & old );
     /**< @internal */


 public:
  /**
   @brief Constructor that initializes the archived<> with initial_value.
   After construction, the archived<>'s current value is initial_value.
  */
  explicit archived
  (
    const value_type &
     initial_value /**< The initial value. */
  );

  archived( const archived< Value > & other ) = delete;
  archived( archived< Value > && other ) = delete;
  archived< Value > & operator= ( const archived< Value > & other ) = delete;
  archived< Value > & operator= ( archived< Value > && other ) = delete;

  /**
   @brief Increments the value by increment. 
   Uses operator+= on the value.
   Returns a new version.

   @return A new version.
  */
  version_type increment_by
  (
    const value_type &
     increment /**< The value to increment by. */
  );

  /**
   @brief Returns the current value.

   @return the current value.
  */
  value_type value() const;

  /**
   @brief Clears the stored history data.
   Leaves current value unchanged.
   All associated versions are invalidated.
   Returns a new (valid) version.

   @return A new version.
  */
  version_type clear_history();

  /**
   @brief Clears the stored history data.
   Sets current value to initial_value.
   All associated versions are invalidated.
   Returns a new (valid) version.

   @return A new version.
  */
  version_type reset
  (
    const value_type &
     initial_value /**< The new value. */
  );

  /**
   @brief Returns a new version.

   @return A new version.
  */
  version_type current() const;
};

/**
 @internal @brief A commit is an atomic diff.
 It contains an iterator to its successor and the value of the diff.
*/
template< class Value >
class archived< Value >::commit
  : public std::pair< archived< Value >::iterator_type , Value >
{
};

/**
 @brief A version of an archived<>

 An archived<> provides the concept of a version.
 A version is associated with the archived<> that created it.
 A version has a validity, it is either valid or invalid.

 A version is only valid as long as the associated
 archived<> exists.

 Valid versions can be used to compute the difference between 
 the value of the associated archived<> at creation time of the version
 and the current one.

 A version is valid when acquired via an archived<>, but can be invalidated
 by certain actions on the associated archived<>. 
*/
template< class Value >
class archived< Value >::version
{
 public:
  typedef Value value_type; /**< @brief The associated archived value's type. */
  typedef archived< Value > archive_type; 
                            /**< @brief The associated archived's type */

 private:
  typedef archive_type::iterator_type iterator_type;
  
  friend archived< Value >; /**< @internal */

  iterator_type archive_iterator_; /**< @internal @brief Points to the
                                        first commit after the version. */
  
  friend value_type
   diff_to_current<version_type>( const version & old );
    /**< @internal */

  /**
   @internal @brief A constructor initializing archive_iterator_
   with archive_iterator.
  */
  explicit version
  (
    const iterator_type &
     archive_iterator /**< The initial archive_iterator_ */
  );

 public:
  /**
   @brief Default Constructor.
   A default constructed version is not valid.
  */
  version();

  /**
   @brief Default Copy Constructor
   A copy constructed version has the same same
   properties as the source.
  */
  version
  (
    const version &
     other /**< The source of the copy. */
  ) = default;

  /**
   @brief Default Move Constructor
   A move constructed version has the same same
   properties as the source.
  */
  version
  (
    version &&
     other /**< The source of the move. */
  ) = default;

  /**
   @brief Default Assignment operator
   Copies from other.
   An assigned version has the same same
   properties as the source.

   @return A reference to *this
  */
  version & operator=
  (
    const version &
     other /**< The source of the copy. */
  ) = default;
  
  /**
   @brief Default Move Assignment operator
   Moves from other.
   A move assigned version has the same same
   properties as the source.

   @return A reference to *this
  */
  version & operator=
  (
    version &&
     other /**< The source of the copy. */
  ) = default;
};



/*
  Implementation of archived<> class members
*/

template< class Value>
 void
  archived< Value >::compute_diff_to_current
  (
    const typename archived< Value >::iterator_type &
     old
  )
{
  auto next = old->first;
  if( ! is_head_commit( next ) )
  {
    compute_diff_to_current( next );
    old->second += next->second;
    old->first = next->first;
  }
}

template< class Value >
 void
  archived< Value >::create_head_commit() 
{
  commit_type head; // Value initialization of std::pair :
                    // head.second is properly initialized.
  storage_.push_front( head );
  storage_.front().first = storage_.begin();
}

template< class Value >
 bool
  archived< Value >::is_head_commit
  (
    const typename archived< Value >::iterator_type &
     iterator_to_commit
  )
{
  return ( iterator_to_commit->first == iterator_to_commit );
}

template< class Value >
  archived< Value >::archived
  (
    const value_type &
     initial_value
  )
  : storage_() ,
    last_()
{
  reset( initial_value );
}

template< class Value>
 typename archived< Value >::version_type
  archived< Value >::increment_by
  ( 
    const typename archived< Value >::value_type &
     increment
  )
{
  const auto old_head = storage_.begin();
  old_head->second = increment;
  create_head_commit();

  const auto new_head = storage_.begin();
  old_head->first = new_head;
  return version_type( new_head );
}

template< class Value >
 typename archived< Value >::value_type
  archived< Value >::value() const
{
  return diff_to_current( last_ );
}

template< class Value >
 typename archived< Value >::version_type
  archived< Value >::clear_history()
{
  const auto current_value = value();
  reset( value() );
}

template< class Value >
 typename archived< Value >::version_type
  archived< Value >::reset
  (
    const typename archived< Value>::value_type &
     initial_value
  )
{
 storage_.clear();
 create_head_commit();
 last_ = version_type( storage_.begin() );

 return increment_by( initial_value );
}

template< class Value >
 typename archived< Value >::version_type
  archived< Value >::current() const
{
  return version_type( storage_.begin() );
}

/*
  Implementation of archived<>::version class members
*/

template< class Value>
  archived< Value >::version::version
  ( 
    const iterator_type &
     archive_iterator
  )
  : archive_iterator_( archive_iterator )
{
}

template< class Value>
  archived< Value>::version::version()
  : archive_iterator_()
{
}

/*
 Implementation of non-member functions
*/

template< class Version >
 typename Version::value_type
  diff_to_current
  (
    const Version &
     old
  )
{
  const auto archive_iterator = old.archive_iterator_;

  Version::archive_type::compute_diff_to_current( old.archive_iterator_ );
  return archive_iterator->second;
}

