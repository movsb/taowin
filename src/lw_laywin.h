#ifndef __laywin_h__
#define __laywin_h__

#include "lw_util.h"
#include "lw_control.h"
#include "lw_syscontrols.h"
#include "lw_window.h"
#include "lw_resmgr.h"

namespace laywin{

    class window_creator : public window
	{
    public:
        window_creator();
        ~window_creator();

	protected:
		virtual LPCTSTR get_skin_xml() const;
		virtual LRESULT handle_message(UINT umsg, WPARAM wparam, LPARAM lparam);
		virtual LRESULT on_menu(int id, bool is_accel = false) { return 0; }
		virtual LRESULT on_notify(HWND hwnd, control* pc, int code, NMHDR* hdr) { return 0; }

	private:
		virtual LRESULT __handle_message(UINT umsg, WPARAM wparam, LPARAM lparam) override;

	protected:
        container*  _root;
        resmgr      _mgr;
	};

    void init();
    int loop_message();
}

#endif//__laywin_h__
