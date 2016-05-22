/*
Copyright (c) 2016, Boying Xu All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this 
list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.

Neither the name of the copyright holder nor the names of its contributors may 
be used to endorse or promote products derived from this software without 
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUB STITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
bool re::usecache = false;
std::map<std::pair<std::string, int>, std::shared_ptr<re::RegexObject>> re::Cache;
std::map<std::pair<std::wstring, int>, std::shared_ptr<re::RegexObjectW>> re::CacheW;
pcre2_compile_context_8 * re::ccontext_8 = re::CreateCContext_8();
pcre2_compile_context * re::ccontext = re::CreateCContext();

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
int re::getcachesize(){
    return re::Cache.size() + re::CacheW.size();
}
//------------------------------------------------------------------------------
void re::purgecache(){
    re::Cache.clear();
    re::CacheW.clear();
}
//------------------------------------------------------------------------------
std::shared_ptr<re::RegexObject> re::compile(const std::string & pattern, int flags){
    flags = flags | PCRE2_DUPNAMES;
    pcre2_code_8 * re_code = NULL;
    pcre2_compile_context_8 * ccontext_8 = re::ccontext_8;
    if (flags & re::LOCALE){
        auto tables = pcre2_maketables_8(NULL);
        ccontext_8 = pcre2_compile_context_copy_8(re::ccontext_8);
        pcre2_set_character_tables_8(ccontext_8, tables);
        flags &= ~re::LOCALE;
    }
    re_code = pcre2_compile_8(
        (PCRE2_SPTR8)pattern.c_str(),
        PCRE2_ZERO_TERMINATED,
        flags,
        &re::lasterror,
        &re::erroroffset,
        ccontext_8);
    if (ccontext_8 != re::ccontext_8){
        pcre2_compile_context_free_8(ccontext_8);
    }
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
    pcre2_compile_context * ccontext = re::ccontext;
    if (flags & re::LOCALE){
        auto tables = pcre2_maketables(NULL);
        ccontext = pcre2_compile_context_copy(re::ccontext);
        pcre2_set_character_tables(ccontext, tables);
        flags &= ~re::LOCALE;
    }
    re_code = pcre2_compile(
        (PCRE2_SPTR)pattern.c_str(),
        PCRE2_ZERO_TERMINATED,
        flags,
        &re::lasterror,
        &re::erroroffset,
        ccontext);
    if (ccontext != re::ccontext){
        pcre2_compile_context_free(ccontext);
    }
    if (re_code == NULL || re::lasterror != 100){
        return nullptr;
    }
    std::shared_ptr<re::RegexObjectW> p_c(new re::RegexObjectW(re_code, flags, pattern));
    return p_c;
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::subn(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache) {
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return std::make_tuple("", -1);
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return std::make_tuple("", -1);
        }
    }
    return p_re->subn(repl, Str, count);
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::subn(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache) {
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return std::make_tuple("", -1);
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return std::make_tuple("", -1);
        }
    }
    return p_re->subn(userfun, Str, count);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::subn(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache) {
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return std::make_tuple(L"", -1);
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return std::make_tuple(L"", -1);
        }
    }
    return p_re->subn(userfun, Str, count);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::subn(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache) {
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return std::make_tuple(L"", -1);
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return std::make_tuple(L"", -1);
        }
    }
    return p_re->subn(repl, Str, count);
}
//------------------------------------------------------------------------------
std::string re::sub(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count, int flags) {
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return "";
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return "";
        }
    }
    return p_re->sub(userfun, Str, count);
}
//------------------------------------------------------------------------------
std::wstring re::sub(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count, int flags) {
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return L"";
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return L"";
        }
    }
    return p_re->sub(userfun, Str, count);
}
//------------------------------------------------------------------------------
std::string re::sub(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count, int flags) {
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return "";
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return "";
        }
    }
    return p_re->sub(repl, Str, count);
}
//------------------------------------------------------------------------------
 std::wstring re::sub(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count, int flags) {
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return L"";
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return L"";
        }
    }
    return p_re->sub(repl, Str, count);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObject> re::search(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return nullptr;
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return nullptr;
        }
    }
    return p_re->search(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObjectW> re::search(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return nullptr;
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return nullptr;
        }
    }
    return p_re->search(Str);
}
//------------------------------------------------------------------------------
std::vector<std::string> re::split(const std::string & pattern, const std::string & Str,size_t maxsplit, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                std::vector<std::string> v;
                return v;
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            std::vector<std::string> v;
            return v;
        }
    }
    return p_re->split(Str);
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::split(const std::wstring & pattern, const std::wstring & Str, size_t maxsplit, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                std::vector<std::wstring> v;
                return v;
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            std::vector<std::wstring> v;
            return v;
        }
    }
    return p_re->split(Str);
}
//------------------------------------------------------------------------------
std::vector<std::string> re::findall(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                std::vector<std::string> v;
                return v;
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            std::vector<std::string> v;
            return v;
        }
    }
    return p_re->findall(Str);
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::findall(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                std::vector<std::wstring> v;
                return v;
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            std::vector<std::wstring> v;
            return v;
        }
    }
    return p_re->findall(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iter> re::finditer(const std::string & pattern, const std::string & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObject> p_re;
    if (re::usecache){
        if (re::Cache.find(std::make_pair(pattern, flags)) != re::Cache.end()){
            p_re = re::Cache[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return nullptr;
            }
            re::Cache[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return nullptr;
        }
    }
    return p_re->finditer(Str);
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iterW> re::finditer(const std::wstring & pattern, const std::wstring & Str, int flags){
    flags = flags | PCRE2_DUPNAMES;
    std::shared_ptr <re::RegexObjectW> p_re;
    if (re::usecache){
        if (re::CacheW.find(std::make_pair(pattern, flags)) != re::CacheW.end()){
            p_re = re::CacheW[std::make_pair(pattern, flags)];
        }
        else {
            p_re = re::compile(pattern, flags);
            if (!p_re){
                return nullptr;
            }
            re::CacheW[std::make_pair(pattern, flags)] = p_re;
        }
    }
    else{
        p_re = re::compile(pattern, flags);
        if (!p_re){
            return nullptr;
        }
    }
    return p_re->finditer(Str);
}
//==============================================================================
//------------------------------------------------------------------------------
re::MatchObject::MatchObject(std::shared_ptr<RegexObject> re, const std::string & Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos):
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
re::MatchObjectW::MatchObjectW(std::shared_ptr<RegexObjectW> re, const std::wstring & Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos):
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
m_re(re),
m_match_data(nullptr)
{

}
//------------------------------------------------------------------------------
re::RegexObjectW::RegexObjectW(pcre2_code * re, size_t flags, std::wstring pattern) :
m_flags(flags),
m_pattern(pattern),
m_re(re),
m_match_data(nullptr)
{
}
//------------------------------------------------------------------------------
re::RegexObject::~RegexObject(){
    pcre2_match_data_free_8(m_match_data);
    pcre2_code_free_8((pcre2_code_8 *)this->m_re);
    m_re = nullptr;
}
//------------------------------------------------------------------------------
re::RegexObjectW::~RegexObjectW(){
    pcre2_match_data_free(m_match_data);
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
    std::unique_ptr<re::iter> p_it(new re::iter(shared_from_this(), Str, endpos));
    return p_it;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::iterW> re::RegexObjectW::finditer(const std::wstring & Str, size_t pos, int endpos){
    std::unique_ptr<re::iterW> p_it(new re::iterW(shared_from_this(), Str, endpos));
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
    v.push_back(Str.substr(Pos, Str.length() - Pos));
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
    v.push_back(Str.substr(Pos, Str.length() - Pos));
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::string> re::RegexObject::findall(const std::string & Str, size_t pos, int endpos){
    std::vector<std::string> v;
    if (this->m_pattern == ""){
        return v;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    }
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    if (this->m_pattern == ""){
        return v;
    }
    while (1){
        int lastindex = pcre2_match_8(
            m_re,
            (PCRE2_SPTR8)Str.c_str(),
            strlength,
            pos,
            0,
            m_match_data,
            NULL);
        if (lastindex > 0){
            PCRE2_SIZE * ovector = pcre2_get_ovector_pointer_8(m_match_data);
            v.push_back(Str.substr(ovector[0], ovector[1] - ovector[0]));
            pos = ovector[1];
        }
        else{
            break;
        }
    }
    return v;
}
//------------------------------------------------------------------------------
std::vector<std::wstring> re::RegexObjectW::findall(const std::wstring & Str, size_t pos, int endpos){
    std::vector<std::wstring> v;
    if (this->m_pattern == L""){
        return v;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    }
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    if (this->m_pattern == L""){
        return v;
    }
    while (1){
        int lastindex = pcre2_match(
            m_re,
            (PCRE2_SPTR)Str.c_str(),
            strlength,
            pos,
            0,
            m_match_data,
            NULL);
        if (lastindex > 0){
            PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(m_match_data);
            v.push_back(Str.substr(ovector[0], ovector[1] - ovector[0]));
            pos = ovector[1];
        }
        else{
            break;
        }
    }
    return v;
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::RegexObject::subn(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count){
    std::string Str1(Str);
    int Offset = 0;
    size_t Count = 0;
    for (auto it = this->finditer(Str); !it->AtEnd(); ++*it.get()){
        Count++;
        auto S = it->Get()->m_groups[0];
        auto E = it->Get()->m_groups[1];
        std::string r = userfun(it->Get());
        Str1.replace(S + Offset, E - S, r);
        Offset += r.length() - (E - S);
    }
    return std::make_tuple(Str1, Count);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::RegexObjectW::subn(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count){
    std::wstring Str1(Str);
    int Offset = 0;
    size_t Count = 0;
    for (auto it = this->finditer(Str); !it->AtEnd(); ++*it.get()){
        Count++;
        auto S = it->Get()->m_groups[0];
        auto E = it->Get()->m_groups[1];
        std::wstring r = userfun(it->Get());
        Str1.replace(S + Offset, E - S, r);
        Offset += r.length() - (E - S);
    }
    return std::make_tuple(Str1, Count);
}
//------------------------------------------------------------------------------
std::tuple<std::string, size_t> re::RegexObject::subn(const std::string & repl, const std::string & Str, size_t count){
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    }
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
            Str1.length(),
            0,
            (int)option,
            m_match_data,
            NULL,
            (PCRE2_SPTR8)repl.c_str(),
            repl.length(),
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
                re::lasterror = ret;  //TODO: raise error later
                return std::make_tuple(Str, 0);
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
    return std::make_tuple(Str1,c);
}
//------------------------------------------------------------------------------
std::tuple<std::wstring, size_t> re::RegexObjectW::subn(const std::wstring & repl, const std::wstring & Str, size_t count){
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    }
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
            Str1.length(),
            0,
            (int)option,
            m_match_data,
            NULL,
            (PCRE2_SPTR)repl.c_str(),
            repl.length(),
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
    return std::make_tuple(Str1, c);
}
//------------------------------------------------------------------------------
std::string re::RegexObject::sub(const std::string & repl, const std::string & Str, size_t count){
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    }
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
    while (i <= count){
        outputbuffer = (PCRE2_UCHAR8 *)malloc(sizeof(PCRE2_UCHAR8 *) * outlength);
        int ret = pcre2_substitute_8(
            (pcre2_code_8 *)m_re,
            (PCRE2_SPTR8)Str1.c_str(),
            Str1.length(),
            0,
            (int)option,
            m_match_data,
            NULL,
            (PCRE2_SPTR8)repl.c_str(),
            repl.length(),
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
                return "";
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
    return Str1;
}
//------------------------------------------------------------------------------
std::string re::RegexObject::sub(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count){
    std::string Str1(Str);
    int Offset = 0;
    for (auto it = this->finditer(Str); !it->AtEnd(); ++*it.get()){
        auto S = it->Get()->m_groups[0];
        auto E = it->Get()->m_groups[1];
        std::string r = userfun(it->Get());
        Str1.replace(S + Offset, E - S, r);
        Offset += r.length() - (E - S);
    }
    return Str1;
}
//------------------------------------------------------------------------------
std::wstring re::RegexObjectW::sub(const std::wstring & repl, const std::wstring & Str, size_t count){
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    }
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
            Str1.length(),
            0,
            (int)option,
            m_match_data,
            NULL,
            (PCRE2_SPTR)repl.c_str(),
            repl.length(),
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
                return L"";
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
    return Str1;
}
//------------------------------------------------------------------------------
std::wstring re::RegexObjectW::sub(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count){
    std::wstring Str1(Str);
    int Offset = 0;
    for (auto it = this->finditer(Str); !it->AtEnd(); ++*it.get()){

        auto S = it->Get()->m_groups[0];
        auto E = it->Get()->m_groups[1];
        std::wstring r = userfun(it->Get());
        Str1.replace(S + Offset, E - S, r);
        Offset += r.length() - (E - S);
    }
    return Str1;
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObject> re::RegexObject::search(const std::string & Str, size_t pos, int endpos){
    if (!m_re){
        return nullptr;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    }
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    int lastindex = pcre2_match_8(
        m_re,
        (PCRE2_SPTR8)Str.c_str(),
        strlength,
        pos,
        0,
        m_match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer_8(m_match_data);
        std::unique_ptr<MatchObject> p_sm(new MatchObject(shared_from_this(), Str, ovector, lastindex, pos, endpos));
        return p_sm;
    }
    else {
        return nullptr;
    }
}
//------------------------------------------------------------------------------
std::unique_ptr<re::MatchObjectW> re::RegexObjectW::search(const std::wstring & Str, size_t pos, int endpos){
    if (!m_re){
        return nullptr;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    }
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    int lastindex = pcre2_match(
        m_re,
        (PCRE2_SPTR)Str.c_str(),
        strlength,
        pos,
        0,
        m_match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(m_match_data);
        std::unique_ptr<MatchObjectW> p_sm(new MatchObjectW(shared_from_this(), Str, ovector, lastindex, pos, endpos));
        return p_sm;
    }
    else {
        return nullptr;
    }
}
//------------------------------------------------------------------------------
void re::RegexObject::search(std::unique_ptr<re::MatchObject> & M, const std::string & Str, size_t pos, int endpos){
    if (!m_re){
        return;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    }
    //pcre2_match_data_8 * match_data = pcre2_match_data_create_from_pattern_8(m_re, NULL);
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    int lastindex = pcre2_match_8(
        m_re,
        (PCRE2_SPTR8)Str.c_str(),
        strlength,
        pos,
        0,
        m_match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer_8(m_match_data);
        M->m_pos = pos;
        M->m_endpos = strlength;
        M->m_groups.clear();
        for (size_t i = 0; (int)i < lastindex * 2; i++){
            M->m_groups.push_back(ovector[i]);
        }
        return;
    }
    else {
        M = nullptr;
    }
}
//------------------------------------------------------------------------------
void re::RegexObjectW::search(std::unique_ptr<re::MatchObjectW> & M, const std::wstring & Str, size_t pos, int endpos){
    if (!m_re){
        return;
    }
    if (!m_match_data){
        m_match_data = pcre2_match_data_create_from_pattern(m_re, NULL);
    }
    size_t strlength = 0;
    if (endpos > -1){
        strlength = endpos;
    }
    else{
        strlength = Str.length();
    }
    int lastindex = pcre2_match(
        m_re,
        (PCRE2_SPTR)Str.c_str(),
        strlength,
        pos,
        0,
        m_match_data,
        NULL);
    if (lastindex > 0){
        PCRE2_SIZE * ovector = pcre2_get_ovector_pointer(m_match_data);
        M->m_pos = pos;
        M->m_endpos = strlength;
        M->m_groups.clear();
        for (size_t i = 0; (int)i < lastindex * 2; i++){
            M->m_groups.push_back(ovector[i]);
        }
        return;
    }
    else {
        M = nullptr;
        return;
    }
}
//==============================================================================
re::iter::iter(std::shared_ptr<re::RegexObject> regexobj, const std::string & Str, int endpos):
m_regexobj(regexobj),
m_str(Str),
m_endpos(endpos)
{
    m_matchobj = m_regexobj->search(m_str, 0, endpos);
    if (m_matchobj){
        if (m_pos == (int)m_matchobj->end(0)){
            m_pos++;
        }
        else{
            m_pos = (int)m_matchobj->end(0);
        }
    }
}
//------------------------------------------------------------------------------
re::iterW::iterW(std::shared_ptr<re::RegexObjectW> regexobj, const std::wstring & Str, int endpos) :
m_regexobj(regexobj),
m_str(Str),
m_endpos(endpos)
{
    m_matchobj = m_regexobj->search(m_str, 0);
    if (m_matchobj){
        if (m_pos == (int)m_matchobj->end(0)){
            m_pos++;
        }
        else{
            m_pos = (int)m_matchobj->end(0);
        }
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
    m_regexobj->search(m_matchobj, m_str, m_pos, m_endpos);
     if (m_matchobj){
         if (m_pos == (int)m_matchobj->end(0)){
             m_pos++;
         }
         else{
             m_pos = (int)m_matchobj->end(0);
         }
    }
    return * this;
}
//------------------------------------------------------------------------------
re::iterW & re::iterW::operator ++() {
     m_regexobj->search(m_matchobj, m_str, m_pos, m_endpos);
    if (m_matchobj){
        if (m_pos == (int)m_matchobj->end(0)){
            m_pos++;
        }
        else{
            m_pos = (int)m_matchobj->end(0);
        }
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
