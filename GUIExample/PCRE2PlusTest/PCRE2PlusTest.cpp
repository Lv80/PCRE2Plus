#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/sysopt.h>
#include <string>
#include <vector>
#include "../../src/PCRE2Plus.h"


using namespace PCRE2Plus;
class MainFrame : public wxFrame {
protected:
    wxButton * m_buttonFindAll;
    wxTextCtrl * m_textCtrlRegex;
    wxTextCtrl * m_textCtrlString;
    wxTextCtrl * m_textCtrlFindAll;
public:
    MainFrame(const wxString& title) {
        wxXmlResource::Get()->LoadFrame(this, NULL, "Frame1");

        m_buttonFindAll = XRCCTRL(*this, "m_buttonFindAll", wxButton);
        m_textCtrlRegex = XRCCTRL(*this, "m_textCtrlRegex", wxTextCtrl);
        m_textCtrlString = XRCCTRL(*this, "m_textCtrlString", wxTextCtrl);
        m_textCtrlFindAll = XRCCTRL(*this, "m_textCtrlFindAll", wxTextCtrl);
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
        );
        //------------------------------------------------------------------------------
        m_buttonFindAll->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [=](wxCommandEvent &event){
            m_textCtrlFindAll->Clear();
            std::wstring a = m_textCtrlRegex->GetValue();
            auto R = re::compile(a);
            if (R){
                std::wstring s = m_textCtrlString->GetValue();
                auto v = R->findall(s);
                for (auto i = v.begin(); i < v.end(); i++){
                    m_textCtrlFindAll->AppendText(*i);
                    m_textCtrlFindAll->AppendText("\n");
                }
            }
            m_textCtrlFindAll->Refresh();

        });
        //------------------------------------------------------------------------------

    }
    virtual ~MainFrame(){
    }
   
};// end class MainFrame
//=============================================================================
class CJApp : public wxApp
{
public:
    CJApp() {
        m_pFrame = NULL;
    }
    virtual ~CJApp()
    {
        if (NULL != m_pFrame)
        {
            //    delete m_pFrame;
            m_pFrame = NULL;
        }
    }
public:
    virtual bool OnInit()
    {
        wxSystemOptions::SetOption("msw.remap", 2);
        wxInitAllImageHandlers();
        wxXmlResource::Get()->InitAllHandlers();
        FILE * f;
        std::string path;
        path = "..\\PCRE2PlusTest\\Frame1.xrc";
        f = fopen(path.c_str(), "r");
        if (f != NULL) {
            if (wxXmlResource::Get()->Load(path)){
                fclose(f);
                goto success;
            }
        }
        path = ".\\Frame1.xrc";
        f = fopen(path.c_str(), "r");
        if (f != NULL) {
            if (wxXmlResource::Get()->Load(path)){
                fclose(f);
                goto success;
            }
        }
        return false;
    success:
        m_pFrame = new MainFrame("");
        m_pFrame->Show(true);
        return true;
    }
private:
    MainFrame* m_pFrame;
};

DECLARE_APP(CJApp)
IMPLEMENT_APP(CJApp)