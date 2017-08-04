#include "./test.hpp"
#include <set>
#include <memory>

static
void
test_basics()
{
    Book book("Dune", "Frank Herbert");

    BOOST_TEST(book.title() == "Dune");
    BOOST_TEST(book.author() == "Frank Herbert");
}

static
void
test_is_pimpl()
{
    BOOST_TEST(false == impl_ptr<Foo>::value);
    BOOST_TEST(false == impl_ptr<int>::value);
    BOOST_TEST(false == impl_ptr<int*>::value);
    BOOST_TEST(false == impl_ptr<int const&>::value);
    BOOST_TEST(true  == impl_ptr<Shared>::value);
    BOOST_TEST(true  == impl_ptr<Shared const>::value);
    BOOST_TEST(false == impl_ptr<Shared*>::value);
    BOOST_TEST(true  == impl_ptr<Value>::value);
    BOOST_TEST(true  == impl_ptr<Base>::value);
    BOOST_TEST(true  == impl_ptr<Derived1>::value);
    BOOST_TEST(true  == impl_ptr<Derived1 const>::value);
    BOOST_TEST(true  == impl_ptr<Derived1 const&>::value);
    BOOST_TEST(true  == impl_ptr<Derived2>::value);
}

static
void
test_swap()
{
    Shared pt16 (5);
    Shared pt17 (5);
    Value  vt13 (5);
    Value  vt14 (5);

    BOOST_TEST(pt16.trace() == "Shared::implementation(int)");
    BOOST_TEST(pt17.trace() == "Shared::implementation(int)");
    BOOST_TEST(pt16 != pt17);
    BOOST_TEST(pt16.id() != pt17.id());
    BOOST_TEST(vt13.trace() == "Value::implementation(int)");
    BOOST_TEST(vt14.trace() == "Value::implementation(int)");
    BOOST_TEST(vt13.id() != vt14.id());

    int old_pt16_id = pt16.id();
    int old_pt17_id = pt17.id();
    int old_vt13_id = vt13.id();
    int old_vt14_id = vt14.id();

    pt16.swap(pt17);
    vt13.swap(vt14);

    BOOST_TEST(pt16.id() == old_pt17_id);
    BOOST_TEST(pt17.id() == old_pt16_id);
    BOOST_TEST(vt13.id() == old_vt14_id);
    BOOST_TEST(vt14.id() == old_vt13_id);
}

static
void
test_runtime_polymorphic_behavior()
{
    Base        base1 (1);
    Derived1 derived1 (1, 2);
    Derived2 derived2 (1, 2, 3);
    Base        base2 (derived1);
    Base        base3 (derived2);
    Base        base4 (Derived2(2,3,4)/*const ref to temporary*/);
    Base*         bp1 = &base1;
    Base*         bp2 = &base2;
    Base*         bp3 = &base3;
    Base*         bp4 = &derived1;
    Base*         bp5 = &derived2;
    Base         bad1 = impl_ptr<Base>::null();
    Base         bad2 = impl_ptr<Base>::null();
    Base         bad3 = impl_ptr<Derived1>::null();
    Base         bad4 = impl_ptr<Derived2>::null();
    Derived1     bad5 (impl_ptr<Derived1>::null());
    Derived2     bad6 (impl_ptr<Derived2>::null());

    BOOST_TEST(derived1.trace() == "Derived1::implementation(int, int)");
    BOOST_TEST(derived2.trace() == "Derived2::implementation(int, int, int)");

    BOOST_TEST(base1 != derived1);
    BOOST_TEST(base1 != derived2);
    BOOST_TEST(base2 == derived1);
    BOOST_TEST(base3 == derived2);
    BOOST_TEST(derived1 != base1);
    BOOST_TEST(derived2 != base1);
    BOOST_TEST(derived1 == base2);
    BOOST_TEST(derived2 == base3);

    bool bad1_bool1 =   bad5 ? true : false; // Shared conversion to bool
    bool bad1_bool2 = !!bad5; // Shared operator!()
    bool bad2_bool1 =   bad6 ? true : false; // Shared conversion to bool
    bool bad2_bool2 = !!bad6; // Shared operator!()

    BOOST_TEST(!bad1_bool1);
    BOOST_TEST(!bad1_bool2);
    BOOST_TEST(!bad2_bool1);
    BOOST_TEST(!bad2_bool2);

    BOOST_TEST(base2.call_virtual() == "Derived1::call_virtual()");
    BOOST_TEST(base3.call_virtual() == "Derived2::call_virtual()");
    BOOST_TEST(bp1->call_virtual() == "Base::call_virtual()");
    BOOST_TEST(bp2->call_virtual() == "Derived1::call_virtual()");
    BOOST_TEST(bp3->call_virtual() == "Derived2::call_virtual()");
    BOOST_TEST(bp4->call_virtual() == "Derived1::call_virtual()");
    BOOST_TEST(bp5->call_virtual() == "Derived2::call_virtual()");
}

static
void
test_null()
{
    Shared p01 = impl_ptr<Shared>::null(); BOOST_TEST(p01.trace() == "null");
    Shared p02 (impl_ptr<Shared>::null()); BOOST_TEST(p02.trace() == "null");
    Value  v01 = impl_ptr<Value>::null();  BOOST_TEST(v01.trace() == "null");
    Value  v02 (impl_ptr<Value>::null());  BOOST_TEST(v02.trace() == "null");

    Base     p03 (impl_ptr<    Base>::null()); BOOST_TEST(p03.trace() == "null");
    Derived1 p04 (impl_ptr<Derived1>::null()); BOOST_TEST(p04.trace() == "null");
    Derived2 p05 (impl_ptr<Derived2>::null()); BOOST_TEST(p05.trace() == "null");
    Base     p06 (p03); BOOST_TEST(p06.trace() == "null");
    Base     p07 (p04); BOOST_TEST(p07.trace() == "null");
    Base     p08 (p05); BOOST_TEST(p08.trace() == "null");
}

