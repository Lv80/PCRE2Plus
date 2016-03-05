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


bool throwX(std::string const& s) { throw std::runtime_error(s); }
bool hopefully(bool v) { return v; }

void setUtf8Mode(FILE* f, char const name[]){
#if defined(_MSC_VER)
    int const newMode = _setmode(_fileno(f), _O_U8TEXT);
    hopefully(newMode != -1)
        || throwX(std::string() + "setmode failed for " + name);
#endif
}


std::wstring s2ws(const std::string& str)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX> converterX;
    return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
    typedef std::codecvt_utf8<wchar_t> convert_typeX;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    
    return converterX.to_bytes(wstr);
}

std::string ws2s(const std::string& wstr)
{
    return wstr;
}

using namespace PCRE2Plus;

void PRINT(const std::string s){
#if defined(_MSC_VER)
    auto ws = s2ws(s);
    std::wcout << ws;
    return;
#endif    
    std::cout << s;
}

void PRINT(const std::wstring s){
#if defined(_MSC_VER)
    std::wcout << s;
    return;
#endif
    std::cout << ws2s(s);
}

void PRINTLN(const std::string s){
    if (s.size() > 0){
        PRINT(s);
    }
#if defined(_MSC_VER)
    std::wcout << std::endl;
    return;
#endif
    std::cout << std::endl;
}

void PRINTLN(const std::wstring s){
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
    std::wstring x(10, '=');
    std::wstring t;
    t.append(x).append(title).append(x);
    PRINTLN(t);
}

void print_header(std::string title){
    std::string x(10, '=');
    std::string t;
    t.append(x).append(title).append(x);
    PRINTLN(t);
}



