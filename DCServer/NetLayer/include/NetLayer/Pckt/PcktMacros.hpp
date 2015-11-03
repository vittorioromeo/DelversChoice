#pragma once

#include <vrm/pp.hpp>

#define NL_DEFINE_PCKT_PROXY(mI, mName)                                     \
    inline const auto& mName() const { return std::get<mI>(this->fields); } \
    inline auto& mName() { return std::get<mI>(this->fields); }


// Implementation of the preprocessor loop that generates the type list for
// packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL(mIdx, mData, mArg) \
    VRM_PP_TPL_EXPLODE(VRM_PP_TPL_ELEM(mArg, 0)) VRM_PP_COMMA_IF(mIdx)

// Preprocessor loop that generates the type list for packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(...) \
    VRM_PP_FOREACH_REVERSE(                    \
        NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL, ~, __VA_ARGS__)



// Implementation of the preprocessor loop that generates the fields inside the
// packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(mIdx, mData, mArg) \
    NL_DEFINE_PCKT_PROXY(mIdx, VRM_PP_TPL_ELEM(mArg, 1))

// Preprocessor loop that generates the fields inside the packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST(...) \
    VRM_PP_FOREACH(NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL, ~, __VA_ARGS__)

#define NL_IMPL_PCKT_BASETYPE(mName, mFieldTpls)        \
    ::nl::Impl::Pckt<NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST( \
        VRM_PP_TPL_EXPLODE(mFieldTpls))>

#define NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)                        \
    struct mName : NL_IMPL_PCKT_BASETYPE(mName, mFieldTpls)           \
    {                                                                 \
        using BaseType = NL_IMPL_PCKT_BASETYPE(mName, mFieldTpls);    \
        using BaseType::BaseType;                                     \
        NL_IMPL_DEFINE_PCKT_BODY_LIST(VRM_PP_TPL_EXPLODE(mFieldTpls)) \
    }

#define NL_DEFINE_PCKT(mName, mFieldTpls) NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)


#define NL_IMPL_PCKT_BASETYPE_1(mName, mTpl) \
    ::nl::Impl::Pckt<VRM_PP_TPL_ELEM(VRM_PP_TPL_ELEM(mTpl, 0), 0)>

#define NL_IMPL_PCKT_BASETYPE_0(mName) ::nl::Impl::Pckt<char>

#define NL_DEFINE_PCKT_1(mName, mTpl)                          \
    struct mName : NL_IMPL_PCKT_BASETYPE_1(mName, mTpl)        \
    {                                                          \
        using BaseType = NL_IMPL_PCKT_BASETYPE_1(mName, mTpl); \
        using BaseType::BaseType;                              \
        NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(0, ~, mTpl)     \
    }


#define NL_DEFINE_PCKT_0(mName)                          \
    struct mName : NL_IMPL_PCKT_BASETYPE_0(mName)        \
    {                                                    \
        using BaseType = NL_IMPL_PCKT_BASETYPE_0(mName); \
        using BaseType::BaseType;                        \
    }
