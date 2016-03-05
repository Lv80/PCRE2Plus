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

#define PCRE2_STATIC 1
#include <pcre2.h>
#include <string>
#include <vector>
#include <memory>
#include <tuple>


namespace PCRE2Plus{
    //==========================================================================
    class re{
    protected:
        static int lasterror;
        static size_t erroroffset;
    public:
        class RegexObject;
        class MatchObject;
        class iter;
        class RegexObjectW;
        class MatchObjectW;
        class iterW;
        static std::shared_ptr<re::RegexObject> compile(const std::string & pattern, int flags = 0);
        static std::shared_ptr<re::RegexObjectW> compile(const std::wstring & pattern, int flags = 0);
        static std::unique_ptr<re::MatchObject> search(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::unique_ptr<re::MatchObjectW> search(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::vector<std::string> split(const std::string & pattern, const std::string & Str, size_t maxsplit = 0, int flags = 0);
        static std::vector<std::wstring> split(const std::wstring & pattern, const std::wstring & Str, size_t maxsplit = 0, int flags = 0);
        static std::vector<std::string> findall(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::vector<std::wstring> findall(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::unique_ptr<re::iter> finditer(const std::string & pattern, const std::string & Str, int flags = 0);
        static std::unique_ptr<re::iterW> finditer(const std::wstring & pattern, const std::wstring & Str, int flags = 0);
        static std::string sub(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0);
        static std::wstring sub(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::string, size_t> subn(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0);
        static std::tuple<std::wstring, size_t> subn(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0);
        static std::string escape(const std::string & unquoted);
        static std::wstring escape(const std::wstring & unquoted);
        static int getlasterror();
        static int geterroroffset();
        static std::string getlasterrorstr();
    public:
        static const size_t DOTALL = PCRE2_DOTALL;
        static const size_t S = DOTALL;
        static const size_t MULTLINE = PCRE2_MULTILINE;
        static const size_t M = MULTLINE;
        static const size_t IGNORECASE = PCRE2_CASELESS;
        static const size_t I = IGNORECASE;
        static const size_t VERBOSE = PCRE2_EXTENDED;
        static const size_t X = VERBOSE;
        static const size_t U = PCRE2_UCP | PCRE2_UTF;

        static const size_t LOCALE = 0x10000000u;  /* Not yet used by pcre2 */
        static const size_t L = LOCALE;


        //======================================================================
        class MatchObject{
        public:
            MatchObject(std::shared_ptr<RegexObject> re, std::string Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos);
            std::string group(size_t i);
            std::string group();
            std::string group(std::string name);
            size_t pos();
            size_t endpos();
            size_t start(size_t i);
            size_t end(size_t i);
            std::vector<std::string> groups();
            std::string string();
            size_t lastindex();
            std::vector<int> span(size_t i);
            std::shared_ptr<RegexObject> re();
        protected:
            std::shared_ptr<RegexObject> m_re;
            std::string m_str;
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
            friend class re::MatchObject;
        public:
            RegexObject(pcre2_code_8 * re, size_t flags, std::string pattern);
            ~RegexObject();
            size_t flags();
            std::string pattern();
            std::unique_ptr<re::MatchObject> search(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::vector<std::string> split(const std::string & Str, size_t maxsplit = 0);
            std::vector<std::string> findall(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::iter> finditer(const std::string & Str, size_t pos = 0, int endpos = -1);
            std::string sub(const std::string & repl, const std::string & Str, size_t count = 0);
            std::tuple<std::string, size_t> subn(const std::string & repl, const std::string & Str, size_t count = 0);

        };
        //======================================================================
        class iter{
        protected:
            std::shared_ptr<RegexObject> m_regexobj;
            int m_pos;
            std::string m_str;
            std::unique_ptr<MatchObject> m_matchobj;
        public:
            iter(std::shared_ptr<RegexObject> regexobj, std::string Str);
            std::unique_ptr<MatchObject> Get();
            bool AtEnd();
            void next();
            iter & operator ++();
            iter & operator ++(int);
        };

        //======================================================================

        class MatchObjectW {
        public:
            MatchObjectW(std::shared_ptr<RegexObjectW> re, std::wstring Str, PCRE2_SIZE * ovector, size_t lastindex, size_t pos, size_t endpos);
            std::wstring group(size_t i);
            std::wstring group();
            std::wstring group(std::wstring name);
            size_t pos();
            size_t endpos();
            size_t start(size_t i);
            size_t end(size_t i);
            std::vector<std::wstring> groups();
            std::wstring string();
            size_t lastindex();
            std::vector<int> span(size_t i);
            std::shared_ptr<RegexObjectW> re();
        protected:
            std::shared_ptr<RegexObjectW> m_re;
            std::wstring m_str;
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
            friend class re::MatchObjectW;
        public:
            RegexObjectW(pcre2_code * re, size_t flags, std::wstring pattern);
            ~RegexObjectW();
            size_t flags();
            std::wstring pattern();
            std::unique_ptr<re::MatchObjectW> search(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::vector<std::wstring> split(const std::wstring & Str, size_t maxsplit = 0);
            std::vector< std::wstring> findall(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::unique_ptr<re::iterW> finditer(const  std::wstring & Str, size_t pos = 0, int endpos = -1);
            std::wstring sub(const  std::wstring & repl, const  std::wstring & Str, size_t count = 0);
            std::tuple<std::wstring, size_t> subn(const std::wstring & repl, const std::wstring & Str, size_t count = 0);
        };
        //======================================================================
        class iterW{
        protected:
            std::shared_ptr<RegexObjectW> m_regexobj;
            int m_pos;
            std::wstring m_str;
            std::unique_ptr<MatchObjectW> m_matchobj;

        public:
            iterW(std::shared_ptr<RegexObjectW> regexobj, std::wstring Str);
            std::unique_ptr<MatchObjectW> Get();
            bool AtEnd();
            void next();
            iterW & operator ++();
            iterW & operator ++(int);
        };
    };
};