static
void
test_constructors()
{
    singleton_type single;
    Foo               foo;
    Foo const   const_foo = foo;
    Foo&              ref = foo;
    Foo const&  const_ref = const_foo;
    Foo*              ptr = &foo;
    Foo const*  const_ptr = &const_foo;

    Shared p11;             BOOST_TEST(p11.trace() == "Shared::implementation()");
    Shared p12 (5);         BOOST_TEST(p12.trace() == "Shared::implementation(int)");
    Shared p15 (5, 6);      BOOST_TEST(p15.trace() == "Shared::implementation(int, int)");
    Shared p13 = p12;       BOOST_TEST(p13.id() == p12.id()); // No copying. Implementation shared.
    Shared p14 (p12);       BOOST_TEST(p14.id() == p12.id()); // No copying. Implementation shared.
                            BOOST_TEST(p13.trace() == "Shared::implementation(int)"); // trace state is the same
                            BOOST_TEST(p14.trace() == "Shared::implementation(int)"); // trace state is the same
    Shared p16 (single);    BOOST_TEST(p16.trace() == "Shared::implementation()");
    Shared p17 (single);    BOOST_TEST(p17.trace() == "Shared::implementation()");
                            BOOST_TEST(p17.id() == p16.id()); // No copying. Implementation shared.

    Shared p21(foo);        BOOST_TEST(p21.trace() == "Shared::implementation(Foo&)");
    Shared p22(const_foo);  BOOST_TEST(p22.trace() == "Shared::implementation(Foo const&)");
    Shared p23(ref);        BOOST_TEST(p23.trace() == "Shared::implementation(Foo&)");
    Shared p24(const_ref);  BOOST_TEST(p24.trace() == "Shared::implementation(Foo const&)");
    Shared p25(ptr);        BOOST_TEST(p25.trace() == "Shared::implementation(Foo*)");
    Shared p26(const_ptr);  BOOST_TEST(p26.trace() == "Shared::implementation(Foo const*)");

    Value v11;              BOOST_TEST(v11.trace() == "Value::implementation()");
    Value v12 (5);          BOOST_TEST(v12.trace() == "Value::implementation(int)");
    Value v13 = v12;        BOOST_TEST(v13.trace() == "Value::implementation(Value::implementation const&)");
    Value v14 (v12);        BOOST_TEST(v14.trace() == "Value::implementation(Value::implementation const&)");
                            BOOST_TEST(v13.id() != v12.id()); // Copied.
                            BOOST_TEST(v14.id() != v12.id()); // Copied.
}

static
void
test_assignments()
{
    Value v11 (3);
    Value v12 (5);

    BOOST_TEST(v11 != v12);
    BOOST_TEST(v11.id() != v12.id());

    v11 = v12;

    BOOST_TEST(v11 == v12);
    BOOST_TEST(v11.id() != v12.id());
}

static
void
test_bool_conversions()
{
    Shared  p1;
    Value v1;
    Shared  p2 = impl_ptr<Shared>::null();
    Value v2 = impl_ptr<Value>::null();

    BOOST_TEST(p1.trace() == "Shared::implementation()");
    BOOST_TEST(v1.trace() == "Value::implementation()");
    BOOST_TEST(p2.trace() == "null");
    BOOST_TEST(v2.trace() == "null");

    BOOST_TEST(bool(p1));
    BOOST_TEST(!!p1);
    BOOST_TEST(bool(v1));
    BOOST_TEST(!!v1);
    BOOST_TEST(!bool(p2));
    BOOST_TEST(!p2);
    BOOST_TEST(!bool(v2));
    BOOST_TEST(!v2);
}

static
void
test_comparisons()
{
    Shared p1;
    Shared p2 (5);
    Shared p3 = p2;
    Value  v1;
    Value  v2 (5);
    Value  v3 = v2;

    BOOST_TEST(p2 != p1); // calls impl_ptr::op!=()
    BOOST_TEST(p2 == p3); // calls impl_ptr::op==()
    BOOST_TEST(v2 == v3);
    BOOST_TEST(v2.trace() == "Value::operator==(Value const&)");
    BOOST_TEST(v2 != v1);
    BOOST_TEST(v2.trace() == "Value::operator==(Value const&)");
}

static
void
test_singleton()
{
    singleton_type single;

    Shared p1 (single);
    Shared p2 (single);
    std::set<Shared> collected;

    collected.insert(p1);
    collected.insert(p2);

    BOOST_TEST(p1 == p2);       // Equality test
    BOOST_ASSERT (!(p1 < p2));  // Equivalence test
    BOOST_ASSERT (!(p2 < p1));  // Equivalence test
    BOOST_TEST(collected.size() == 1);
}

void
foo(std::unique_ptr<int> a)
{
}

int
main(int argc, char const* argv[])
{
    test_basics();
    test_null();
    test_is_pimpl();
    test_constructors();
    test_assignments();
    test_bool_conversions();
    test_comparisons();
    test_runtime_polymorphic_behavior();
    test_swap();
    test_singleton();

    return boost::report_errors();
}

