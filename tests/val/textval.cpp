
#include <a4testing.hpp>

#include <sl3/value.hpp>
#include <sl3/error.hpp>
#include "eqorder.hpp"
#include <functional>

namespace sl3
{
  namespace
  {

    using VauleRelation = std::function<bool(const Value&, const Value&)>;

    VauleRelation strong_eq = std::equal_to<sl3::Value>() ;
    VauleRelation strong_lt = std::less<Value>() ;

    VauleRelation weak_eq = sl3::weak_eq;
    VauleRelation weak_lt = sl3::weak_lt ;
  
    void
    create ()
    {
      Value a(std::string("foo"));
      Value b(a);
      Value c = a ;
      Value d = std::string("foo") ;

      BOOST_CHECK_EQUAL (a, std::string("foo")) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (b, std::string("foo")) ;
      BOOST_CHECK_EQUAL (b.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (c, std::string("foo")) ;
      BOOST_CHECK_EQUAL (c.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (d, std::string("foo")) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Text) ;

      Value e;
      BOOST_CHECK (e.isNull()) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Null) ;
      e = std::string("foo") ;
      BOOST_CHECK_EQUAL (e, std::string("foo")) ;
      BOOST_CHECK_EQUAL (e.getType(), Type::Text) ;
      BOOST_CHECK (!e.isNull()) ;

    }


    void
    assing ()
    {

      Value a;
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;
      std::string text = std::string("foo") ;
      a = text ;
      BOOST_CHECK_EQUAL (a , text) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Text) ;
      BOOST_CHECK (!a.isNull()) ;

      Value b;
      b = a ;
      BOOST_CHECK_EQUAL (b , text) ;
      BOOST_CHECK_EQUAL (b , a) ;

      Value c{"bar"} ;
      BOOST_CHECK_EQUAL(c.text(), "bar") ;
      c = a  ;
      BOOST_CHECK_EQUAL(c.text(), "foo") ;

      Value d{} ;
      d = std::move (c) ;
      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL(d.text(), "foo") ;

      Value e{""};
      BOOST_CHECK_EQUAL(e.text(), "") ;
      e = std::string ("bar");
      BOOST_CHECK_EQUAL(e.text(), "bar") ;

      e = std::move (d);
      BOOST_CHECK (d.isNull()) ;
      BOOST_CHECK_EQUAL(e.text(), "foo") ;

      e = std::move (d);
      BOOST_CHECK (e.isNull()) ;


    }

