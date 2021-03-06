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
            <Progress height="40" name="c" range="0,100" color="255,0,0" bkcolor="255,255,0" />
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
            _c = _root->find<taowin::Progress>(_T("c"));
            _c->set_pos(50);
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
    taowin::Progress* _c;
};

}

void test_progress()
{
    auto w = new TW;
    w->create();
    w->show();
}
