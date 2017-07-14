#include "../testing.hpp"
#include <sl3/database.hpp>
#include <sl3/error.hpp>

#include <string>

SCENARIO("creating a database")
{
  GIVEN ("a valid and an invalid database name/path")
  {
    auto good = ":memory:" ;
    auto bad = "/this/does/not/exist/123" ;

    WHEN ("using the valid name/path")
    {
      THEN ("constructing the db works")
      {
        CHECK_NOTHROW(sl3::Database{good});
      }
    }

    WHEN ("using the invalid name/path")
    {
      THEN ("constructing the db works")
      {
        CHECK_THROWS_AS(sl3::Database{bad}, sl3::SQLite3Error) ;
      }
    }
  }
}



SCENARIO("creating some test data")
{
  GIVEN ("an im memory database")
  {
    sl3::Database db{":memory:"};

    WHEN ("executing invalid SQL")
    {
      auto wrong_sql = "CRATE NABLE foo (f1, f2);" ;
      THEN ("ecexute triggers an exception")
      {
        CHECK_THROWS_AS (db.execute (wrong_sql), sl3::SQLite3Error) ;
      }

    }

    WHEN ("creating a table and some test data")
    {
      db.execute (
          "CREATE TABLE tbltest (intFld INTEGER,txtFld TEXT, dblFld real );"
          "INSERT INTO tbltest VALUES (1, 'eins', 1.111) ;"
          "INSERT INTO tbltest VALUES (2, 'zwei', 2.22) ;"
          "INSERT INTO tbltest VALUES (3, NULL, NULL) ;"
      );

      THEN ("I can veryfy that the table and some test data exists")
      {
        CHECK (db.getTotalChanges () == 3) ;
        CHECK_NOTHROW (db.execute ("DELETE FROM tbltest;")) ;
        CHECK (db.getTotalChanges () == 6) ;
        CHECK (db.getRecentlyChanged () == 3) ;
      }

    }
  }
}


SCENARIO("move construct a database")
{
  GIVEN("a db with a table")
  {
    sl3::Database db{":memory:"};

    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;");

    CHECK_NOTHROW (db.execute ("SELECT COUNT(*) FROM tbltest;")) ;

    WHEN ("Moveconstruct a new db from an existing one")
    {
      sl3::Database db1{std::move(db)} ;
      THEN("the new constructed db has data")
      {
        CHECK_NOTHROW (db1.execute ("SELECT COUNT(*) FROM tbltest;")) ;
      }

      AND_THEN("the moved db is disconnected")
      {
        CHECK_THROWS_AS (db.execute ("SELECT COUNT(*) FROM tbltest;"),
                          sl3::ErrNoConnection) ;

      }
    }
  }
}


SCENARIO("prepare commands")
{
  GIVEN("a db with a table")
  {
    sl3::Database db{":memory:"};

    db.execute ("CREATE TABLE tbltest (f INTEGER);"
                  "INSERT INTO tbltest VALUES (1) ;");

    REQUIRE_NOTHROW (db.execute ("SELECT COUNT(*) FROM tbltest;")) ;

    WHEN ("compiling a valid sql statement")
    {
      auto sql = "SELECT * FROM tbltest;" ;

      THEN("a new command is created")
      {
        CHECK_NOTHROW(
          auto cmd = db.prepare (sql) ;
          (void)cmd;
        );
      }
    }

    WHEN ("compiling an invalid sql statement")
    {
      auto sql = "SELECT_X * X_FROM tbltest;" ;

      THEN("creating a command throws")
      {
        CHECK_THROWS_AS((void)db.prepare (sql), sl3::SQLite3Error);
      }
    }

    WHEN ("compiling on a disconnected db")
    {
      auto db1 = std::move (db) ;
      auto sql = "SELECT * FROM tbltest;" ;
      auto sql1 = "INSERT INTO tbltest VALUES (?);" ;

      THEN("creating a command throws")
      {
        CHECK_THROWS_AS((void)db.prepare (sql), sl3::ErrNoConnection);

        sl3::DbValues vals{ sl3::DbValue{1} } ;
        CHECK_THROWS_AS((void)db.prepare (sql1, vals), sl3::ErrNoConnection);
      }
    }

    WHEN ("compiling sql with the correct number of parameters")
    {
      auto sql = "INSERT INTO tbltest VALUES (?);" ;
      THEN("creating a command succeeds")
      {
        sl3::DbValues vals{ sl3::DbValue{1} } ;
        CHECK_NOTHROW((void)db.prepare (sql,vals));
      }
    }

    WHEN ("compiling sql with a wrong number of parameters")
    {
      auto sql = "INSERT INTO tbltest VALUES (?);" ;

      THEN("creating a command throws")
      {
        sl3::DbValues vals{ sl3::DbValue{1}, sl3::DbValue{2} } ;
        CHECK_THROWS((void)db.prepare (sql,sl3::DbValues{}));
        CHECK_THROWS((void)db.prepare (sql,vals));
        // Details about the exception in the command tests
      }
    }
  }
}












