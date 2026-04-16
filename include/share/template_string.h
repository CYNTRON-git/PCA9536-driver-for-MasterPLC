#pragma once
#include <mplc/libs/bind.hpp>
#include <share/mplcshare.h>

struct TemplateString final {
    typedef std::string lua_type;
    enum StatusCode { OK, PlainText, BadTmpl, VarNotFound };
    typedef mplc::lib::function<bool(const std::string&, std::ostream&)> callback_t;

public:  // ------------- Constructors -------------
    template<class Func, class Type>
    TemplateString(Func func, Type& ref)
        : func(mplc::lib::bind(func, &ref, _1, _2)), status(OK) {}  // may change to lambda
    explicit TemplateString(callback_t cb): func(std::move(cb)), status(OK) {}
    explicit TemplateString(const TemplateString& copy)
        : func(copy.func), m_str_tmpl(copy.m_str_tmpl), status(copy.status) {}
    TemplateString(TemplateString&& copy) noexcept
        : func(std::move(copy.func)), m_str_tmpl(std::move(copy.m_str_tmpl)), status(copy.status) {}
    virtual ~TemplateString() = default;

public:  // -------------  Operators  --------------
    TemplateString& operator=(const TemplateString& _tmpl) {
        status = _tmpl.status;
        m_str_tmpl = _tmpl.m_str_tmpl;
        func = _tmpl.func;
        return *this;
    }
    TemplateString& operator=(TemplateString&& _tmpl) noexcept {
        status = _tmpl.status;
        m_str_tmpl = std::move(_tmpl.m_str_tmpl);
        func = std::move(_tmpl.func);
        return *this;
    }
    TemplateString& operator=(const std::string& _tmpl) {
        status = check_tmpl(_tmpl);
        m_str_tmpl = _tmpl;
        return *this;
    }

public:  // -------------   Methods   -------------
    MPLCSHARE_API std::string str() const;
    MPLCSHARE_API StatusCode format(std::ostream& out) const;
    operator std::string() const {
        return str();
    }
    MPLCSHARE_API static StatusCode check_tmpl(const std::string& new_tmpl);
    const std::string& raw() {
        return m_str_tmpl;
    }

    void Swap(TemplateString& obj) {
        std::swap(obj.m_str_tmpl, m_str_tmpl);
        std::swap(obj.func, func);
    }

private:
    friend struct AsUtf8;
    callback_t func;
    std::string m_str_tmpl;
    StatusCode status;
};
