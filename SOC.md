#NOTE:
Presume following code applied

	using namespace PCRE2Plus


# 7.2. re — Regular expression operations
*supported by pcre2*

*Note*
	
	In C++11 raw string is written in following way:
	R"()" (c++) = r"" (python)
	So if you write r"(\w+)" in python, you need to add extra parentheses: R"((\w+))"


##7.2.1. Regular Expression Syntax
*supported by pcre2*

##7.2.2. Module Contents

###re.compile(pattern, flags=0)
*Supported*

static std::shared_ptr<re::RegexObject> re::compile(const std::string & pattern, int flags = 0)

static std::shared_ptr<re::RegexObjectW> re::compile(const std::wstring & pattern, int flags = 0)

Error Handling: Yes

    auto prog = re::compile(pattern)
    auto result = prog.match(string)

    is equivalent to

    auto result = re::match(pattern, string)


####re.DEBUG

*Not supported*

####re.I
####re.IGNORECASE

*Supported*

re::I

####re.L
####re.LOCALE

*Supported*

re::L

re::LOCALE

####re.M
####re.MULTILINE

*Supported*

re::M

re::MULTILINE

####re.S
####re.DOTALL

*Supported*

re:S

re::DOTALL

####re.U
####re.UNICODE

*Not sure or not necessary*

	UNICODE can not be used as it's a macro in PCRE 2 defination for other purpose
	use re::U only
	
	it seems only wide string can use this feature properly see UNICODE support section in PCRE2Pluse document

####re.X
####re.VERBOSE

*Supported*

re::X

re::VERBOSE

####re.search(pattern, string, flags=0)

*Supported*

static std::unique_ptr<re::MatchObject> re::search(const std::string & pattern, const std::string & Str, int flags = 0)

static std::unique_ptr<re::MatchObjectW> re::search(const std::wstring & pattern, const std::wstring & Str, int flags = 0)

Error Handling: pattern only

####re.match(pattern, string, flags=0)

*Not supported*

*Note*
	
	Please use re::search instead of re::match


####re.split(pattern, string, maxsplit=0, flags=0)

*Supported*

static std::vector<std::string> re::split(const std::string & pattern, const std::string & Str, size_t maxsplit = 0, int flags = 0)

static std::vector<std::wstring> re::split(const std::wstring & pattern, const std::wstring & Str, size_t maxsplit = 0, int flags = 0)

Error Handling: pattern only

####re.findall(pattern, string, flags=0)

*Supported*

static std::vector<std::string> re::findall(const std::string & pattern, const std::string & Str, int flags = 0)

static std::vector<std::wstring> re::findall(const std::wstring & pattern, const std::wstring & Str, int flags = 0)

Error Handling: pattern only

####re.finditer(pattern, string, flags=0)

*Supported*

static std::unique_ptr<re::iter> re::finditer(const std::string & pattern, const std::string & Str, int flags = 0)

static std::unique_ptr<re::iterW> re::finditer(const std::wstring & pattern, const std::wstring & Str, int flags = 0)

Error Handling: pattern only

Example:

```C++

	for (auto x = re::finditer(R"(\w+)", "abc def ghi"); !x->AtEnd(); ++*x.get()){
        std::cout << x->Get()->group(0) << std::endl;
    }

```
####re.sub(pattern, repl, string, count=0, flags=0)

*Supported*
	
static std::string re::sub(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0)

static std::wstring re::sub(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0)

Error Handling: pattern only

	NOTE: \x e.g(\1) as group reference in repl text is not support, use $x e.g $1 instead

####re.subn(pattern, repl, string, count=0, flags=0)

*Supported*

static std::tuple<std::string, size_t> re::subn(const std::string & pattern, const std::string & repl, const std::string & Str, size_t count = 0, int flags = 0)

static std::tuple<std::wstring, size_t> re::subn(const std::wstring & pattern, const std::wstring & repl, const std::wstring & Str, size_t count = 0, int flags = 0)

Error Handling: pattern only


####re.escape(string)

*Supported*

static std::string re::escape(const std::string & unquoted)

static std::wstring re::escape(const std::wstring & unquoted)

Error Handling: No

####re.purge()

*Not supported*

####exception re.error

*Not supported*

use re::lasterror() and re::getlasterrorstr()

re::lasterror() == 100 is OK

##7.2.3. Regular Expression Objects

###class re.RegexObject

*Supported*

####search(string[, pos[, endpos]])

*Supported*

std::unique_ptr<re::MatchObject> search(const std::string & Str, size_t pos = 0, int endpos = -1)

std::unique_ptr<re::MatchObjectW> search(const  std::wstring & Str, size_t pos = 0, int endpos = -1)

Error Handling: No

####match(string[, pos[, endpos]])

*Not supported*

*Note:* Please use search instead of match

####split(string, maxsplit=0)

*Supported*

std::vector<std::string> split(const std::string & Str, size_t maxsplit = 0)

std::vector<std::wstring> split(const std::wstring & Str, size_t maxsplit = 0)

