#include <windows.h>
#include <commctrl.h>

#include <taowin/core/tw_taowin.h>


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
        <Font name="1" face="΢���ź�" size="12"/>
    </Resource>
    <Root>
        <Vertical padding="5,5,5,5">
            <combobox name="c" />
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
            _c = _root->find<taowin::ComboBox>(_T("c"));
            _c->add_string(_T("111"));
            _c->add_string(_T("222"));
            _c->add_string(_T("333"));

            _c->on_sel_change([this](int index, void* ud) {
                LogLog(_T("ѡ�иı� %d,%p"), index, ud);
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

    taowin::ComboBox* _c;
};

void test_combobox()
{
    auto w = new TW;
    w->create();
    w->show();
}
