#include "../include/NetLayer/NetLayer.hpp"


#define NL_DEFINE_PCKT_PROXY(mIdx, mName) \
	inline const auto& mName() const { return std::get<mIdx>(this->fields); } \
	inline auto& mName() { return std::get<mIdx>(this->fields); }



#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL(mIdx, mData, mArg) \
	SSVPP_TPL_ELEM(mArg, 0) SSVPP_COMMA_IF(mIdx)

#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(...) \
	SSVPP_FOREACH_REVERSE(NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL, SSVPP_TPL_MAKE(), __VA_ARGS__)


#define NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(mIdx, mData, mArg) \
	NL_DEFINE_PCKT_PROXY(mIdx, SSVPP_TPL_ELEM(mArg, 1))

#define NL_IMPL_DEFINE_PCKT_BODY_LIST(...) \
	SSVPP_FOREACH(NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL, SSVPP_TPL_MAKE(), __VA_ARGS__)



#define NL_DEFINE_PCKT(mName, mFieldTpls) \
	struct mName : ::nl::Pckt \
	< \
		NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(SSVPP_TPL_EXPLODE(mFieldTpls)) \
	> \
	{ \
		NL_IMPL_DEFINE_PCKT_BODY_LIST(SSVPP_TPL_EXPLODE(mFieldTpls)) \
	}

NL_DEFINE_PCKT
(
	AuthRequest,
	(
		(int, 				requestID),
		(std::string, 		requestUser),
		(float, 			requestPriority),
		(std::vector<int>,	secondaryIDs)
	)
);
/*
NL_DEFINE_PCKT
(
	TestPckt, (())
);
*/
struct AuthSuccess : nl::Pckt
<
>
{

};

struct AuthFailure : nl::Pckt
<
>
{

};

struct ImportantPacket : nl::Pckt
<
>
{

};

using ClientPackets = nl::PcktSet
<
	nl::PcktIDAuto<AuthRequest>,
	nl::PcktIDFixed<100, ImportantPacket>
>;

using ServerPackets = nl::PcktSet
<
	nl::PcktIDAuto<AuthSuccess>,
	nl::PcktIDAuto<AuthFailure>
>;

// nl::ManagedHost test;


// using Test = nl::PcktManager<MyPackets>;

int main2()
{
	#define LOSIZE(x) ssvu::lo("sizeof("s + SSVPP_TOSTR(x) + ")") << sizeof(x) << "\n"

	LOSIZE(AuthRequest);
	LOSIZE(AuthSuccess);
	LOSIZE(AuthFailure);
	LOSIZE(ImportantPacket);

	SSVU_ASSERT_STATIC_NM(ClientPackets::getPcktID<AuthRequest>() == 0);
	SSVU_ASSERT_STATIC_NM(ClientPackets::getPcktID<ImportantPacket>() == 100);

	SSVU_ASSERT_STATIC_NM(ServerPackets::getPcktID<AuthSuccess>() == 0);
	SSVU_ASSERT_STATIC_NM(ServerPackets::getPcktID<AuthFailure>() == 1);

	SSVU_ASSERT_STATIC_NM(ssvu::isSame<ClientPackets::PcktType<0>, AuthRequest>());
	SSVU_ASSERT_STATIC_NM(ssvu::isSame<ClientPackets::PcktType<100>, ImportantPacket>());

	SSVU_ASSERT_STATIC_NM(ssvu::isSame<ServerPackets::PcktType<0>, AuthSuccess>());
	SSVU_ASSERT_STATIC_NM(ssvu::isSame<ServerPackets::PcktType<1>, AuthFailure>());

	nl::PcktBuf pb;
	pb << ssvu::Tpl<int, float, std::string>{};
	pb << AuthRequest{};

	AuthRequest p;
	p.requestID() = 5;

	return 0;
}