Error Handling: No

####findall(string[, pos[, endpos]])

*Supported*

std::vector<std::string> findall(const std::string & Str, size_t pos = 0, int endpos = -1)

std::vector< std::wstring> findall(const  std::wstring & Str, size_t pos = 0, int endpos = -1)

Error Handling: No

####finditer(string[, pos[, endpos]])

*Supported*

std::unique_ptr<re::iter> finditer(const std::string & Str, size_t pos = 0, int endpos = -1);

std::unique_ptr<re::iterW> finditer(const  std::wstring & Str, size_t pos = 0, int endpos = -1)

Error Handling: No

####sub(repl, string, count=0)

*Supported*

std::string sub(const std::string & repl, const std::string & Str, size_t count = 0)

std::stringw sub(const std::stringw & repl, const std::stringw & Str, size_t count = 0)

Error Handling: repl only

####subn(repl, string, count=0)

*Supported*

std::tuple<std::string, size_t> subn(const std::string & repl, const std::string & Str, size_t count = 0)

std::tuple<std::wstring, size_t> subn(const std::wstring & repl, const std::wstring & Str, size_t count = 0)

Error Handling: repl only

####flags

*Supported*

size_t flags()

Error Handling: No

####groups

*Not supported*

####groupindex

*Not supported*

####pattern

*Supported*

std::string pattern()

std::wstring pattern()

Error Handling: No

##7.2.4. Match Objects

###class re.MatchObject

*Supported*

```C++

    auto match = re::search(pattern, string)
    if (match)
        process(match)

```

Match objects support the following methods and attributes:

####expand(template)

*Not supported*

####group([group1, ...])

*Supported*

std::string group(size_t i)

std::string group()

std::string group(std::wstring name)

std::wstring group(size_t i)

std::wstring group()

std::wstring group(std::wstring name)

Error Handling: No

Example 1:

```C++

    //We don't support match, use search instead
    auto m = re::search(R"(^(\w+) (\w+))", "Isaac Newton, physicist");
    if (m){
        std::cout << m->group(0) << std::endl;  //Isaac Newton
        std::cout << m->group(1) << std::endl;  //Isaac
        std::cout << m->group(2) << std::endl;  //Newton
    }

```
Example 2:

```C++

    //We don't support match, use search instead
    auto m = re::search(R"(^(?P<first_name>\w+) (?P<last_name>\w+))", "Malcolm Reynolds");
    if (m){
        std::cout << m->group("first_name") << std::endl;  //Malcolm
        std::cout << m->group("last_name") << std::endl;   //Reynolds
        std::cout << m->group(1) << std::endl;  //Malcolm
        std::cout << m->group(2) << std::endl;  //Reynolds
    }

```
Example 3:

```C++

    //We don't support match, use search instead
    auto m = re::search(R"(^(..)+)", "a1b2c3");
    if (m){
        std::cout << m->group(1) << std::endl;  //c3
    }

```

####groups([default])

*Supported*

std::vector<std::string> groups()

std::vector<std::wstring> groups()

Error Handling: No

Example 1:

```C++

    //We don't support match, use search instead
    auto m = re::search(R"(^(\d+)\.(\d+))", "24.1632");
    if (m){
        auto v = m->groups();
        for (auto i = v.begin(); i < v.end(); i++){
            std::cout << *i << std::endl;
        }
    }

	//24
	//1632

```

If we make the decimal place and everything after it optional, not all groups might participate in the match. These groups will default to None unless the default argument is given:

```C++

    //We don't support match, use search instead
    auto m = re::search(R"(^(\d+)\.?(\d+)?)", "24");
    if (m){
        auto v = m->groups();
        for (auto i = v.begin(); i < v.end(); i++){
            std::cout << *i << std::endl;
        }
    }

	//24

```

####groupdict([default])

*Not supported*

####start([group])
####end([group])

*Supported*

size_t start(size_t i)

size_t end(size_t i)

Error Handling: No

An example that will remove remove_this from email addresses:

```C++

    std::string email = "tony@tiremove_thisger.net";
    auto m = re::search("emove_this", email);
    //use str::string::substr instead of python string slicing
    std::cout << email.substr(0, m->start(0)-1) << email.substr(m->end(0), email.length() - m->end(0)) << std::endl;
    // tony@tiger.net

```

####span([group])

*Supported*
	
std::vector<int> span(size_t i)

Error Handling: No

	return type: 
	std::vector<int>   (size = 2)

####pos

*Supported*

size_t pos()

Error Handling: No

####endpos

*Supported*

size_t endpos()

Error Handling: No

####lastindex

*Supported*

####lastgroup

*Not supported*

size_t lastindex()

Error Handling: No

####re

*Supported*

std::shared_ptr<RegexObject> re()

std::shared_ptr<RegexObjectW> re()

Error Handling: No

####string

*Supported*

std::string string()

std::wstring string()

Error Handling: No

##7.2.5. Examples

See Examples in sample project