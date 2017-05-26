/******************************************************************************
 ------------- Copyright (c) 2009-2017 H a r a l d  A c h i t z ---------------
 ---------- < h a r a l d dot a c h i t z at g m a i l dot c o m > ------------
 ---- This Source Code Form is subject to the terms of the Mozilla Public -----
 ---- License, v. 2.0. If a copy of the MPL was not distributed with this -----
 ---------- file, You can obtain one at http://mozilla.org/MPL/2.0/. ----------
 ******************************************************************************/

#ifndef SL3__ERROR_HPP_
#define SL3__ERROR_HPP_

#include <exception>
#include <iosfwd>
#include <stdexcept>
#include <string>

#include <sl3/config.hpp>

namespace sl3
{
  /**
   * \brief Error codes
   *
   * These enum values used to create ErrType objects.
   *
   * Each ErrCode becomes an ErrType object.
   *
   */
  enum class ErrCode
  {
    SQL3Error       = 1, ///< sqlite3 error happened
    NoConnection    = 2, ///< no database
    OutOfRange      = 5, ///< index op out of range
    TypeMisMatch    = 6, ///< type cast problem
    NullValueAccess = 7, ///< accessing a value that is Null
    UNEXPECTED      = 99 ///< for everything that happens unexpected
  };

  constexpr const char*
  ErrCodeName (ErrCode ec)
  {
    return ec == ErrCode::SQL3Error
               ? "SQLite3Error"
               : ec == ErrCode::NoConnection
                     ? "NoConnection"
                     : ec == ErrCode::OutOfRange
                           ? "OutOfRange"
                           : ec == ErrCode::TypeMisMatch
                                 ? "TypeMisMatch"
                                 : ec == ErrCode::NullValueAccess
                                       ? "NullValueAccess"
                                       : ec == ErrCode::UNEXPECTED
                                             ? "UNEXPECTED"
                                             : "NA";
  }

  /**
   * \brief Exception base type
   *
   * A command base for all ErrType objects.
   * All exceptions thrown by the library wil have this base type.
   */
  class LIBSL3_API Error : public std::runtime_error
  {
  protected:
  public:
    /// Inherit constructors from std::runtime_error
    using std::runtime_error::runtime_error;

    /**
      * \brief Get ErrCode
      * \return the Errcode of the excetion
      */
    virtual ErrCode getId () const = 0;
  };

  /**
   * \brief overloaded stream operator
   * \param os ostream
   * \param e the Error
   * \return the ostream
   */
  LIBSL3_API std::ostream& operator<< (std::ostream& os, const Error& e);

  /**
   * \brief Object class representing an ErrCode
   *
   *  Allows typedef objects using an Errcode.
   *  Each sl3::ErrCode becomes an ErrType object.
   */
  template <ErrCode ec> class LIBSL3_API ErrType final : public Error
  {
  public:
    ErrType ()
    : Error ("")
    {
    }

    using Error::Error;

    ErrCode
    getId () const override
    {
      return ec;
    }
  };

  /// error that will be thrown if no open database was available but needed
  using ErrNoConnection = ErrType<ErrCode::NoConnection>;

  /// thrown if an index op is out of range
  using ErrOutOfRange = ErrType<ErrCode::OutOfRange>;

  /**
   * \brief thrown in case of an incorrect type usage
   */
  using ErrTypeMisMatch = ErrType<ErrCode::TypeMisMatch>;

  /// thrown in case of accessing a Null value field/parameter
  using ErrNullValueAccess = ErrType<ErrCode::NullValueAccess>;

  /// thrown if something unexpected happened, mostly used by test tools and in
  /// debug mode
  using ErrUnexpected = ErrType<ErrCode::UNEXPECTED>;

  /**
   * \brief packaging an error from sqlite3.
   * This exception will be thrown if sqlite3 reports an error.
   *
   * Holds the sqlite3 error code and the error message if it is available.
   * \see Database::getMostRecentErrMsg
   * \see Database::getMostRecentErrCode
   *
   * \n Extended error codes can be obtained through Database class.
   * \see Database::getMostRecentExtendedErrCode
   * \see Database::enableExtendedResultCodes
   *
   */
  template <> class LIBSL3_API ErrType<ErrCode::SQL3Error> final : public Error
  {
  public:
    /**
     * \brief c'tor
     * \param sl3ec sqite error code
     * \param sl3msg sqite error code
     */
    ErrType<ErrCode::SQL3Error> (int sl3ec, const char* sl3msg)
    : ErrType<ErrCode::SQL3Error> (sl3ec, sl3msg, "")
    {
    }

    /**
     * \brief c'tor
     * \param sl3ec sqite error code
     * \param sl3msg sqite error code
     * \param msg additional message
     */
    ErrType<ErrCode::SQL3Error> (int                sl3ec,
                                 const char*        sl3msg,
                                 const std::string& msg)
    : Error ("(" + std::to_string (sl3ec) + ":" + sl3msg + "):" + msg)
    , _sqlite_ec (sl3ec)
    , _sqlite_msg (sl3msg)
    {
    }

    ErrCode
    getId () const override
    {
      return ErrCode::SQL3Error;
    }

    /**
     * \brief  Get the sqlite3 error code
     * \return the sqlite3 error code
     */
    int
    SQLiteErrorCode () const
    {
      return _sqlite_ec;
    }

    /**
     * \brief  Get the sqlite3 error message.
     *
     * If the exception was created with a sqlite a error message
     * it can be accessed here.
     *
     * \return the sqlite3 error message
     */
    const std::string&
    SQLiteErrorMessage () const
    {
      return _sqlite_msg;
    };

  private:
    int         _sqlite_ec;
    std::string _sqlite_msg;
  };

  using SQLite3Error = ErrType<ErrCode::SQL3Error>;

#define ASSERT_EXCEPT(exp, except) \
  if (!(exp))                      \
  throw except (std::string (__FUNCTION__) + ": " + #exp)

} // ns

#endif /* ...ERROR_HPP_ */
