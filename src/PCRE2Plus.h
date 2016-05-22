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

#if ! defined(MARKUP_SIZEOFWCHAR)
#if __SIZEOF_WCHAR_T__ == 4 || __WCHAR_MAX__ > 0x10000
#define MARKUP_SIZEOFWCHAR 4
#else
#define MARKUP_SIZEOFWCHAR 2
#endif
#endif

#if MARKUP_SIZEOFWCHAR == 4
#define PCRE2_CODE_UNIT_WIDTH 32
#else
#define PCRE2_CODE_UNIT_WIDTH 16
#endif

#if defined(_MSC_VER)
#if defined (_DEBUG)
#pragma comment(lib, "libpcre2-8d")
#if MARKUP_SIZEOFWCHAR == 4
#pragma comment(lib, "libpcre2-32d")
#else
#pragma comment(lib, "libpcre2-16d")
#endif
#else
#pragma comment(lib, "libpcre2-8")
#if MARKUP_SIZEOFWCHAR == 4
#define PCRE2_CODE_UNIT_WIDTH 32
#pragma comment(lib, "libpcre2-32")
#else
#pragma comment(lib, "libpcre2-16")
#endif
#endif
#endif





#define PCRE2_STATIC 1
#include <pcre2.h>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <map>
#include <functional>
namespace PCRE2Plus{
    //==========================================================================
    class re {
    protected:
        inline static pcre2_compile_context_8 * CreateCContext_8(){
            pcre2_compile_context_8 * t = pcre2_compile_context_create_8(NULL);
            pcre2_set_newline_8(t, PCRE2_NEWLINE_ANY);
            return t;
        }
        inline static pcre2_compile_context * CreateCContext(){
            pcre2_compile_context * t = pcre2_compile_context_create(NULL);
            pcre2_set_newline(t, PCRE2_NEWLINE_ANY);
            return t;
        }
        static int lasterror;
        static size_t erroroffset;
        static pcre2_compile_context_8 * ccontext_8;
        static pcre2_compile_context * ccontext;
    public:
        static bool usecache;
        class RegexObject;
        class MatchObject;
        class iter;
        class RegexObjectW;
        class MatchObjectW;
        class iterW;
    protected:
        static std::map<std::pair<std::string, int>, std::shared_ptr<re::RegexObject>> Cache;
        static std::map<std::pair<std::wstring, int>, std::shared_ptr<re::RegexObjectW>> CacheW;
    public:
        static int getcachesize();
        static void purgecache();
        static std::shared_ptr<re::RegexObject> compile(const std::string & pattern, int flags = 0);
        static std::shared_ptr<re::RegexObjectW> compile(const std::wstring & pattern, int flags = 0);
        static std::unique_ptr<re::MatchObject> search(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::unique_ptr<re::MatchObject> search(const std::string & pattern, const std::string && Str, int flags = 0) = delete;
        static std::unique_ptr<re::MatchObjectW> search(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::unique_ptr<re::MatchObjectW> search(const std::wstring & pattern, const std::wstring && Str, int flags = 0) = delete;
        static std::vector<std::string> split(const std::string & pattern, const std::string & Str, size_t maxsplit = 0, int flags = 0);
        static std::vector<std::string> split(const std::string & pattern, const std::string && Str, size_t maxsplit = 0, int flags = 0) = delete;
        static std::vector<std::wstring> split(const std::wstring & pattern, const std::wstring & Str, size_t maxsplit = 0, int flags = 0);
        static std::vector<std::wstring> split(const std::wstring & pattern, const std::wstring && Str, size_t maxsplit = 0, int flags = 0) = delete;
        static std::vector<std::string> findall(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::vector<std::string> findall(const std::string & pattern, const std::string && Str, int flags = 0) = delete;
        static std::vector<std::wstring> findall(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::vector<std::wstring> findall(const std::wstring & pattern, const std::wstring && Str, int flags = 0) = delete;
        static std::unique_ptr<re::iter> finditer(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::unique_ptr<re::iter> finditer(const std::string & pattern, const std::string && Str, int flags = 0) = delete;
        static std::unique_ptr<re::iterW> finditer(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::unique_ptr<re::iterW> finditer(const std::wstring & pattern, const std::wstring && Str, int flags = 0) = delete;
        static std::string sub(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0);
        static std::string sub(const std::string & pattern, const std::string & repl, const std::string && Str, size_t count = 0, int flags = 0) = delete;
        static std::wstring sub(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::wstring sub(const std::wstring & pattern, const std::wstring & repl, const std::wstring && Str, size_t count = 0, int flags = 0) = delete;
        static std::string sub(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count = 0, int flags = 0);
        static std::string sub(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string && Str, size_t count = 0, int flags = 0) = delete;
        static std::wstring sub(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::wstring sub(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring && Str, size_t count = 0, int flags = 0) = delete;
        static std::tuple<std::string, size_t> subn(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::string, size_t> subn(const std::string & pattern, const std::string & repl, const std::string && Str, size_t count = 0, int flags = 0) = delete;
        static std::tuple<std::wstring, size_t> subn(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::wstring, size_t> subn(const std::wstring & pattern, const std::wstring & repl, const std::wstring && Str, size_t count = 0, int flags = 0) = delete;
        static std::tuple<std::string, size_t> subn(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::string, size_t> subn(const std::string & pattern, std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string && Str, size_t count = 0, int flags = 0) = delete;
        static std::tuple<std::wstring, size_t> subn(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::wstring, size_t> subn(const std::wstring & pattern, std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring && Str, size_t count = 0, int flags = 0) = delete;
        static std::string escape(const std::string & unquoted);
        static std::wstring escape(const std::wstring & unquoted);
        static int getlasterror();
        static int geterroroffset();
        static std::string getlasterrorstr();
    public:
        static const size_t DOTALL = PCRE2_DOTALL;
        static const size_t S = DOTALL;
        //we force using PCRE2_NEWLINE_ANY to match any chars, no matter how complier parameter is given.
        static const size_t MULTLINE = PCRE2_MULTILINE;
        static const size_t M = MULTLINE;
        static const size_t IGNORECASE = PCRE2_CASELESS;
        static const size_t I = IGNORECASE;
        static const size_t VERBOSE = PCRE2_EXTENDED;
        static const size_t X = VERBOSE;
        static const size_t U = PCRE2_UCP | PCRE2_UTF;

        static const size_t LOCALE = 0x10000000u;  /* TODO: Not yet used by pcre2 */
        static const size_t L = LOCALE;


        //======================================================================
        class MatchObject{
        public:
            MatchObject(std::shared_ptr<RegexObject> re, const std::string & Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos);
            std::string group(size_t i);
            std::string group();
            std::string group(std::string name);
            size_t pos();
            size_t endpos();
            size_t start(size_t i = 0);
            size_t end(size_t i = 0);
            std::vector<std::string> groups();
            std::string string();
            size_t lastindex();
            std::vector<int> span(size_t i);
            std::shared_ptr<RegexObject> re();
            friend class RegexObject;
        protected:
            std::shared_ptr<RegexObject> m_re;
            const std::string & m_str;
            size_t m_pos;
            size_t m_endpos;
            std::vector<size_t> m_groups;
            size_t m_lastindex;
        };
        //======================================================================
        class RegexObject : public std::enable_shared_from_this<RegexObject>{
        protected:
            size_t m_flags;
            std::string m_pattern;
            pcre2_code_8 * m_re;
            pcre2_match_data_8 * m_match_data;
            friend class re::MatchObject;
            friend class re::iter;
            void search(std::unique_ptr<re::MatchObject> & M, const std::string & Str, size_t pos, int endpos);
        public:
            RegexObject(pcre2_code_8 * re, size_t flags, std::string pattern);
            ~RegexObject();
            size_t flags();
            std::string pattern();
            std::unique_ptr<re::MatchObject> search(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::MatchObject> search(const std::string && Str, size_t pos = 0, int endpos = -1) = delete;
            std::vector<std::string> split(const std::string & Str, size_t maxsplit = 0);
            std::vector<std::string> split(const std::string && Str, size_t maxsplit = 0) = delete;
            std::vector<std::string> findall(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::vector<std::string> findall(const std::string && Str, size_t pos = 0, int endpos = -1) = delete;
            std::unique_ptr<re::iter> finditer(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::iter> finditer(const std::string && Str, size_t pos = 0, int endpos = -1) = delete;
            std::string sub(const std::string & repl, const std::string & Str, size_t count = 0);
            std::string sub(const std::string & repl, const std::string && Str, size_t count = 0) = delete;
            std::string sub(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count = 0);
            std::string sub(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string && Str, size_t count = 0) = delete;
            std::tuple<std::string, size_t> subn(const std::string & repl, const std::string & Str, size_t count = 0);
            std::tuple<std::string, size_t> subn(const std::string & repl, const std::string && Str, size_t count = 0) = delete;
            std::tuple<std::string, size_t> subn(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string & Str, size_t count = 0);
            std::tuple<std::string, size_t> subn(std::function<std::string(const std::unique_ptr<re::MatchObject> &)> userfun, const std::string && Str, size_t count = 0) = delete;

        };
        //======================================================================
        class iter{
        protected:
            std::shared_ptr<RegexObject> m_regexobj;
            int m_pos;
            int m_endpos;
            const std::string & m_str;
            std::unique_ptr<MatchObject> m_matchobj;
        public:
            iter(std::shared_ptr<RegexObject> regexobj, const std::string & Str, int endpos = -1);
            std::unique_ptr<MatchObject> Get();
            bool AtEnd();
            void next();
            iter & operator ++();
            iter & operator ++(int);
        };

        //======================================================================

        class MatchObjectW {
        public:
            MatchObjectW(std::shared_ptr<RegexObjectW> re, const std::wstring & Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos);
            std::wstring group(size_t i);
            std::wstring group();
            std::wstring group(std::wstring name);
            size_t pos();
            size_t endpos();
            size_t start(size_t i = 0);
            size_t end(size_t i = 0);
            std::vector<std::wstring> groups();
            std::wstring string();
            size_t lastindex();
            std::vector<int> span(size_t i);
            std::shared_ptr<RegexObjectW> re();
            friend class RegexObjectW;
        protected:
            std::shared_ptr<RegexObjectW> m_re;
            const std::wstring & m_str;
            size_t m_pos;
            size_t m_endpos;
            std::vector<size_t> m_groups;
            size_t m_lastindex;
        };
        //======================================================================
        class RegexObjectW : public std::enable_shared_from_this< RegexObjectW >{
        protected:
            size_t m_flags;
            std::wstring m_pattern;
            pcre2_code * m_re;
            pcre2_match_data * m_match_data;
            friend class re::MatchObjectW;
            friend class re::iterW;
            void search(std::unique_ptr<re::MatchObjectW> & M, const std::wstring & Str, size_t pos, int endpos);
        public:
            RegexObjectW(pcre2_code * re, size_t flags, std::wstring pattern);
            ~RegexObjectW();
            size_t flags();
            std::wstring pattern();
            std::unique_ptr<re::MatchObjectW> search(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::MatchObjectW> search(const  std::wstring && Str, size_t pos = 0, int endpos = -1) = delete;
            std::vector<std::wstring> split(const std::wstring & Str, size_t maxsplit = 0);
            std::vector<std::wstring> split(const std::wstring && Str, size_t maxsplit = 0) = delete;
            std::vector< std::wstring> findall(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::vector< std::wstring> findall(const  std::wstring && Str, size_t pos = 0, int endpos = -1) = delete;
            std::unique_ptr<re::iterW> finditer(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::iterW> finditer(const  std::wstring && Str, size_t pos = 0, int endpos = -1) = delete;
            std::wstring sub(const  std::wstring & repl, const  std::wstring & Str, size_t count = 0);
            std::wstring sub(const  std::wstring & repl, const  std::wstring && Str, size_t count = 0) = delete;
            std::wstring sub(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count = 0);
            std::wstring sub(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring && Str, size_t count = 0) = delete;
            std::tuple<std::wstring, size_t> subn(const std::wstring & repl, const std::wstring & Str, size_t count = 0);
            std::tuple<std::wstring, size_t> subn(const std::wstring & repl, const std::wstring && Str, size_t count = 0) = delete;
            std::tuple<std::wstring, size_t> subn(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring & Str, size_t count = 0);
            std::tuple<std::wstring, size_t> subn(std::function<std::wstring(const std::unique_ptr<re::MatchObjectW> &)> userfun, const std::wstring && Str, size_t count = 0) = delete;
        };
        //======================================================================
        class iterW{
        protected:
            std::shared_ptr<RegexObjectW> m_regexobj;
            int m_pos;
            std::wstring m_str;
            std::unique_ptr<MatchObjectW> m_matchobj;
            int m_endpos;
        public:
            iterW(std::shared_ptr<RegexObjectW> regexobj, const std::wstring & Str, int endpos = -1);
            std::unique_ptr<MatchObjectW> Get();
            bool AtEnd();
            void next();
            iterW & operator ++();
            iterW & operator ++(int);
        };
    };
};