    void
    move()
    {
      Value a{std::string("foo")};
      BOOST_CHECK (!a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Text) ;

      Value b{std::move(a)};
      BOOST_CHECK (a.isNull()) ;
      BOOST_CHECK_EQUAL (a.getType(), Type::Null) ;

      BOOST_CHECK_EQUAL (b.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (b, std::string("foo")) ;

      Value c = std::move(b);
      BOOST_CHECK (b.isNull()) ;

      BOOST_CHECK_EQUAL (c.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (c, std::string("foo")) ;

      Value d;
      d = std::move(c);

      BOOST_CHECK (c.isNull()) ;
      BOOST_CHECK_EQUAL (d.getType(), Type::Text) ;
      BOOST_CHECK_EQUAL (d, std::string("foo")) ;


    }

    void
     equality ()
     {
        Value a("a"), b("a"), c("a") ;
        using namespace eqo ;

        BOOST_CHECK (eq_reflexive (a, strong_eq));
        BOOST_CHECK (weak_reflexive (a,b, weak_eq));

        BOOST_CHECK (eq_symmetric (a,b, strong_eq));
        BOOST_CHECK (eq_symmetric (a,b, weak_eq));

        BOOST_CHECK (eq_transitive (a,b,c, strong_eq));
        BOOST_CHECK (eq_transitive (a,b,c, weak_eq));

     }



     void
     strictTotalOrdered ()
     {
       using namespace eqo ;

       BOOST_CHECK (strong_eq(Value{1.0}, Value{1.0}));
       BOOST_CHECK (!strong_eq(Value{1}, Value{1.0}));

       Value a("a"), b("a"), c("a") ;
       Value d("a"), e("b"), f("c") ;

       BOOST_CHECK (eq_reflexive (a, strong_eq));
       BOOST_CHECK (eq_symmetric (a,b, strong_eq));
       BOOST_CHECK (eq_transitive (a,b,c, strong_eq));

       BOOST_CHECK (irreflexive (a,b, strong_eq, strong_lt));
       BOOST_CHECK (lt_transitive (d,e,f, strong_lt));
       BOOST_CHECK (trichotomy (d,e, strong_eq, strong_lt));


     }

     void
     weakTotalOrdered ()
     {

       BOOST_CHECK (!weak_lt (Value("a"),Value(1)));
       BOOST_CHECK (weak_lt (Value("a"),Value("b")));
       BOOST_CHECK (weak_lt (Value("a"),Value(Blob{})));

     }



     void
     eject()
     {
       // weak order makes only senes with  int and real

       // but I use this for the eject test

       std::string txt = "a";
       Value val (txt);

       BOOST_CHECK_THROW(val.ejectBlob(), ErrTypeMisMatch);

       auto txt2 =  val.ejectText() ;

       BOOST_CHECK (txt2  == txt2 );

       BOOST_CHECK (val.isNull() );

       BOOST_CHECK_THROW(val.ejectText(), ErrNullValueAccess);

     }



     void
     compareWithOthers ()
     {
       Value nullVal ;
       Value intVal (1) ;
       Value realVal (1.0) ;
       Value textVal ("a");
       Value blobVal (Blob{});

       BOOST_CHECK (textVal != nullVal);
       BOOST_CHECK (textVal > nullVal);

       BOOST_CHECK (textVal != intVal);
       BOOST_CHECK (textVal > intVal);

       BOOST_CHECK (textVal != realVal);
       BOOST_CHECK (textVal > realVal);

       BOOST_CHECK (textVal != blobVal);
       BOOST_CHECK (textVal < blobVal);

       BOOST_CHECK (textVal != std::string());
       BOOST_CHECK (textVal == std::string("a"));

       BOOST_CHECK (textVal != 1);
       BOOST_CHECK (textVal != int64_t{1});
       BOOST_CHECK (textVal != 1.1);

       BOOST_CHECK (! (Value{""} < Value{2}) );



     }

     void
     implicitConvert ()
     {
        Value a("foo");

        BOOST_CHECK_NO_THROW({ std::string x = a  ; (void)x; });
        BOOST_CHECK_THROW({ Blob x = a  ; (void)x; }, ErrTypeMisMatch);
        BOOST_CHECK_THROW({ int x = a  ; (void)x; }, ErrTypeMisMatch);
        BOOST_CHECK_THROW({ int64_t x = a  ; (void)x; }, ErrTypeMisMatch);
        BOOST_CHECK_THROW({ double x = a  ; (void)x; }, ErrTypeMisMatch);

        {
          std::string x = a ;
          BOOST_CHECK_EQUAL(x , "foo") ;
        }


        BOOST_CHECK_THROW({ std::string x = Value{}; (void)x; },
                          ErrNullValueAccess);


        BOOST_CHECK_THROW( Value{}.text () ,
                          ErrNullValueAccess);

        BOOST_CHECK_THROW( Value{1}.text () ,
                          ErrTypeMisMatch);


      }


    a4TestAdd (a4test::suite ("textval")
          .addTest ("create", create)
          .addTest ("assing", assing)
          .addTest ("move", move)
          .addTest ("equality", equality)
          .addTest ("strictTotalOrdered", strictTotalOrdered)
          .addTest ("weakTotalOrdered", weakTotalOrdered)
          .addTest ("compareWithOthers", compareWithOthers)
          .addTest ("implicitConvert", implicitConvert)
          .addTest ("eject", eject)
          );
  }
}

