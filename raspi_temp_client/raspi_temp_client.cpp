/*As this is a GUi application under Windows, it should look for WinMain, not main. Check the project properties at Linker -> System and make sure "Subsystem:Windows" is selected.*/


#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"

enum
{
    wxID_BUTCONN = 0,
    wxID_BUTCONN2 = 1,
    wxID_BUTCONN3 = 2,
    wxID_BUTCONN4 = 3,
    wxID_DESC,
    wxID_SOCKET
};


class Api : public wxApp
{
public:
    virtual bool OnInit();
};


class Window : public wxFrame
{
protected: //Window Controls
    wxPanel* m_panel;
    wxButton* m_connect_button;
    wxButton* m_connect_button2;
    wxButton* m_connect_button3;
    wxButton* m_connect_button4;
    wxTextCtrl* m_log_box;
public:
    wxSocketClient* Socket;
    bool socketOpen = false;

    Window(const wxString& title, const wxPoint& pos, const wxSize& size,
        long style = wxDEFAULT_FRAME_STYLE);
    void FunConnect(wxCommandEvent& evt);
    void ReadTemp(wxCommandEvent& evt);
    void closeSocket(wxCommandEvent& evt);
    void Quit(wxCommandEvent& evt);
    void OnSocketEvent(wxSocketEvent& evt);
private:
    DECLARE_EVENT_TABLE()

};

//Event Table
BEGIN_EVENT_TABLE(Window, wxFrame)
EVT_BUTTON(wxID_BUTCONN, Window::FunConnect)
EVT_BUTTON(wxID_BUTCONN2, Window::ReadTemp)
EVT_BUTTON(wxID_BUTCONN3, Window::closeSocket)
EVT_BUTTON(wxID_BUTCONN4, Window::Quit)
EVT_SOCKET(wxID_SOCKET, Window::OnSocketEvent)
END_EVENT_TABLE()

void Window::FunConnect(wxCommandEvent& evt)
{
    m_log_box->AppendText(_T("Connecting to Mike's T&H server...\n"));
    m_connect_button->Enable(FALSE);
    m_connect_button2->Enable(TRUE);
    m_connect_button3->Enable(TRUE);

    //Connecting to the server
    wxIPV4address adr;
    //adr.Hostname(_T("192.168.1.51"));
    adr.Hostname(_T("96.32.41.209"));
    adr.Service(54000);

    //Create the socket
    Window::Socket = new wxSocketClient();

    Window::Socket->SetEventHandler(*this, wxID_SOCKET);
    Window::Socket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    Window::Socket->Notify(TRUE);

    Window::Socket->Connect(adr, false);
    m_log_box->AppendText(_T("Connecting...\n"));

    evt.Skip();

    return;
}

void Window::ReadTemp(wxCommandEvent& evt)
{
    char buffer[5];

    sprintf(buffer, "%s", "read");

    Window::Socket->Write(buffer, sizeof(buffer));
    evt.Skip();

}

void Window::closeSocket(wxCommandEvent& evt)
{
    char buffer[5];

    sprintf(buffer, "%s", "quit");

    Window::Socket->Write(buffer, sizeof(buffer));
    Window::Socket->Destroy();
    m_connect_button->Enable(TRUE);
    m_connect_button2->Enable(FALSE);
    m_connect_button3->Enable(FALSE);
    Window::socketOpen = false;
    evt.Skip();

}

void Window::Quit(wxCommandEvent& evt)
{
    char buffer[5];

    if (Window::socketOpen)
    {
        sprintf(buffer, "%s", "quit");
        Window::Socket->Write(buffer, sizeof(buffer));
        Window::Socket->Destroy();
        m_connect_button->Enable(TRUE);
        m_connect_button2->Enable(FALSE);
        m_connect_button3->Enable(FALSE);
        Window::socketOpen = false;
    }
    Close(true);
    evt.Skip();

}

void Window::OnSocketEvent(wxSocketEvent& evt)
{
    wxSocketBase* Sock = evt.GetSocket();

    char buffer[256];

    memset(buffer, 0, sizeof(buffer));

    switch (evt.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_CONNECTION) Connection successful\n\n"));

        sprintf(buffer, "0123456789!");

        Sock->Write(buffer, sizeof(buffer));
        m_log_box->AppendText(wxString("LOOPBACK TEST: ") + "\n");
        m_log_box->AppendText(wxString("    Sent ") + buffer + "\n");
        Window::socketOpen = true;
        break;
    }

    case wxSOCKET_INPUT:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_INPUT)\n"));
        Sock->Read(buffer, sizeof(buffer));
        m_log_box->AppendText(wxString("    received ") + buffer + "\n\n");

        break;
    }

    case wxSOCKET_LOST:
    {
        m_log_box->AppendText(_T("OnSocketEvent(wxSOCKET_LOST)\n"));
        Sock->Destroy();
        m_connect_button->Enable(TRUE);

        break;
    }

    default:
    {
        m_log_box->AppendText(_T("OnSocketEvent : unknown socket event\n"));
        break;
    }
    }
}

Window::Window(const wxString& title, const wxPoint& pos, const wxSize& size,
    long style) : wxFrame(NULL, -1, title, pos, size, style)
{
    m_panel = new wxPanel(this, wxID_ANY);
    m_connect_button = new wxButton(m_panel, wxID_BUTCONN, _T("connect"), wxPoint(10, 10), wxSize(100, 30));
    m_connect_button2 = new wxButton(m_panel, wxID_BUTCONN2, _T("read temp"), wxPoint(110, 10), wxSize(100, 30));
    m_connect_button2->Enable(FALSE);
    m_connect_button3 = new wxButton(m_panel, wxID_BUTCONN3, _T("close socket"), wxPoint(210, 10), wxSize(100, 30));
    m_connect_button3->Enable(FALSE);
    m_connect_button4 = new wxButton(m_panel, wxID_BUTCONN4, _T("quit"), wxPoint(310, 10), wxSize(100, 30));
    m_log_box = new wxTextCtrl(m_panel, wxID_DESC, wxEmptyString, wxPoint(10, 60), wxSize(400, 300), wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);
    m_log_box->SetValue("Welcome Mike's remote temperature and humidity server: Client\nClient Ready!\n\n");
}

bool Api::OnInit()
{

    Window* MainWindow = new Window(_T("Mike's T&H server: Client App"), wxDefaultPosition, wxSize(500, 500));
    MainWindow->Centre();
    MainWindow->Show(TRUE);

    return TRUE;
}

IMPLEMENT_APP(Api)