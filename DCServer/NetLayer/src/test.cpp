#include "../include/NetLayer/NetLayer.hpp"
#include "/home/vittorioromeo/OHWorkspace/private/cpp/cppcon2015/definitive_WIP/Other/Other.hpp"

// TODO:
// * decide abstraction layers
// * decide packet definition and settings definitionsyntax
// * implement an additional layer that abstracts the Architecture module over PacketTypes
// * test stuff.

// Defines getters and setters for a packet field.
#define NL_DEFINE_PCKT_PROXY(mIdx, mName) \
	inline const auto& mName() const { return std::get<mIdx>(this->fields); } \
	inline auto& mName() { return std::get<mIdx>(this->fields); }

// Implementation of the preprocessor loop that generates the type list for packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL(mIdx, mData, mArg) \
	SSVPP_TPL_EXPLODE(SSVPP_TPL_ELEM(mArg, 0)) SSVPP_COMMA_IF(mIdx)

// Preprocessor loop that generates the type list for packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(...) \
	SSVPP_FOREACH_REVERSE(NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL, SSVPP_TPL_MAKE(), __VA_ARGS__)



// Implementation of the preprocessor loop that generates the fields inside the packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(mIdx, mData, mArg) \
	NL_DEFINE_PCKT_PROXY(mIdx, SSVPP_TPL_ELEM(mArg, 1))

// Preprocessor loop that generates the fields inside the packet type.
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
/*
struct AuthRequest : nl::Pckt
<
	int, 				// 0
	std::string, 		// 1
	float, 				// 2
	std::vector<int>	// 3
>
{
	NL_DEFINE_PCKT_PROXY(0, requestID);
	NL_DEFINE_PCKT_PROXY(1, requestUser);
	NL_DEFINE_PCKT_PROXY(2, requestPriority);
	NL_DEFINE_PCKT_PROXY(3, secondaryIDs);
};
*/
NL_DEFINE_PCKT
(
	AuthRequest,
	(
		((int), requestID),
		((std::string), requestUser),
		((float), requestPriority),
		((std::map<int, float>), secondaryIDs)
	)
);

namespace MPL = ::ecs::MPL;

namespace experiment
{
	// Settings forward-declaration.
	template<typename>
	struct Settings;

	namespace Impl
	{
		namespace PcktIDMode
		{
			struct Automatic { };

			template<typename TPcktIDType, TPcktIDType TID>
			struct Manual : std::integral_constant<TPcktIDType, TID>
			{

			};
		}
	}

	template
	<
		typename TPcktIDType = std::size_t
	>
	struct Settings
	{
		using PcktIDType = TPcktIDType;

		struct PcktIDMode
		{
			using Automatic = Impl::PcktIDMode::Automatic;

			template<PcktIDType TID>
			using Manual = Impl::PcktIDMode::Manual<PcktIDType, TID>;
		};
	};

	template
	<
		typename TPcktType,
		typename TPcktIDMode
	>
	struct PcktBind { };

	template<typename... Ts>
	using PcktBinds = MPL::TypeList<Ts...>;

	template
	<
		typename TSettings,
		typename TPcktBinds
	>
	struct Config
	{
		using Settings = TSettings;
		using PcktBinds = TPcktBinds;

		// static_assert validity of settings
		// static_assert validity of packet binds


	};
}

namespace tests
{
	namespace nle = experiment;

	using MySettings = nle::Settings
	<
		std::size_t
	>;

	using MyPcktBinds = nle::PcktBinds
	<
		nle::PcktBind<int, MySettings::PcktIDMode::Manual<10>>,
		nle::PcktBind<float, MySettings::PcktIDMode::Automatic>
	>;

	using MyConfig = nle::Config
	<
		MySettings,
		MyPcktBinds
	>;
}

template<typename T> T getInput(const std::string& mTitle)
{
	T input;
	std::cin >> input;

	ssvu::lo(mTitle) << input << "\n";
	return input;
}

void choiceServer()
{
	/*ssvu::lo() << "Insert port:\n";
	auto port(getInput<nl::Port>("Port"));
	nl::ManagedSocket server{port};*/

	nl::ManagedHost server{27015};
	int cycles{2};

	while(server.isBusy())
	{
		NL_DEBUGLO() << "bsy";
		if(cycles-- <= 0)
		{
			ssvu::lo("PROCODIO") <<  "\n";
			server.stop();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// ssvu::lo() << "...\n";

		auto processedCount(0u);

		server.process([&processedCount](auto& data, const auto& ip, auto port)
		{
			++processedCount;

			ssvu::lo() << "Received some data from " << ip << ":" << port << "!\n";

			std::string str;
			data >> str;

			ssvu::lo() << "Data: " << str << "\n";
		});

		ssvu::lo() << "Processed packets: " << processedCount << "\n\n";
	}

	NL_DEBUGLO() << "serverend\n";
}

void choiceClient()
{
	/*ssvu::lo() << "Insert port:\n";
	auto port(getInput<nl::Port>("Port"));
	nl::ManagedSocket client{port};*/

	nl::ManagedHost client{27016};

	while(client.isBusy())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		nl::PcktBuf data;
		data << "hello!"s;

		client.send(data, nl::IpAddr::getLocalAddress(), 27015);

		// ssvu::lo() << "...\n";
	}
}

int main()
{
	ssvu::lo("Choose")
		<< "\n"
		<< "0. Server\n"
		<< "1. Client\n"
		<< "_. Exit\n";

	auto choice(getInput<int>("Choice"));

	if(choice == 0)
	{
		choiceServer();
		NL_DEBUGLO() << "end choiceserver";
	}
	else if(choice == 1)
	{
		choiceClient();
	}
	else
	{
		std::terminate();
	}

	NL_DEBUGLO() << "return0 ";
	return 0;
}