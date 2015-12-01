#include "tw_window.h"

namespace taowin{

    window_manager __window_manager;

	window::window()
		: _hwnd(NULL)
        , _is_dialog(false)
        , _return_code(0)
	{

	}

	window::~window()
	{

	}

	HWND window::create()
	{
        window_meta_t metas;
        get_metas(&metas);
		_hwnd = ::CreateWindowEx(metas.exstyle, metas.classname, metas.caption, metas.style,
            100, 100, 300, 250, nullptr, nullptr, nullptr, this);
		assert(_hwnd);
		return _hwnd;
	}

	int window::domodal(HWND owner)
	{
        assert(owner != nullptr);
        _is_dialog = true;

        window_meta_t metas;
        get_metas(&metas);
		_hwnd = ::CreateWindowEx(metas.exstyle, metas.classname, metas.caption, metas.style,
            100, 100, 300, 250, owner, nullptr, nullptr, this);
		assert(_hwnd);
        show();
        return dialog_manager(this, owner).run();
	}

	void window::show(bool show /*= true*/, bool focus /*= true*/)
	{
		assert(::IsWindow(_hwnd));
		::ShowWindow(_hwnd, show ? (focus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
	}

	void window::center()
	{
		assert(::IsWindow(_hwnd));
		assert((GetWindowLongPtr(_hwnd, GWL_STYLE)&WS_CHILD) == 0);
		RECT rcDlg = {0};
		::GetWindowRect(_hwnd, &rcDlg);
		RECT rcArea = {0};
		RECT rcCenter = {0};
		HWND hWnd = *this;
		HWND hWndParent = ::GetParent(_hwnd);
		HWND hWndCenter = ::GetWindow(_hwnd, GW_OWNER);
		if(hWndCenter != NULL)
			hWnd = hWndCenter;

		// ��������ʾ��ģʽ����Ļ����
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rcArea = oMonitor.rcWork;

		if(hWndCenter == NULL)
			rcCenter = rcArea;
		else
			::GetWindowRect(hWndCenter, &rcCenter);

		int DlgWidth = rcDlg.right - rcDlg.left;
		int DlgHeight = rcDlg.bottom - rcDlg.top;

		// Find dialog's upper left based on rcCenter
		int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
		int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

		// The dialog is outside the screen, move it inside
		if(xLeft < rcArea.left) xLeft = rcArea.left;
		else if(xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
		if(yTop < rcArea.top) yTop = rcArea.top;
		else if(yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
		::SetWindowPos(_hwnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	LRESULT window::__handle_message(UINT umsg, WPARAM wparam, LPARAM lparam)
	{
        if(umsg == WM_CLOSE) {
            HWND owner = ::GetWindow(_hwnd, GW_OWNER);
            if(owner && _is_dialog) {
                ::PostQuitMessage(_return_code);
                ::EnableWindow(owner, TRUE);
                ::SetActiveWindow(owner);
            }
        }
        return ::DefWindowProc(_hwnd, umsg, wparam, lparam);
	}

	void window::on_first_message()
	{

	}

	void window::on_final_message()
	{

	}

	LRESULT __stdcall window::__window_procedure(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		window* pThis = reinterpret_cast<window*>(::GetWindowLongPtr(hwnd, 4));

		if(umsg == WM_NCCREATE) {
			LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
			pThis = static_cast<window*>(lpcs->lpCreateParams);
			pThis->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, 4, reinterpret_cast<LPARAM>(pThis));
            if(!pThis->_is_dialog)
                __window_manager.add_message_filter(pThis);
			pThis->on_first_message();
            return TRUE; // must
		}
        else if(umsg == WM_NCDESTROY) {
            ::SetWindowLongPtr(pThis->_hwnd, 4, 0);
            LRESULT lRes = ::DefWindowProc(hwnd, umsg, wparam, lparam);
            if(pThis) {
                if(!pThis->_is_dialog)
                    __window_manager.remove_message_filter(pThis);
                pThis->on_final_message();
            }
            return 0;
		}

        return pThis
            ? pThis->__handle_message(umsg, wparam, lparam)
            : ::DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	void window::close(int code)
	{
        _return_code = code;
		send_message(WM_CLOSE);
	}

    void window::get_metas(window_meta_t* metas) {
        metas->caption = "taowin";
        metas->classname = "taowin";
        if(_is_dialog)
            metas->style = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZE|WS_MINIMIZEBOX);
        else
            metas->style = WS_OVERLAPPEDWINDOW;
        metas->exstyle = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    void register_window_classes() {
        WNDCLASSEX wc = {0};
        wc.cbSize = sizeof(wc);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wc.hIcon = wc.hIconSm = (HICON)::LoadImage(::GetModuleHandle(nullptr), (LPCSTR)101, IMAGE_ICON, 32, 32, 0);
        wc.hInstance = ::GetModuleHandle(nullptr);
        wc.lpfnWndProc = &window::__window_procedure;
        wc.lpszClassName = "taowin";
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.cbWndExtra = sizeof(void*)* 2; // [[extra_ptr][this]]
        ::RegisterClassEx(&wc);
    }
}