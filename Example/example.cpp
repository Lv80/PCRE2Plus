#define _CRT_SECURE_NO_WARNINGS
#include "PCRE2Plus.h"

#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include <clocale>


#if defined(_MSC_VER)
#include <io.h>
#include <fcntl.h>
#endif


#include <chrono>
#include <ctime>
std::string GetTheTime(){
    using namespace std::chrono;
    auto now = system_clock::now();
    time_point<system_clock> epoch;
    microseconds ms = duration_cast<milliseconds>(now - epoch);
    double epoch_time = (unsigned long long)ms.count() / 1000000.0;
    unsigned long long postfix = (unsigned long long)ms.count() % 1000000;
    std::time_t   time = static_cast<time_t>(epoch_time);
    std::tm tm = *std::localtime(&time);
    char Buf[80];
    std::strftime(Buf, sizeof(Buf), "%Y-%m-%dT%H:%M:%S", &tm);
    std::string finaltime(Buf);
    return finaltime.append(".").append(std::to_string(postfix));
}
bool throwX(std::string const& s) { 
    throw std::runtime_error(s); 
}
bool hopefully(bool v) { 
    return v; 
}
void setUtf8Mode(FILE* f, char const name[]) {
#if defined(_MSC_VER)
    int const newMode = _setmode(_fileno(f), _O_U8TEXT);
    hopefully(newMode != -1)
        || throwX(std::string() + "setmode failed for " + name);
#endif
}
std::wstring s2ws(const std::string& str) {
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX> converterX;
    return converterX.from_bytes(str);
}
std::string ws2s(const std::wstring& wstr) {
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}
std::string ws2s(const std::string& wstr) {
    return wstr;
}
void PRINT(const std::string s) {
#if defined(_MSC_VER)
    auto ws = s2ws(s);
    std::wcout << ws;
    return;
#endif    
    std::cout << s;
}
void PRINT(const std::wstring s) {
#if defined(_MSC_VER)
    std::wcout << s;
    return;
#endif
    std::cout << ws2s(s);
}
void PRINTLN(const std::string s) {
    if (s.size() > 0){
        PRINT(s);
    }
#if defined(_MSC_VER)
    std::wcout << std::endl;
    return;
#endif
    std::cout << std::endl;
}
void PRINTLN(const std::wstring s) {
    if (s.size() > 0){
        PRINT(s);
    }
#if defined(_MSC_VER)
    std::wcout << std::endl;
    return;
#endif
    std::cout << std::endl;
}
void print_header(std::wstring title){
    std::wstring x(5, '=');
    std::wstring t;
    t.append(x).append(title).append(x);
    PRINTLN("");
    PRINTLN(t);
}
void print_header(std::string title){
    std::string x(5, '=');
    std::string t;
    t.append(x).append(title).append(x);
    PRINTLN("");
    PRINTLN(GetTheTime() + ": " + t);
}


