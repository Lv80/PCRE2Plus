![LICENSE](https://img.shields.io/badge/LICENSE-BSD-YELLOW.svg)
![C++11](https://img.shields.io/badge/C%2B%2B-11-green.svg)
![PCRE2](https://img.shields.io/badge/PCRE-2-orange.svg)
![Windows](https://img.shields.io/badge/Windows-%E2%9C%93-brightgreen.svg)
![Linux](https://img.shields.io/badge/Linux-%E2%9C%93-brightgreen.svg)
![OSX](https://img.shields.io/badge/OSX-%E2%9C%93-brightgreen.svg)

#PCRE2 Plus

##Description

A python style re wrapper for PCRE2, C++ 11 standard followed, easy handling by using smart pointers. 

Unicode and wide string friendly.

The *motivation* for making this project is to adapt the powerful PCRE2 utility to C++ followed by Python standard, so the user don't need to remember the details of c style regexp functions provide by PCRE2 as long as he/she can write regexp function in Python style.


##Installation 

###Install PCRE2
Homepage and Download: [http://www.pcre.org/](http://www.pcre.org/)

**Minimal PCRE2 version required: pcre2-10.21**

Alternately, pcre2-10.21 is cached in this repo: [pcre-2-10.21.zip](3PP/pcre2-10.21.zip)


###Install on Windows (VC++)

Download build bat file from this repo: 
[build_pcre2_10.21.bat](3PP/build_pcre2_10.21.bat) 

	1. Extract pcre2-10.21.zip
	2. Place the build bat file in the source
	2. Start VC++ command line
	3. Go the directory of pcre2 source
	4. Execute build bat
	5. If everything OK, you'll get three lib files: libpcre2-8.lib libpcre2-16.lib libpcre2-32.lib

###Install on *nix

	1. Extract pcre2-10.21.zip
	2. Go to pcre2 source directory
	2. ./configuere   --enable-pcre2-16 --enable-pcre2-32
	3. make && make install 
	
**make sure 16 and 32 bit character support are supported besides 8 bit**

##Usage

Put PCRE2 source directory to C++ project include and library

Put PCRE2 lib files to project linker parameter (all 8, 16 and 32 bit)

### Add PCRE2 to VC++
![vcdir](Img/vcdir.png)
![vcinput](Img/vcinput.png)

### Add PCRE2 to Xcode
Presume the pcre2 is installed to ~/.local

![xcodesetting](Img/xcodesetting.png)

### Add PCRE2Plus to Project

Include [PCRE2Plus.h](PCRE2Plus/PCRE2Plus.h) in Project

Add [PCRE2Plus.cpp](PCRE2Plus/PCRE2Plus.cpp) to Project

```c++

    #include "PCRE2Plus.h"
    using namespace PCRE2Plus;
    std::string c = R"(\w+)";
    auto Regex = re::compile(c);
    if (Regex){
        std::cout<<"OK"<<std::endl;
        auto M = Regex->search("abc");
        if (M){
            std::cout<<(M->group())<<std::endl;
        }
    }
    else{
        std::cout<<"NOK"<<std::endl;
        std::cout<<re::getlasterror()<<std::endl;
        std::cout<<re::getlasterrorstr()<<std::endl;
    }

```

##Syntax

Python re syntax document: [https://docs.python.org/2/library/re.html](https://docs.python.org/2/library/re.html)

BASELINE: 2.7.11

[State of compliance for python re document](SOC.md)

###Unicode Support

Unicode are support by PCRE2

Make sure 16 bit(for VC++) or 32 bit (for *nix/Mac) are linked to the project.

All Unicode Chars shall be processed by wide string(std::wstring, begins with "L" indicator).

And make sure, once the wide string is used for strings, pattern and repl **MUST** be wide string also.

	There are two kinds of objects: ObjFoo and ObjFooW, the latter one is for wide string. 
	
	Every logical code has two copys, one for ANSI and the other for wide string.

	Users shall not aware of the difference as they can use keyword auto to take the object(smart pointer) returned from compile or search functions. 
	
	Of course, user shall use std::string or std::wstring in the last phase based on they input string type.


Example of matching Chinese chars (wide string)

```c++

    auto v = re::findall(LR"(\p{Han})", L"赵 钱 孙 李");
    for (auto i = v.begin(); i < v.end();i++){
        std::wcout<<(*i)<<std::endl;
    }

	//赵
	//钱
	//孙
	//李
    
```

Reference of Unicode Category:

[http://www.regular-expressions.info/unicode.html](http://www.regular-expressions.info/unicode.html)

##Exceptional Handling

There is no exception handling

nullptr or empty string or empty vector will be returned if anything goes wrong

Use following function for regexp compilation error or sub/subn function

re::getlasterror()  // != 100 for abnormal cases

re::getlasterrorstr()

re::geterroroffset()


Examples:
There is a wxwidgets based regex test project in repo

[src](PCRE2PlusTest)

[Binary](PCRE2PlusTest/Binary.zip)

Here are some examples

Snippet

```C++

        m_textCtrlRegex->Bind(wxEVT_TEXT, [=](wxCommandEvent &event){
            std::wstring a = m_textCtrlRegex->GetValue();
            auto R = re::compile(a);
            if (R){
                m_textCtrlRegex->SetBackgroundColour(wxColor(0,255,0));
                m_textCtrlRegex->Refresh();
                m_textCtrlFindAll->Clear();
                m_textCtrlFindAll->Refresh();
            }
            else{
                m_textCtrlRegex->SetBackgroundColour(wxColor(255, 0, 0));
                m_textCtrlRegex->Refresh();
                m_textCtrlFindAll->Clear();
                m_textCtrlFindAll->AppendText(std::to_string(re::getlasterror()));
                m_textCtrlFindAll->AppendText("\n");
                m_textCtrlFindAll->AppendText(re::getlasterrorstr());
                m_textCtrlFindAll->Refresh();
            }
        }

```

![screenshot](Img/screenshot.gif)

![err1](Img/err1.png)

![err2](Img/err2.png)

![err3](Img/err3.png)

![chs](Img/chs.png)

![jp](Img/jp.png)


**No error** shall be trigged during the RegexObj.search function

For details see [State of compliance for python re document](SOC.md) document

##Known Issues
The current phase of this project is to support basic function and handle unicode texts properly

some functions related to the position are not fully tested(although a few other functions are depend on them and works)

##Developer
API: [http://www.pcre.org/current/doc/html/pcre2api.html](http://www.pcre.org/current/doc/html/pcre2api.html)

Coding style for PCRE2Plus:

tab => 4 spaces

Unix Line ending


## License

**BSD**

>    Copyright (c) 2016, Boying Xu
>    All rights reserved.

>    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

>    1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

>    2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

>    3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

>    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUB
>    STITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.