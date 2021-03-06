#include <cctype>
#include <cstring>

#include <cstring>
#include <memory>

#include "tw_parser.h"

#include <map>
#include <vector>
#include <string>

namespace taowin {
    namespace parser {
        enum class TK {
            error = 0, tag, attr, assign, value, text, close, close1, close2, eof, next,
        };

        static TCHAR* p = nullptr;

        static inline void _skip() {
            while(*p == _T(' ') || *p == _T('\t') || *p == _T('\r') || *p == _T('\n'))
                p++;
        }

        enum class SC {
            init, tag,
        };

        static SC sc = SC::init;
        static TK tk = TK::error;
        static const TCHAR* tt;  // token text
        static TCHAR nc; // next char to be used, if not zero

        static void next() {
            if(nc) {
                *p = nc;
                nc = 0;
            }
            if(sc == SC::init) {
                _skip();
                if(*p == _T('<')) {
                    p++;
                    if(*p == _T('/')) {
                        p++;
                        tt = p;
                        while(::isalnum(*p))
                            p++;
                        if(*p == _T('>')) {
                            *p = _T('\0');
                            p++;
                            sc = SC::init;
                            tk = TK::close2;
                            return;
                        }
                    } else if(::isalpha(p[0])) {
                        tt = p;
                        while(::isalnum(*p))
                            p++;
                        if(*p != _T('\0')) {
                            nc = *p;
                            *p = _T('\0');
                        }
                        sc = SC::tag;
                        tk = TK::tag;
                        return;
                    }
                } else {
                    tt = p;
                    while(*p && *p != _T('<') && *p != _T('>'))
                        p++;
                    if(*p != _T('\0')) {
                        nc = *p;
                        *p = _T('\0');
                    }
                    tk = *tt ? TK::text
                        : *p ? TK::error
                        : TK::eof;
                    return;
                }
            } else if(sc == SC::tag) {
                _skip();
                if(::isalpha(*p)) {
                    tt = p;
                    while(::isalnum(*p))
                        p++;
                    if(*p != _T('\0')) {
                        nc = *p;
                        *p = _T('\0');
                    }
                    tk = TK::attr;
                    return;
                } else if(*p == _T('=')) {
                    *p = _T('\0');
                    p++;
                    tk = TK::assign;
                    return;
                } else if(*p == _T('\'') || *p == _T('\"')) {
                    const TCHAR c = *p++;
                    tt = p;
                    while(*p && *p != c)
                        p++;
                    if(*p == c) {
                        *p = _T('\0');
                        p++;
                        tk = TK::value;
                        return;
                    }
                } else if(*p == _T('/')) {
                    *p = _T('\0');
                    p++;
                    if(*p == _T('>')) {
                        p++;
                        sc = SC::init;
                        tk = TK::close1;
                        return;
                    }
                } else if(*p == _T('>')) {
                    p++;
                    sc = SC::init;
                    tk = TK::close;
                    return;
                }
            }

            if(!*p) {
                tk = TK::eof;
                return;
            }

            tk = TK::error;
            return;
        }

        inline bool is(const TCHAR* t, const TCHAR* a) {
            return ::_tcscmp(t, a) == 0;
        }


        static void parse(TCHAR* xml, PARSER_OBJECT* parent) {
            p = xml;
            for(;;) {
                next();
                if(tk == TK::tag) {
                    const TCHAR* tag_name = tt;
                    PARSER_OBJECT* obj = new PARSER_OBJECT;
                    obj->tag = tag_name;
                    parent->append_child(obj);
                    next();
                    while(tk == TK::attr) {
                        //std::cout << " " << tt;
                        const TCHAR* attr = tt;
                        next();
                        if(tk == TK::assign) {
                            //std::cout << "=\"";
                            next();
                            if(tk == TK::value) {
                                obj->set_attr(attr, tt);
                                next();
                                continue;
                            }

                            throw _T("value expected after assignment.");
                        }
                        obj->set_attr(attr, _T(""));
                    }

                    if(tk == TK::close) {
                        //std::cout << "\n";
                        for(;;) {
                            parse(p, obj);
                            if(tk == TK::next)
                                continue;

                            else if(tk == TK::close2) {
                                if(::_tcsncmp(tag_name, tt, ::_tcslen(tt)) != 0)
                                    throw _T("mismatched opening tag and closing tag.");

                                tk = TK::next;
                                //std::cout << tt;
                                //::cout << "\n";
                                return;
                            }

                            if(tk == TK::eof) throw _T("premature eof, expecting closing tag.");
                            else throw _T("expecting closing tag.");
                        }
                    } else if(tk == TK::close1) {
                        tk = TK::next;
                        //std::cout << "\n";
                        return;
                    }

                    throw _T("unexpected following token for open tag.");
                } else if(tk == TK::text) {
                    tk = TK::next;
                    //std::cout << "<!CDATA[" << tt << "]]\n";
                    return;
                } else if(tk == TK::close2) {
                    return;
                } else if(tk == TK::eof) {
                    return;
                }

                throw _T("unexpected token while calling parse.");
            }
        }

        PARSER_OBJECT* parse(TCHAR* xml, const TCHAR** err) {
            PARSER_OBJECT po;

            if(!xml) xml = _T("");

            try {
                parse(xml, &po);
            }
            catch(const TCHAR* e) {
                if(err) *err = e;
                return nullptr;
            }

            return po.first_child();
        }

        PARSER_OBJECT* parse(const TCHAR* xml, const TCHAR** err) {
            xml = xml ? xml : _T("");
            int len = (int)::_tcslen(xml) + 1;
            std::unique_ptr<TCHAR[]> x(new TCHAR[len]);
            ::memcpy(x.get(), xml, len*sizeof(TCHAR));
            return parse(x.get(), err);
        }

    }
}
