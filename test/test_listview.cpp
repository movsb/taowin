#include <windows.h>
#include <commctrl.h>

#include <taowin/core/tw_taowin.h>

namespace {

class DataSource : public taowin::ListView::IDataSource
{
protected:
    virtual size_t size() const override
    {
        return 100;
    }

    virtual LPCTSTR get(int item, int subitem) const override
    {
        static taowin::string text;
        // text = !subitem ? std::to_string(item) : std::to_string(subitem);
        return text.c_str();
    }
};

class TW : public taowin::WindowCreator
{
public:
    TW()
    {
    }

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
            <listview name="lv" style="singlesel,ownerdata" exstyle="clientedge">  </listview>
        </Vertical>
    </Root>
</Window>
)tw");
        return json;
    }

    virtual LRESULT handle_message(UINT umsg, WPARAM wparam, LPARAM lparam) override
    {
        switch(umsg) {
        case WM_CREATE:
        {
            //center();
            auto lv = _root->find<taowin::ListView>(_T("lv"));
            auto& cm = lv->get_column_manager();
            cm.push(_T("item1"), true, 50, _T("id1"));
            cm.push(_T("item2"), true, 100, _T("id2"));
            cm.push(_T("item3"), true, 150, _T("id3"));
            lv->update_columns();
            lv->set_source(&_data);

            lv->on_custom_draw([this](NMHDR* hdr) {
                LogLog(_T("�Ի�"));
                return 0;
            });

            lv->on_double_click([this](int item, int subitem) {
                LogLog(_T("˫����%d, %d��"), item, subitem);
                return 0;
            });

            lv->on_header_divider_dblclick([this](NMHDR* hdr) {
                LogLog(_T("��ͷ�ָ���˫��"));
                return 0;
            });

            lv->on_header_end_drag([this]() {
                LogLog(_T("�����Ϸ�"));
                return 0;
            });

            lv->on_header_end_track([this](NMHDR* hdr) {
                LogLog(_T("�����϶�"));
                return 0;
            });

            lv->on_header_rclick([this]() {
                LogLog(_T("��ͷ�Ҽ�����"));
                return 0;
            });

            lv->on_item_changed([this](NMHDR* hdr) {
                LogLog(_T("״̬�ı�"));
                return 0;
            });

            lv->on_key_down([this](NMHDR* hdr) {
                LogLog(_T("��������"));
                return 0;
            });

            lv->on_right_click([this](int item, int subitem) {
                LogLog(_T("�Ҽ����� %d,%d"), item, subitem);
                return 0;
            });

            return 0;
        }
        }
        return __super::handle_message(umsg, wparam, lparam);
    }

    virtual taowin::SystemControl* filter_control(HWND hwnd) override
    {
        auto lv = _root->find<taowin::ListView>(_T("lv"));
        if(hwnd == lv->get_header()) return lv;
        return nullptr;
    }
private:
    DataSource _data;
};

}

void test_listview()
{
    auto w = new TW;
    w->create();
    w->show();
}
