#include <gtest/gtest.h>
namespace testing {
namespace internal {
enum GTestColor {
    COLOR_DEFAULT,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW
};

extern void ColoredPrintf(GTestColor color, const char * fmt, ...);
}
}
#define PRINTF(...)                                                                        \
    do {                                                                                   \
        testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] "); \
        testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__);    \
    } while (0)

// C++ stream interface
class TestCout : public std::stringstream {
    public:
    ~TestCout() {
        PRINTF("%s", str().c_str());
    }
};

#define TEST_COUT TestCout()

#include "PCRE2Plus.h"
#include <iostream>
#include <string>
using namespace PCRE2Plus;
//re::usecache = true;

//------------------------------------------------------------------------------
TEST(pcre2plus, basic_matching) {
    std::string str = "HelloWorld";
    auto        R   = re::compile(R"(\w+)");
    auto        M   = R->search(str.c_str());
    ASSERT_TRUE(R != nullptr);
    EXPECT_TRUE(re::getlasterror() == 100);
    ASSERT_TRUE(M != nullptr);
    EXPECT_TRUE(M->group() == str);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, multiple_matching) {
    std::string pattern = R"((\w+ )+)";
    auto        M = re::search(pattern, "abc def ghi ");
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    //The below result is correct, duplicated groups are flushed
    auto v1 = M->group();
    EXPECT_EQ("abc def ghi ", v1);
    auto v2 = M->groups();
    EXPECT_EQ(std::vector<std::string>({"ghi "}), v2);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_finditer) {
    std::string              str           = "111A222BBBBB333CC";
    std::vector<std::string> expect_result = {"3 4 A", "7 12 BBBBB", "15 17 CC"};
    size_t                   i             = 0;
    for (auto x = re::finditer(R"([A-Z]+)", str); *x; ++x) {
        std::string tmp_str = std::to_string(x->start()) + " " + std::to_string(x->end()) + " " + x->group();
        EXPECT_EQ(expect_result[i++], tmp_str);
    }
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_finditer_2) {
    std::string str = "ABC def ggg";
    size_t      i   = 0;
    for (auto x = re::finditer("", str); *x; ++x) {
        EXPECT_EQ(i, x->start());
        EXPECT_EQ(i, x->end());
        EXPECT_EQ("", x->group());
        i++;
    }
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_finditer_3) {
    std::string              str           = "abc def ghi";
    std::vector<std::string> expect_result = {"0 3 abc", "4 7 def", "8 11 ghi"};
    size_t                   i             = 0;
    for (auto x = re::finditer(R"(\w+)", str); *x; x++) {
        std::string tmp_str = std::to_string(x->start()) + " " + std::to_string(x->end()) + " " + x->group();
        EXPECT_EQ(expect_result[i++], tmp_str);
    }
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_split) {
    std::string str = "abc00def11ghi22";
    auto        v   = re::split(R"(\d\d)", str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"abc", "def", "ghi", ""}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_split_2) {
    std::string str = "abc00def11ghi22";
    auto        v   = re::split(R"((\d\d))", str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"abc", "00", "def", "11", "ghi", "22", ""}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_split_3) {
    std::string str = "abc00def11ghi22";
    auto        v   = re::split(R"((\d)(\d))", str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"abc", "0", "0", "def", "1", "1", "ghi", "2", "2", ""}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_split_4) {
    std::string str = "abc00def11ghi22";
    auto        v   = re::split(R"(notfound)", str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"abc00def11ghi22"}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_findall) {
    std::string str = "abc def ghi";
    auto        v   = re::findall(R"(.+)", str, re::IGNORECASE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"abc def ghi"}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, position) {
    auto pattern = re::compile(R"(d)");
    EXPECT_EQ(re::getlasterror(), 100);
    std::string str = "dog";
    auto        N   = pattern->search(str, 1);
    //NO FOUND, but last error still should be 100
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_TRUE(N == nullptr);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_DOTALL) {
    std::string str = "ABC\r\nDEF\rHIJ\nGGG";
    auto        M   = re::search(R"(.+)", str, re::DOTALL);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ("ABC\r\nDEF\rHIJ\nGGG", M->group());
}
//--------------------------------------------------------------------------
TEST(pcre2plus, re_MULTILELINE) {
    std::string str = "ABC\r\nDEF\rHIJ\nGGG";
    auto        v   = re::findall(R"(^.+$)", str, re::MULTLINE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"ABC", "DEF", "HIJ", "GGG"}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_MULTILELINE_1) {
    std::string str = "ABC\r\nDEF\rHIJ\nGGG";
    auto        v   = re::findall(R"(^.+$)", str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>(), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_MULTILELINE_2) {
    std::string str = "ABC\r\nDEF\rHIJ\nGGG";
    auto        v   = re::findall(R"(\A.+$)", str, re::MULTLINE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"ABC"}), v);
}
//--------------------------------------------------------------------------
TEST(pcre2plus, re_indications) {
    std::string str = "ABC\r\nDEF\rHIJ\nGGG";
    auto        v   = re::findall(R"(^(?i)[a-z]+$)", str, re::MULTLINE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"ABC", "DEF", "HIJ", "GGG"}), v);
}
//--------------------------------------------------------------------------
TEST(pcre2plus, re_back_reference) {
    std::string str = "abc ddd hig";
    auto        v   = re::findall(R"((\w)\1\1)", str, re::IGNORECASE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::string>({"ddd"}), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_subn_fun) {
    std::string str("ABC def ggg");
    auto out = re::subn(R"(\w)", [=](const re::M_PT M) { auto x = M->group(); return x.append("X1"); }, str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(9, std::get<1>(out));
    EXPECT_EQ("AX1BX1CX1 dX1eX1fX1 gX1gX1gX1", std::get<0>(out));
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_subn_str) {
    std::string str("ABC def ggg");
    auto        R = re::compile(R"()");
    ASSERT_TRUE(R != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    auto out = R->subn(std::string("Y1"), str);
    EXPECT_EQ(12, std::get<1>(out));
    EXPECT_EQ("Y1AY1BY1CY1 Y1dY1eY1fY1 Y1gY1gY1gY1", std::get<0>(out));
}
//------------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match) {
    //We don't support match, use search instead
    std::string str = "a1b2c3";
    auto        M   = re::search(R"XX(^(..)+)XX", str);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ("c3", M->group(1));
}
//------------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match_group) {
    //We don't support match, use search instead
    std::string str = "Isaac Newton, physicist";
    auto        M   = re::search(R"(^(\w+) (\w+))", str);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ("Isaac Newton", M->group(0));
    EXPECT_EQ("Isaac", M->group(1));
    EXPECT_EQ("Newton", M->group(2));
}
//--------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match_group_2) {
    //We don't support match, use search instead
    std::string str = "Malcolm Reynolds";
    auto        M   = re::search(R"(^(?P<first_name>\w+) (?P<last_name>\w+))", str);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ("Malcolm", M->group(1));
    EXPECT_EQ("Reynolds", M->group(2));
    EXPECT_EQ("Malcolm", M->group("first_name"));
    EXPECT_EQ("Reynolds", M->group("last_name"));
}
//--------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match_group_3) {
    //We don't support match, use search instead
    std::string str = "24.1632";
    auto        M   = re::search(R"(^(\d+)\.(\d+))", str);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    auto v = M->groups();
    EXPECT_EQ(std::vector<std::string>({"24", "1632"}), v);
}
//--------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match_group_4) {
    //We don't support match, use search instead
    std::string str = "24";
    auto        M   = re::search(R"(^(\d+)\.?(\d+)?)", str);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    auto v = M->groups();
    EXPECT_EQ(std::vector<std::string>({"24"}), v);
    //FIXME : python result ('24',None)
}
//--------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_4_match_group_5) {
    std::string email = "tony@tiremove_thisger.net";
    auto        M     = re::search("emove_this", email);
    ASSERT_TRUE(M != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    //use str::string::substr instead of python sting slicing
    EXPECT_EQ("tony@ti", email.substr(0, M->start(0) - 1));
    EXPECT_EQ("ger.net", email.substr(M->end(0), email.length() - M->end(0)));
}
//--------------------------------------------------------------------------
TEST(pcre2plus, py_Example_in_2_7_2_re_sub) {
    std::string str = "def myfunc():";
    auto        out = re::sub(R"(def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):)",
                       std::string(R"(static PyObject*\npy_$1(void)\n{)"),
                       str);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ("static PyObject*\npy_myfunc(void)\n{", out);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_sub_wstring) {
    std::wstring str(L"双喜雙喜");
    auto R =             re::compile(LR"(\w)");
    ASSERT_TRUE(R != nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    auto out = R->sub([=](const re::MW_PT M) { return M->group().append(L"1"); }, str);
    EXPECT_EQ(L"双1喜1雙1喜1", out);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_sub_wstring_1) {
    std::wstring str = LR"(张三丰 周伯通)";
    auto out = re::sub(LR"((\S)\S+)", std::wstring(LR"XX($1某某)XX"), str);
    EXPECT_EQ(L"张某某 周某某", out);
}
TEST(pcre2plus, re_uniocde_inidcation) {
    std::wstring STR = LR"(张a三丰)";
    auto v = re::findall(LR"(\p{Han})",    STR);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(std::vector<std::wstring>({ L"张",L"三", L"丰" }), v);
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_Unicode) {
    std::wstring str = L"äbc";
    auto         M = re::search(LR"(\p{L}+)", str.c_str());
    ASSERT_TRUE(M !=nullptr);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(L"äbc", M->group());
}
//------------------------------------------------------------------------------
TEST(pcre2plus, re_locale) {
#if defined(_MSC_VER)
    setlocale(LC_CTYPE, "swedish");
#else
    setlocale(LC_CTYPE, "sv_SE");
#endif
    std::wstring str = LR"(Åbc)";
    auto out = re::sub(LR"(\w+)", std::wstring(LR"(äbc)"), str, 0, re::LOCALE);
    EXPECT_EQ(re::getlasterror(), 100);
    EXPECT_EQ(L"äbc", out);
}

int main(int argc, char * argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/*

#if defined(_MSC_VER)
#else
    print_header("G++ or clang++ to use unicode in basic string(seems not working)");
    {
        std::string STR = "Å";
        auto v = re::findall(R"(\p{L})", STR, re::U);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
#endif
    //--------------------------------------------------------------------------
#if defined(_MSC_VER)
#else
    print_header("G++ or clang++ to use wide unicode in basic string(seems not working)");
    {
        std::string STR = "梅 兰 竹 菊";
        auto v = re::findall(R"(\p{Han})", STR, re::U);
        for (auto i = v.begin(); i < v.end();i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
#endif
}
*/