int main(int argc, char* argv[]) {
#if defined(_MSC_VER)
    setUtf8Mode( stdout, "stdout" );
#endif

    std::string c = R"(\w+)";
    auto C = re::compile(c);
    if (C){
        PRINTLN("OK");
        auto M = C->search("abc");
        if (M){
            PRINTLN(ws2s(M->group()));
        }
    }
    else{
        PRINTLN("NOK");
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }
   
    print_header("Test0.1");

    auto tmp = R"((\w+ )+)";
    auto M = re::search(tmp , R"(abc def ghi )");
	if (M){
        PRINTLN(M->group());
        auto v = M->groups();
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


    print_header("Test re::finditer");
    {
        for (auto x = re::finditer(R"([A-Z])", "111A222B333C"); !x->AtEnd(); ++*x.get()){
            PRINTLN(x->Get()->group());
        }
    }


    print_header("Test re::split");
    {
        auto v = re::split(R"((\d)(\d))", "abc00def11hig22");
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
    }

    print_header("Test re::findall");
    {
        auto v = re::findall(R"(.+)", "abc def hig", re::IGNORECASE);
        PRINTLN(std::to_string(v.size()));;

        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);;
        }
    }
    
    print_header("Test position");
    {
        auto pattern = re::compile(R"(d)");
        auto N = pattern->search("dog", 1);
        if (N){
            PRINTLN(N->group());;
        }
    }
    
    
    print_header("Test re::finditer");
    {
        for (auto x = re::finditer(R"(\w+)", "abc def ghi"); !x->AtEnd(); ++*x.get()){
            PRINTLN(x->Get()->group());
        }
    }
    
    print_header("Example in 2.7.4 - match group");
    {
        //We don't support match, use search instead
        auto m = re::search(R"(^(\w+) (\w+))", "Isaac Newton, physicist");
        if (m){
            PRINTLN(m->group(0));  //Isaac Newton
            PRINTLN(m->group(1));  //Isaac
            PRINTLN(m->group(2));  //Newton
        }
    }

    print_header("Example in 2.7.4 - match group");
    {
        //We don't support match, use search instead
        auto m = re::search(R"(^(?P<first_name>\w+) (?P<last_name>\w+))", "Malcolm Reynolds");
        if (m){
            PRINTLN(m->group("first_name"));  //Malcolm
            PRINTLN(m->group("last_name"));   //Reynolds
            PRINTLN(m->group(1));  //Malcolm
            PRINTLN(m->group(2));  //Reynolds
        }
    }

    print_header("Example in 2.7.4 - match");
    {
        //We don't support match, use search instead
        auto m = re::search(R"XX(^(..)+)XX", "a1b2c3");
        if (m){
            PRINTLN(m->group(1));  //c3
        }
    }

    print_header("Example in 2.7.4 - match groups");
    {
        //We don't support match, use search instead
        auto m = re::search(R"(^(\d+)\.(\d+))", "24.1632");
        if (m){
            auto v = m->groups();
            for (auto i = v.begin(); i < v.end(); i++){
                PRINTLN(*i);
            }
        }
    }

    print_header("Example in 2.7.4 - match groups");
    {
        //We don't support match, use search instead
        auto m = re::search(R"(^(\d+)\.?(\d+)?)", "24");
        if (m){
            auto v = m->groups();
            for (auto i = v.begin(); i < v.end(); i++){
                PRINTLN(*i);
            }
        }
    }

    print_header("Example in 2.7.4 - match start end");
    {
        std::string email = "tony@tiremove_thisger.net";
        auto m = re::search("emove_this", email);
        //use str::string::substr instead of python sting slicing
        PRINT(email.substr(0, m->start(0) - 1));
        PRINTLN(email.substr(m->end(0), email.length() - m->end(0)));
        // tony@tiger.net
    }

    print_header("Example in 2.7.2 re.sub");
    {
        PRINTLN(re::sub(R"(def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):)",
            R"(static PyObject*\npy_$1(void)\n{)",
            "def myfunc():"));
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }

    print_header("Test Unicode");
    {
        auto X = re::search(LR"(\p{L}+)", L"äbc");
        if (X){
            PRINTLN(X->group());
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());

    }

    print_header("Test Locale");
    {
#if defined(_MSC_VER)
        setlocale(LC_CTYPE, "swedish");
#else
        setlocale(LC_CTYPE, "sv_SE");
#endif
        auto r = re::sub(LR"(\w+)", LR"(äbc)", LR"(Åbc)", 0, re::LOCALE); 
        PRINTLN(ws2s(r));
    }

    
    print_header("Test back reference");
    {
        auto v = re::findall(R"((\w)\1\1)", "abc ddd hig", re::IGNORECASE);
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        PRINTLN(std::to_string(v.size()));;
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);;
        }
    }

    print_header("Test Wide char");
    PRINTLN(ws2s(L"双喜雙喜!"));
    
    {
        auto r = re::sub(LR"XX((\S)\S+)XX", LR"XX($1某某)XX", LR"(张三丰 周伯通)");
        PRINTLN(ws2s(r));
    }
    PRINTLN(std::to_string(re::getlasterror()));
    PRINTLN(re::getlasterrorstr());


    auto v = re::findall(LR"(\p{Han})", L"赵 钱 孙 李");
    for (auto i = v.begin(); i < v.end();i++){
        PRINTLN(*i);
    }
    PRINTLN(std::to_string(re::getlasterror()));
    PRINTLN(re::getlasterrorstr());
    
    
    print_header("G++ or clang++ to use unicode in basic string(seems not working)");
    {
        auto v = re::findall(R"(\p{L})", "Å", re::U);
        for (auto i = v.begin(); i < v.end(); i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
    }

    print_header("G++ or clang++ to use wide unicode in basic string(seems not working)");
    {
        auto v = re::findall(R"(\p{Han})", "梅 兰 竹 菊", re::U);
        for (auto i = v.begin(); i < v.end();i++){
            PRINTLN(*i);
        }
        PRINTLN(std::to_string(re::getlasterror()));
        PRINTLN(re::getlasterrorstr());
        
    }
    
}
