#include <windows.h>
#include <commctrl.h>

#include <taowin/core/tw_taowin.h>

namespace {

class TW : public taowin::WindowCreator
{
public:
	TW()
	{}

protected:
	virtual LPCTSTR get_skin_xml() const override
	{
        LPCTSTR json = _T(R"tw(
<Window title="taowin��ʾ����" size="500,300">
    <Resource>
        <Font name="default" face="΢���ź�" size="12"/>
    </Resource>
    <Root>
        <Vertical padding="5,5,5,5">
            <TextBox name="c" foreground="#00FF00" background="#FF0000"/>
            <TextBox name="c" foreground="#0000FF" background="#00FFFF" text="asd" style="readonly"/>
        </Vertical>
    </Root>
</Window>
)tw");
		return json;
	}

	virtual LRESULT handle_message(UINT umsg, WPARAM wparam, LPARAM lparam) override
	{
		switch(umsg)
		{
		case WM_CREATE:
		{
            _c = _root->find<taowin::TextBox>(_T("c"));
            _c->on_change([this] {
                LogLog(_T("���ݸı䣺%s\n"), _c->get_text().c_str());
                return 0;
            });
			return 0;
		}
		}
        return __super::handle_message(umsg, wparam, lparam);
	}

    virtual taowin::SystemControl* filter_control(HWND hwnd) override
    {
        return nullptr;
    }

private:
    taowin::TextBox* _c;
};

}

void test_edit()
{
    auto w = new TW;
    w->create();
    w->show();
}
