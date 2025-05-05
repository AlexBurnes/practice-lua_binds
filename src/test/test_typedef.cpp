#include <gmock/gmock-matchers.h>
#include <gmock/gmock-more-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <typedef.h>
#include <non_movable.hpp>
#include <deffer.hpp>
#include "test_typedef.hpp"

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(TestUnless, UnlessFalse) {
    bool result = false;
    unless(false) result = true;
    EXPECT_EQ(true, result);
}

TEST(TestNonMovable, NonMovable) {
    non_move_allowed<nonmovable> t;
    EXPECT_TRUE(t());
}

TEST(TestNonMovable, NonCopyable) {
    non_copy_allowed<nonmovable> t;
    EXPECT_TRUE(t());
}

TEST(TestNonMovable, NonMovableCopyable) {
    non_move_and_copy_allowed<nonmovable> t;
    EXPECT_TRUE(t());
}

TEST(TestNonCopyable, Movable) {
    non_move_allowed<noncopyable> t;
    EXPECT_FALSE(t());
}

TEST(TestNonCopyable, NonCopyable) {
    non_copy_allowed<noncopyable> t;
    EXPECT_TRUE(t());
}

TEST(TestCopyable, NonMovable) {
    non_move_allowed<copyable> t;
    EXPECT_TRUE(t());
}

TEST(TestCopyable, Copyable) {
    non_copy_allowed<copyable> t;
    EXPECT_FALSE(t());
}

TEST(TestDerivedNonMovable, NonMovable) {
    class T:public nonmovable {};
    non_move_allowed<T> t;
    EXPECT_TRUE(t());
}

TEST(TestDerivedNonMovable, NonCopyable) {
    class T:public nonmovable {};
    non_copy_allowed<T> t;
    EXPECT_TRUE(t());
}

TEST(TestDerivedNonMovable, NonMovableCopyable) {
    class T:public nonmovable {};
    non_move_and_copy_allowed<T> t;
    EXPECT_TRUE(t());
}

TEST(TestDerivedNonCopyable, Movable) {
    class T:public noncopyable {};
    non_move_allowed<T> t;
    EXPECT_FALSE(t());
}

TEST(TestDerivedNonCopyable, NonCopyable) {
    class T:public noncopyable {};
    non_copy_allowed<T> t;
    EXPECT_TRUE(t());
}

TEST(TestDerivedCopyable, Copyable) {
    class T:public copyable {};
    non_copy_allowed<T> t;
    EXPECT_FALSE(t());
}

//!@not without implicity delete move ctor this test will fail couse if move disabled in base class fallback to copy!
// this test show move is enanble (thats not true)
TEST(TestDerivedCopyable, FailMovable) {
    class T:public copyable {};
    non_move_allowed<T> t;
    EXPECT_FALSE(t());
}

// this test show move is disabled, fallback to copy disabled
TEST(TestDerivedCopyable, NonMovable) {
    class T:public copyable,ctor { T(T &&) = delete;};
    non_move_allowed<T> t;
    EXPECT_TRUE(t());
}

//!@note above tests is defined for only this test that garantied deffer class is not movable or copyable!
TEST(TestDeffer, NonMovableCopyable) {
    non_move_and_copy_allowed<deffer<decltype([](){})>> t;
    EXPECT_TRUE(t());
}

TEST(TestDeffer, Deffered) {
    bool b = false;
    {
        deffer d([&b](){b = true;});
    }
    EXPECT_TRUE(b);
}

TEST(TestDeffer, DefferOrder) {
    int n = 3;
    {
        deffer a([&n](){if (n == 1) --n;});
        deffer b([&n](){if (n == 2) --n;});
        deffer c([&n](){if (n == 3) --n;});
    }
    EXPECT_EQ(n, 0);
}

TEST(TestDeffer, DefferFunc1) {
    auto f = [](bool &a) {a = true;};
    bool b = false;
    {
        deffer d(f, b);
    }
    EXPECT_TRUE(b);
}

TEST(TestDeffer, DefferFunc2) {
    auto f = [](bool a) {a = true;};
    bool b = false;
    {
        deffer d(f, b);
    }
    EXPECT_FALSE(b);
}

TEST(TestDeffer, DefferFunc3) {
    auto f = [](bool &a, std::string s) {
        a = true;
        std::cout << "string s='" << s << "'" << std::endl;
    };
    bool b = false;
    {
        deffer d(f, b, "test");
    }
    EXPECT_TRUE(b);
}