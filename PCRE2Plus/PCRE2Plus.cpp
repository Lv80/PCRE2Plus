#include "PCRE2Plus.h"
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <iostream>
#include <typeinfo> 
using namespace PCRE2Plus;
int re::lasterror = 0;
size_t re::erroroffset = 0;
//------------------------------------------------------------------------------
std::string re::getlasterrorstr(){
    char c[500];
    pcre2_get_error_message_8(re::lasterror, (PCRE2_UCHAR8*)c, 499);
    std::string Str(c);
    return Str;
}
//------------------------------------------------------------------------------ 
int re::geterroroffset(){
    return (int)re::erroroffset;
}
//------------------------------------------------------------------------------
int re::getlasterror(){
    return re::lasterror;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string re::escape(const std::string & unquoted){
    // from PCRE::QuoteMeta function
    std::string result;
    result.reserve(unquoted.size() << 1);

    // Escape any ascii character not in [A-Za-z_0-9].
    //
    // Note that it's legal to escape a character even if it has no
    // special meaning in a regular expression -- so this function does
    // that.  (This also makes it identical to the perl function of the
    // same name except for the null-character special case;
    // see `perldoc -f quotemeta`.)
    for (size_t ii = 0; ii < unquoted.length(); ++ii) {
        // Note that using 'isalnum' here raises the benchmark time from
        // 32ns to 58ns:
        if ((unquoted[ii] < 'a' || unquoted[ii] > 'z') &&
            (unquoted[ii] < 'A' || unquoted[ii] > 'Z') &&
            (unquoted[ii] < '0' || unquoted[ii] > '9') &&
            unquoted[ii] != '_' &&
            // If this is the part of a UTF8 or Latin1 character, we need
            // to copy this byte without escaping.  Experimentally this is
            // what works correctly with the regexp library.
            !(unquoted[ii] & 128)) {
            if (unquoted[ii] == '\0') {  // Special handling for null chars.
                // Can't use "\\0" since the next character might be a digit.
                result += "\\x00";
                continue;
            }
            result += '\\';
        }
        result += unquoted[ii];
    }
    return result;
}
//------------------------------------------------------------------------------
std::wstring re::escape(const std::wstring & unquoted){
    // from PCRE::QuoteMeta function
    std::wstring result;
    result.reserve(unquoted.size() << 1);

    // Escape any ascii character not in [A-Za-z_0-9].
    //
    // Note that it's legal to escape a character even if it has no
    // special meaning in a regular expression -- so this function does
    // that.  (This also makes it identical to the perl function of the
    // same name except for the null-character special case;
    // see `perldoc -f quotemeta`.)
    for (size_t ii = 0; ii < unquoted.length(); ++ii) {
        // Note that using 'isalnum' here raises the benchmark time from
        // 32ns to 58ns:
        if ((unquoted[ii] < 'a' || unquoted[ii] > 'z') &&
            (unquoted[ii] < 'A' || unquoted[ii] > 'Z') &&
            (unquoted[ii] < '0' || unquoted[ii] > '9') &&
            unquoted[ii] != '_' &&
            // If this is the part of a UTF8 or Latin1 character, we need
            // to copy this byte without escaping.  Experimentally this is
            // what works correctly with the regexp library.
            !(unquoted[ii] & 128)) {
            if (unquoted[ii] == '\0') {  // Special handling for null chars.
                // Can't use "\\0" since the next character might be a digit.
                result += L"\\x00";
                continue;
            }
            result += '\\';
        }
        result += unquoted[ii];
    }
    return result;
}
//------------------------------------------------------------------------------
std::shared_ptr<re::RegexObject> re::compile(const std::string & pattern, int flags){
    flags = flags | PCRE2_DUPNAMES;
    pcre2_code_8 * re_code = NULL;
    pcre2_compile_context_8 * ccontext = NULL;

    if (flags & re::LOCALE){
        auto tables = pcre2_maketables_8(NULL);
        ccontext = pcre2_compile_context_create_8(NULL);
        pcre2_set_character_tables_8(ccontext, tables);
    }
    flags &= !re::LOCALE;
    re_code = pcre2_compile_8(
        (PCRE2_SPTR8)pattern.c_str(),
        PCRE2_ZERO_TERMINATED,
        flags,
        &re::lasterror,
        &re::erroroffset,  //TODO: ERROR Position
        ccontext);
    if (re_code == NULL || re::lasterror != 100){
        return nullptr;
    }
    std::shared_ptr<re::RegexObject> p_c(new RegexObject(re_code, flags, pattern));
    return p_c;
}
//------------------------------------------------------------------------------
std::shared_ptr<re::RegexObjectW> re::compile(const std::wstring & pattern, int flags){
    flags |= (PCRE2_UTF | PCRE2_DUPNAMES | PCRE2_UCP);
    pcre2_code * re_code = NULL;
    pcre2_compile_context * ccontext = NULL;

    if (flags & re::LOCALE){
        auto tables = pcre2_maketables(NULL);
        ccontext = pcre2_compile_context_create(NULL);
        pcre2_set_character_tables(ccontext, tables);
    }
    flags &= !re::LOCALE;
    re_code = pcre2_compile(
        (PCRE2_SPTR)pattern.c_str(),
        PCRE2_ZERO_TERMINATED,
        flags,
        &re::lasterror,
        &re::erroroffset,
        ccontext);
    if (re_code == NULL || re::lasterror != 100){
        return nullptr;
    }
    std::shared_ptr<re::RegexObjectW> p_c(new re::RegexObjectW(re_code, flags, pattern));
    return p_c;
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::subn(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return std::make_tuple("", -1);
    }
    return p_re->subn(repl, Str, count);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::subn(const  std::wstring & pattern, const std::wstring & repl, const  std::wstring & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return std::make_tuple(L"", -1);
    }
    return p_re->subn(repl, Str, count);
}
//------------------------------------------------------------------------------
std::string re::sub(const  std::string & pattern, const  std::string & repl, const std::string & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return "";
    }
    std::string r = p_re->sub(repl, Str, count);
    return r;
}
//------------------------------------------------------------------------------
 std::wstring re::sub(const std::wstring & pattern, const  std::wstring & repl, const std::wstring & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return L"";
    }
     std::wstring r=  p_re->sub(repl, Str, count);
    return r;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObject> re::search(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return nullptr;
    }
    return p_re->search(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObjectW> re::search(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return nullptr;
    }
    return p_re->search(Str);
}
//------------------------------------------------------------------------------
std::vector<std::string> re::split(const std::string & pattern, const std::string & Str,size_t maxsplit, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        std::vector<std::string> v;
        return v;
    }
    return p_re->split(Str);
}
//------------------------------------------------------------------------------
std::vector<std::string> re::findall(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        std::vector<std::string> v;
        return v;
    }
    return p_re->findall(Str);
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::findall(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        std::vector<std::wstring> v;
        return v;
    }
    return p_re->findall(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iter> re::finditer(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return nullptr;
    }
    return p_re->finditer(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iterW> re::finditer(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    auto p_re = re::compile(pattern, flags);
    if (!p_re){
        return nullptr;
    }
    return p_re->finditer(Str);
}
//==============================================================================
//------------------------------------------------------------------------------
re::MatchObject::MatchObject(std::shared_ptr<RegexObject> re, std::string Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos):
m_re(re),
m_str(Str), 
m_pos(pos),
m_endpos(endpos),
m_lastindex(lastindex)
{
    for (size_t i = 0; i < lastindex * 2; i++){
        this->m_groups.push_back(ovector[i]);
    }
}
//------------------------------------------------------------------------------
re::MatchObjectW::MatchObjectW(std::shared_ptr<RegexObjectW> re, std::wstring Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos):
m_re(re),
m_str(Str),
m_pos(pos),
m_endpos(endpos),
m_lastindex(lastindex)
{
    for (size_t i = 0; i < lastindex * 2; i++){
        this->m_groups.push_back(ovector[i]);
    }
}
//------------------------------------------------------------------------------
size_t re::MatchObject::pos(){
    return m_pos;
}
//------------------------------------------------------------------------------
size_t re::MatchObjectW::pos(){
    return m_pos;
}
//------------------------------------------------------------------------------
size_t re::MatchObject::endpos(){
    return m_endpos;
}
//------------------------------------------------------------------------------
size_t re::MatchObjectW::endpos(){
    return m_endpos;
}
//------------------------------------------------------------------------------
size_t re::MatchObject::lastindex(){
    return m_lastindex;
}
//------------------------------------------------------------------------------
size_t re::MatchObjectW::lastindex(){
    return m_lastindex;
}
//------------------------------------------------------------------------------
 std::string re::MatchObject::group(){
    return this->group(0);
}
//------------------------------------------------------------------------------
std::wstring re::MatchObjectW::group(){
    return this->group(0);
}
//------------------------------------------------------------------------------
std::string re::MatchObject::group(size_t i){
    if (i > m_lastindex){
        return "";
    }
    return this->m_str.substr(
        this->m_groups[2 * i],
        this->m_groups[2 * i + 1] - this->m_groups[2 * i]);
}
//------------------------------------------------------------------------------
 std::wstring re::MatchObjectW::group(size_t i){
    if (i > m_lastindex){
        return L"";
    }
    return this->m_str.substr(
        this->m_groups[2 * i],
        this->m_groups[2 * i + 1] - this->m_groups[2 * i]);
}
//------------------------------------------------------------------------------
std::string re::MatchObject::group(std::string name){
    int i = pcre2_substring_number_from_name_8((pcre2_code_8 *)m_re->m_re, (PCRE2_SPTR8)name.c_str());
    if (i >= 0){
        return this->group(i);
    }
    else {
        return "";
    }
}
//------------------------------------------------------------------------------
std::wstring re::MatchObjectW::group(std::wstring name){
    int i = pcre2_substring_number_from_name((pcre2_code *)m_re->m_re, (PCRE2_SPTR)name.c_str());
    if (i >= 0){
        return this->group(i);
    }
    else {
        return L"";
    }
}
//------------------------------------------------------------------------------
std::vector<std::string> re::MatchObject::groups(){
    std::vector< std::string> v;
    for (size_t i = 1; i < m_lastindex; i++){
        v.push_back(this->group(i));
    }
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::MatchObjectW::groups(){
    std::vector< std::wstring> v;
    for (size_t i = 1; i < m_lastindex; i++){
        v.push_back(this->group(i));
    }
    return v;
}
//------------------------------------------------------------------------------
std::vector<int> re::MatchObject::span(size_t i){
    std::vector<int> v;
    if (i < m_lastindex){
        v.push_back((int)this->start(i));
        v.push_back((int)this->end(i));
        return v;
    }
    else{
        v.push_back(-1);
        v.push_back(-1);
        return v;
    }
}
//------------------------------------------------------------------------------
size_t re::MatchObject::start(size_t i){
    return this->m_groups[2 * i];
}
//------------------------------------------------------------------------------
size_t re::MatchObjectW::start(size_t i){
    return this->m_groups[2 * i];
}
//------------------------------------------------------------------------------
size_t re::MatchObject::end(size_t i){
    return this->m_groups[2 * i + 1];
}
//------------------------------------------------------------------------------
size_t re::MatchObjectW::end(size_t i){
    return this->m_groups[2 * i + 1];
}
//------------------------------------------------------------------------------
std::shared_ptr<re::RegexObject> re::MatchObject::re(){
    return m_re;
}
//------------------------------------------------------------------------------
std::shared_ptr<re::RegexObjectW> re::MatchObjectW::re(){
    return m_re;
}
//------------------------------------------------------------------------------
 std::string re::MatchObject::string(){
    return m_str;
}
 //------------------------------------------------------------------------------
 std::wstring re::MatchObjectW::string(){
     return m_str;
 }
//==============================================================================
re::RegexObject::RegexObject(pcre2_code_8 * re, size_t flags, std::string pattern):
m_flags(flags),
m_pattern(pattern),
m_re(re)
{
}
//------------------------------------------------------------------------------
re::RegexObjectW::RegexObjectW(pcre2_code * re, size_t flags, std::wstring pattern) :
m_flags(flags),
m_pattern(pattern),
m_re(re)
{
}
//------------------------------------------------------------------------------
re::RegexObject::~RegexObject(){
    pcre2_code_free_8((pcre2_code_8 *)this->m_re);
    m_re = nullptr;
}
//------------------------------------------------------------------------------
re::RegexObjectW::~RegexObjectW(){
    pcre2_code_free((pcre2_code *)this->m_re);
    m_re = nullptr;
}
//------------------------------------------------------------------------------
size_t re::RegexObject::flags(){
    return m_flags;
}
//------------------------------------------------------------------------------
size_t re::RegexObjectW::flags(){
    return m_flags;
}
//------------------------------------------------------------------------------
 std::string re::RegexObject::pattern(){
    return m_pattern;
}
//------------------------------------------------------------------------------
std::wstring re::RegexObjectW::pattern(){
    return m_pattern;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iter> re::RegexObject::finditer(const std::string & Str, size_t pos, int endpos){
     std::string Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    std::unique_ptr<re::iter> p_it(new re::iter(shared_from_this(), Str1));
    return p_it;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iterW> re::RegexObjectW::finditer(const std::wstring & Str, size_t pos, int endpos){
    std::wstring Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    std::unique_ptr<re::iterW> p_it(new re::iterW(shared_from_this(), Str1));
    return p_it;
}
//------------------------------------------------------------------------------
std::vector<std::string> re::RegexObject::split(const std::string & Str, size_t maxsplit){
    std::vector<std::string> v;
    if (this->m_pattern == ""){
        v.push_back(Str);
        return v;
    }
    auto I = this->finditer(Str);
    if (!I){
        v.push_back(Str);
        return v;
    }
    size_t j = 0;
    size_t Pos = 0;
    while (!I->AtEnd()){
        v.push_back(Str.substr(Pos, I->Get()->start(0) - Pos));
        for (size_t i = 1; i < I->Get()->lastindex(); i++){
            v.push_back(I->Get()->group(i));
        }
        Pos = I->Get()->end(0);
        if (maxsplit && ++j > maxsplit){
            break;
        }
        ++(*I.get());
    }
    v.push_back(Str.substr(Pos, Str.size() - Pos));
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::RegexObjectW::split(const std::wstring & Str, size_t maxsplit){
    std::vector<std::wstring> v;
    if (this->m_pattern == L""){
        v.push_back(Str);
        return v;
    }
    auto I = this->finditer(Str);
    if (!I){
        v.push_back(Str);
        return v;
    }
    size_t j = 0;
    size_t Pos = 0;
    while (!I->AtEnd()){
        v.push_back(Str.substr(Pos, I->Get()->start(0) - Pos));
        for (size_t i = 1; i < I->Get()->lastindex(); i++){
            v.push_back(I->Get()->group(i));
        }
        Pos = I->Get()->end(0);
        if (maxsplit && ++j > maxsplit){
            break;
        }
        ++(*I.get());
    }
    v.push_back(Str.substr(Pos, Str.size() - Pos));
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::string> re::RegexObject::findall(const std::string & Str, size_t pos, int endpos){
    std::vector<std::string> v;
    if (this->m_pattern == ""){
        return v;
    }
    pcre2_match_data_8 * match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    std::string Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    while (1 && this->m_pattern != ""){
        int lastindex = pcre2_match_8(
            m_re,
            (PCRE2_SPTR8)Str1.c_str(),
            Str1.size(), // * sizeof(char),   //TODO: Correct Size
            pos,
            0,
            match_data,
            NULL);
        if (lastindex > 0){
            PCRE2_SIZE * ovector = pcre2_get_ovector_pointer_8(match_data);
            v.push_back(Str.substr(ovector[0], ovector[1] - ovector[0]));
            pos = ovector[1];
        }
        else{
            break;
        }
    }
    pcre2_match_data_free_8(match_data);
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::RegexObjectW::findall(const std::wstring & Str, size_t pos, int endpos){
    std::vector<std::wstring> v;
    if (this->m_pattern == L""){
        return v;
    }
    pcre2_match_data * match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
     std::wstring Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    while (1 && this->m_pattern != L""){
        int lastindex = pcre2_match(
            m_re,
            (PCRE2_SPTR)Str1.c_str(),
            Str1.size(),
            pos,
            0,
            match_data,
            NULL);
        if (lastindex > 0){
            PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(match_data);
            v.push_back(Str.substr(ovector[0], ovector[1] - ovector[0]));
            pos = ovector[1];
        }
        else{
            break;
        }
    }
    pcre2_match_data_free(match_data);
    return v;
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::RegexObject::subn(const std::string & repl, const std::string & Str, size_t count){
    if (this->m_pattern == ""){
        return std::make_tuple("", -1);
    }
    pcre2_match_data_8 * match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    std::string Str1 = Str;
    size_t option = PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | PCRE2_SUBSTITUTE_EXTENDED;
    if (count == 0){
        option |= PCRE2_SUBSTITUTE_GLOBAL;
    }
    PCRE2_UCHAR8 * outputbuffer = nullptr;
    PCRE2_SIZE outlength = Str1.length() * 2;
    PCRE2_SIZE * p_outlength = &outlength;

    int c = 0;
    size_t i = 0;
    while(i <= count){
        outputbuffer = (PCRE2_UCHAR8 *)malloc(sizeof(PCRE2_UCHAR8 *) * outlength);
        int ret = pcre2_substitute_8(
            (pcre2_code_8 *)m_re,
            (PCRE2_SPTR8)Str1.c_str(),
            Str1.size(), // * sizeof(char),
            0,
            (int)option,
            match_data,
            NULL,
            (PCRE2_SPTR8)repl.c_str(),
            repl.size(),
            (PCRE2_UCHAR8 *)outputbuffer,
            p_outlength
            );
        if (ret == 0){
            break;
        }
        if (ret < 0){
            if (ret == PCRE2_ERROR_NOMEMORY){
                free(outputbuffer);
                outputbuffer = nullptr;
                continue;
            }
            else{
                re::lasterror = ret;
                return std::make_tuple("", -1);
            }
        }
        std::string r((char *)outputbuffer);
        r = r.substr(0, outlength);
        Str1 = r;
        free(outputbuffer);
        outputbuffer = nullptr;
        c += ret;
        i++;
    }
    Str1.append("\0");
    pcre2_match_data_free_8(match_data);
    return std::make_tuple(Str1,c);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::RegexObjectW::subn(const std::wstring & repl, const std::wstring & Str, size_t count){
    if (this->m_pattern == L""){
        return std::make_tuple(L"", -1);
    }
    pcre2_match_data * match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
     std::wstring Str1 = Str;
    size_t option = PCRE2_SUBSTITUTE_OVERFLOW_LENGTH | PCRE2_SUBSTITUTE_EXTENDED;
    if (count == 0){
        option |= PCRE2_SUBSTITUTE_GLOBAL;
    }
    PCRE2_UCHAR * outputbuffer = nullptr;
    PCRE2_SIZE outlength = Str1.length() * 2;
    PCRE2_SIZE * p_outlength = &outlength;

    int c = 0;
    size_t i = 0;
    while (i <= count){
        outputbuffer = (PCRE2_UCHAR *)malloc(sizeof(PCRE2_UCHAR *) * outlength);
        int ret = pcre2_substitute(
            (pcre2_code *)m_re,
            (PCRE2_SPTR)Str1.c_str(),
            Str1.size(), // * sizeof(wchar_t),
            0,
            (int)option,
            match_data,
            NULL,
            (PCRE2_SPTR)repl.c_str(),
            repl.size(),
            (PCRE2_UCHAR *)outputbuffer,
            p_outlength
            );
        if (ret == 0){
            break;
        }
        if (ret < 0){
            if (ret == PCRE2_ERROR_NOMEMORY){
                free(outputbuffer);
                outputbuffer = nullptr;
                continue;
            }
            else{
                re::lasterror = ret;
                return std::make_tuple(L"", -1);
            }
        }
         std::wstring r((wchar_t *)outputbuffer);
        r = r.substr(0, outlength);
        Str1 = r;
        free(outputbuffer);
        outputbuffer = nullptr;
        c += ret;
        i++;
    }
    Str1.append(L"\0");
    pcre2_match_data_free(match_data);
    return std::make_tuple(Str1, c);
}
//------------------------------------------------------------------------------
std::string re::RegexObject::sub(const std::string & repl, const std::string & Str, size_t count){
    std::string r = std::get<0>(this->subn(repl, Str, count));
    return r;
}
//------------------------------------------------------------------------------
std::wstring re::RegexObjectW::sub(const std::wstring & repl, const std::wstring & Str, size_t count){
    std::wstring r = std::get<0>(this->subn(repl, Str, count));
    return r;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObject> re::RegexObject::search(const std::string & Str, size_t pos, int endpos){
    if (!m_re){
        return nullptr;
    }
    if (this->m_pattern == ""){
        return nullptr;
    }
    pcre2_match_data_8 * match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    std::string Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    int lastindex = pcre2_match_8(
        m_re,
        (PCRE2_SPTR8)Str1.c_str(),
        Str1.size(),
        pos,
        0,
        match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer_8(match_data);
        std::unique_ptr<MatchObject> p_sm(new MatchObject(shared_from_this(), Str, ovector, lastindex, pos, endpos));
        pcre2_match_data_free_8(match_data);
        return p_sm;
    }
    else {
        pcre2_match_data_free_8(match_data);
        return nullptr;
    }
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObjectW> re::RegexObjectW::search(const std::wstring & Str, size_t pos, int endpos){
    if (!m_re){
        return nullptr;
    }
    if (this->m_pattern == L""){
        return nullptr;
    }
    pcre2_match_data * match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    std::wstring Str1 = Str;
    if (endpos > -1){
        Str1 = Str.substr(0, endpos);
    }
    endpos = (int)Str1.length();
    int lastindex = pcre2_match(
        m_re,
        (PCRE2_SPTR)Str1.c_str(),
        Str1.size(),
        pos,
        0,
        match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(match_data);
        std::unique_ptr<MatchObjectW> p_sm(new MatchObjectW(shared_from_this(), Str, ovector, lastindex, pos, endpos));
        pcre2_match_data_free(match_data);
        return p_sm;
    }
    else {
        pcre2_match_data_free(match_data);
        return nullptr;
    }
}
//==============================================================================
re::iter::iter(std::shared_ptr<re::RegexObject> regexobj, std::string Str):
m_regexobj(regexobj),
m_str(Str){
    m_matchobj = m_regexobj->search(m_str, 0);
    if (m_matchobj){
        m_pos = (int)m_matchobj->end(0);
    }
}
//------------------------------------------------------------------------------
re::iterW::iterW(std::shared_ptr<re::RegexObjectW> regexobj, std::wstring Str):
m_regexobj(regexobj),
m_str(Str){
    m_matchobj = m_regexobj->search(m_str, 0);
    if (m_matchobj){
        m_pos = (int)m_matchobj->end(0);
    }
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObject> re::iter::Get(){
    if (this->m_matchobj){
        std::unique_ptr<re::MatchObject> p_m(new MatchObject(*this->m_matchobj.get()));
        return p_m;
    }
    else{
        return nullptr;
    }
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObjectW> re::iterW::Get(){
    if (this->m_matchobj){
        std::unique_ptr<re::MatchObjectW> p_m(new MatchObjectW(*this->m_matchobj.get()));
        return p_m;
    }
    else{
        return nullptr;
    }
}
//------------------------------------------------------------------------------
re::iter & re::iter::operator ++() {
    // actual increment takes place here
     m_matchobj = m_regexobj->search(m_str, m_pos);
     if (m_matchobj){
         m_pos = (int)m_matchobj->end(0);
    }
    return * this;
}
//------------------------------------------------------------------------------
re::iterW & re::iterW::operator ++() {
    // actual increment takes place here
    m_matchobj = m_regexobj->search(m_str, m_pos);
    if (m_matchobj){
        m_pos = (int)m_matchobj->end(0);
    }
    return * this;
}
//------------------------------------------------------------------------------
re::iter & re::iter::operator ++(int) {
    // we make ++X and X++ the same...
    return operator++(); // pre-increment
}
//------------------------------------------------------------------------------
re::iterW & re::iterW::operator ++(int) {
    // we make ++X and X++ the same...
    return operator++(); // pre-increment
}
//------------------------------------------------------------------------------
bool re::iter::AtEnd(){
    if (m_matchobj){
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
bool re::iterW::AtEnd(){
    if (m_matchobj){
        return false;
    }
    return true;
}