using namespace PCRE2Plus;
int main(int argc, char* argv[]) {
#if defined(_MSC_VER)
    setUtf8Mode(stdout, "stdout");
#endif
    //--------------------------------------------------------------------------
    print_header("Test Basic Matching");
    {
        if (auto R = re::compile(R"(\w+)")){
            std::string STR = "HelloWorld";
            if (auto M = R->search(STR)){
                PRINTLN(ws2s(M->group()));
            }
        }
        else{
            PRINTLN("NOK");
            PRINTLN(std::to_string(re::getlasterror()));
            PRINTLN(re::getlasterrorstr());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test Multiple Matching");
    {
        auto tmp = R"((\w+ )+)";
        std::string STR = R"(abc def ghi )";
        if (auto m = re::search(tmp, STR)){
            PRINTLN(m->group());
            auto v = m->groups();
            PRINTLN(std::to_string(v.size()));
            for (auto i = v.begin(); i < v.end(); i++){
                PRINTLN(*i);
            }
        }
        else{
            PRINTLN("NOK");
            PRINTLN(std::to_string(re::getlasterror()));
            PRINTLN(re::getlasterrorstr());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test re::finditer");
    {
        std::string STR = "111A222B333C";
        for (auto x = re::finditer(R"([A-Z])", STR); *x; ++x){
            PRINTLN(std::to_string(x->start()) + " " + std::to_string(x->end()) + " " + x->group());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test re::finditer 2");
    {
        std::string STR = "ABC def ggg";
        for (auto x = re::finditer("", STR); *x; ++x){
            PRINTLN(std::to_string(x->start()) +" " + std::to_string(x->end())+ " " + x->group());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test re::finditer 3");
    {
        std::string STR = "abc def ghi";
        for (auto x = re::finditer(R"(\w+)", STR); *x; x++){
            PRINTLN(std::to_string(x->start()) + " " + std::to_string(x->end()) + " " + x->group());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test re::split");
    {
        std::string STR = "abc00def11hig22";
        auto v = re::split(R"((\d)(\d))", STR);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(v.size()));
    }
    //--------------------------------------------------------------------------
    print_header("Test re::split 2");
    {
        std::string STR = "abc00def11hig22";
        auto v = re::split(R"(\d\d)", STR);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(v.size()));
    }
    //--------------------------------------------------------------------------
    print_header("Test re::split 3");
    {
        std::string STR = "abc00def11hig22";
        auto v = re::split(R"(notfound)", STR);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(v.size()));
    }
    //--------------------------------------------------------------------------
    print_header("Test re::findall");
    {
        std::string STR = "abc def hig";
        auto v = re::findall(R"(.+)", STR, re::IGNORECASE);
        PRINTLN(std::to_string(v.size()));
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test position");
    {
        auto pattern = re::compile(R"(d)");
        std::string STR = "dog";
        auto N = pattern->search(STR, 1);
        if (N){
            PRINTLN(N->group());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test DOTALL");
    {
        std::string STR = "ABC\r\nDEF\rHIJ\nGGG";
        if (auto m = re::search(R"(.+)", STR, re::DOTALL)){
            PRINTLN(m->group());
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test MULTILELINE");
    {
        std::string STR = "ABC\r\nDEF\rHIJ\nGGG";
        auto v = re::findall(R"(^.+$)", STR, re::MULTLINE);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test MULTILELINE 2");
    {
        std::string STR = "ABC\r\nDEF\rHIJ\nGGG";
        auto v = re::findall(R"(^.+$)", STR);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test MULTILELINE 3");
    {
        std::string STR = "ABC\r\nDEF\rHIJ\nGGG";
        auto v = re::findall(R"(\A.+$)", STR, re::MULTLINE);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test CASE");
    {
        std::string STR = "ABC\r\nDEF\rHIJ\nGGG";
        auto v = re::findall(R"(^(?i)[a-z]+$)", STR, re::MULTLINE);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test Unicode");
    {
        std::wstring STR = L"äbc";
        if (auto m = re::search(LR"(\p{L}+)", STR)){
            PRINTLN(m->group());
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
    //--------------------------------------------------------------------------
    print_header("Test Locale");
    {
#if defined(_MSC_VER)
        setlocale(LC_CTYPE, "swedish");
#else
        setlocale(LC_CTYPE, "sv_SE");
#endif
        std::wstring STR = LR"(Åbc)";
        auto r = re::sub(LR"(\w+)", std::wstring(LR"(äbc)"), STR , 0, re::LOCALE); 
        PRINTLN(ws2s(r));
    }
    //--------------------------------------------------------------------------
    print_header("Test subn (function)");
    {
        std::string STR("ABC def ggg");
        auto out = re::subn(R"(\w)",[=](const re::M_PT M){return M->group().append("X1"); }, STR);
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        PRINTLN(std::get<0>(out));
        PRINTLN(std::to_string(std::get<1>(out)));
    }
    //--------------------------------------------------------------------------
    print_header("Test subn (string)");
    {
        std::string STR("ABC def ggg");
        auto R = re::compile(R"()");
        auto out = R->subn(std::string("Y1"), STR);
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        PRINTLN(std::get<0>(out));
        PRINTLN(std::to_string(std::get<1>(out)));
    }
    //--------------------------------------------------------------------------
    print_header("Test sub_wide");
    {
        std::wstring STR(L"张三李四");
        auto R = re::compile(LR"(\w)");
        auto out = R->sub([=](const std::shared_ptr<re::MatchObjectW> & M){return M->group().append(L"1"); }, STR);
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        PRINTLN(out);
    }
    //--------------------------------------------------------------------------
    print_header("Test back reference");
    {
        std::string STR = "abc ddd hig";
        auto v = re::findall(R"((\w)\1\1)", STR, re::IGNORECASE);
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        PRINTLN(std::to_string(v.size()));;
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);;
        }
    }
    //--------------------------------------------------------------------------
    print_header("Test Wide char");
    PRINTLN(ws2s(L"双喜雙喜!"));
    {
        std::wstring STR = LR"(张三丰 周伯通)";
        auto r = re::sub(LR"XX((\S)\S+)XX", std::wstring(LR"XX($1某某)XX"), STR);
        PRINTLN(ws2s(r));
    }
    PRINTLN(std::to_string(re::getlasterror()));
    PRINTLN(re::getlasterrorstr());
    {
        std::wstring STR = LR"(张三丰 周伯通)";
        auto v = re::findall(LR"(\p{Han})", STR);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
    //--------------------------------------------------------------------------
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

    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match group");
    {
        //We don't support match, use search instead
        std::string STR = "Isaac Newton, physicist";
        if (auto m = re::search(R"(^(\w+) (\w+))", STR)){
            PRINTLN(m->group(0));  //Isaac Newton
            PRINTLN(m->group(1));  //Isaac
            PRINTLN(m->group(2));  //Newton
        }
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match group");
    {
        //We don't support match, use search instead
        std::string STR = "Malcolm Reynolds";
        if (auto m = re::search(R"(^(?P<first_name>\w+) (?P<last_name>\w+))", STR)){
            PRINTLN(m->group("first_name"));  //Malcolm
            PRINTLN(m->group("last_name"));   //Reynolds
            PRINTLN(m->group(1));  //Malcolm
            PRINTLN(m->group(2));  //Reynolds
        }
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match");
    {
        //We don't support match, use search instead
        std::string STR = "a1b2c3";
        if (auto m = re::search(R"XX(^(..)+)XX", STR)){
            PRINTLN(m->group(1));  //c3
        }
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match groups");
    {
        //We don't support match, use search instead
        std::string STR = "24.1632";
        if (auto m = re::search(R"(^(\d+)\.(\d+))", STR)){
            auto v = m->groups();
            for (auto i = v.begin(); i < v.end(); i++){
                PRINTLN(*i);
            }
        }
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match groups");
    {
        //We don't support match, use search instead
        std::string STR = "24";
        if (auto m = re::search(R"(^(\d+)\.?(\d+)?)", STR)){
            auto v = m->groups();
            for (auto i = v.begin(); i < v.end(); i++){
                PRINTLN(*i);
            }
        }
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.4 - match start end");
    {
        std::string email = "tony@tiremove_thisger.net";
        auto m = re::search("emove_this", email);
        //use str::string::substr instead of python sting slicing
        PRINT(email.substr(0, m->start(0) - 1));
        PRINTLN(email.substr(m->end(0), email.length() - m->end(0)));
        // tony@tiger.net
    }
    //--------------------------------------------------------------------------
    print_header("Example in 2.7.2 re.sub");
    {
        std::string STR = "def myfunc():";
        PRINTLN(re::sub(R"(def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):)",
            std::string(R"(static PyObject*\npy_$1(void)\n{)"),
            STR));
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
    //--------------------------------------------------------------------------
}
