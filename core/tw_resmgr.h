#pragma once

#include <map>
#include <string>

#include "tw_util.h"

namespace taowin{
    class window_creator;
    class control;
    class syscontrol;

    class resmgr {
    public:
        typedef std::map<string, HFONT> font_maps_t;

        friend class window_creator;
        friend class control;
        friend class syscontrol;

	public:
		resmgr();
		~resmgr();

		void    add_font(const TCHAR* name, const TCHAR* face, int size);
        HFONT   get_font(const TCHAR* name) {
            auto it = _fonts.find(name);
            if(it != _fonts.cend())
                return it->second;
            return nullptr;
        }
		void    set_hwnd(HWND h){
			_hwnd = h;
		}

        HWND    get_hwnd() const {
            return _hwnd;
        }

        unsigned int next_ctrl_id()
        {
            return _next_ctrl_id++;
        }

	private:
		HWND        _hwnd;
        font_maps_t _fonts;
        window_creator* _owner;
        unsigned int    _next_ctrl_id;
	};
}