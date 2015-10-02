#include "../include/NetLayer/NetLayer.hpp"
#include "/home/vittorioromeo/OHWorkspace/cppcon2015/Other/Other.hpp"

// TODO:
// * decide abstraction layers
// * decide packet definition and settings definitionsyntax
// * implement an additional layer that abstracts the Architecture module over
// PacketTypes
// * test stuff.
// * acks/reliability stuff.

namespace nl
{
    namespace Impl
    {
        template <typename... TFields>
        struct Pckt
        {
            ssvu::Tpl<TFields...> fields;
        };
    }
}

// Defines getters and setters for a packet field.
#define NL_DEFINE_PCKT_PROXY(mI, mName)                                       \
    inline const auto& mName() const & { return std::get<mI>(this->fields); } \
    inline auto& mName() & { return std::get<mI>(this->fields); }             \
    inline auto mName() && { return std::move(std::get<mI>(this->fields)); }

// Implementation of the preprocessor loop that generates the type list for
// packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL(mIdx, mData, mArg) \
    VRM_PP_TPL_EXPLODE(VRM_PP_TPL_ELEM(mArg, 0)) VRM_PP_COMMA_IF(mIdx)

// Preprocessor loop that generates the type list for packet field types.
#define NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST(...)                                 \
    VRM_PP_FOREACH_REVERSE_NO_ZERO(NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST_FOR_IMPL, \
                                   ~, __VA_ARGS__)



// Implementation of the preprocessor loop that generates the fields inside the
// packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL(mIdx, mData, mArg) \
    NL_DEFINE_PCKT_PROXY(mIdx, VRM_PP_TPL_ELEM(mArg, 1))

// Preprocessor loop that generates the fields inside the packet type.
#define NL_IMPL_DEFINE_PCKT_BODY_LIST(...)                            \
    VRM_PP_FOREACH_NO_ZERO(NL_IMPL_DEFINE_PCKT_BODY_LIST_FOR_IMPL, ~, \
                           __VA_ARGS__)



#define NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)                         \
    struct mName : ::nl::Impl::Pckt<NL_IMPL_DEFINE_PCKT_TEMPLATE_LIST( \
                       VRM_PP_TPL_EXPLODE(mFieldTpls))>                \
    {                                                                  \
        NL_IMPL_DEFINE_PCKT_BODY_LIST(VRM_PP_TPL_EXPLODE(mFieldTpls))  \
    }

#define NL_DEFINE_PCKT(mName, mFieldTpls) NL_IMPL_DEFINE_PCKT(mName, mFieldTpls)

/*
struct AuthRequest : nl::Pckt
<
    int,                // 0
    std::string,        // 1
    float,              // 2
    std::vector<int>    // 3
>
{
    NL_DEFINE_PCKT_PROXY(0, requestID);
    NL_DEFINE_PCKT_PROXY(1, requestUser);
    NL_DEFINE_PCKT_PROXY(2, requestPriority);
    NL_DEFINE_PCKT_PROXY(3, secondaryIDs);
};
*/
NL_DEFINE_PCKT(AuthRequest, (((int), requestID), ((std::string), requestUser),
                             ((float), requestPriority),
                             ((std::map<int, float>), secondaryIDs)));

namespace MPL = ::ecs::MPL;

namespace experiment
{
    // Settings forward-declaration.
    template <typename>
    struct Settings;

    namespace Impl
    {
        namespace PcktIDMode
        {
            struct Tag_Automatic
            {
            };

            struct Tag_Manual
            {
            };

            struct Automatic : Tag_Automatic
            {
            };

            template <typename TPcktIDType, TPcktIDType TID>
            struct Manual : Tag_Manual, std::integral_constant<TPcktIDType, TID>
            {
            };
        }
    }

    template <typename TPcktIDType = std::size_t>
    struct Settings
    {
        using PcktIDType = TPcktIDType;

        struct PcktIDMode
        {
            using Automatic = Impl::PcktIDMode::Automatic;

            template <PcktIDType TID>
            using Manual = Impl::PcktIDMode::Manual<PcktIDType, TID>;
        };
    };

    template <typename TPcktType, typename TPcktIDMode>
    struct PcktBind
    {
        using Type = TPcktType;
        using IDMode = TPcktIDMode;
    };

    template <typename T>
    using PcktBindType = typename T::Type;

    template <typename T>
    using PcktBindIDMode = typename T::IDMode;

    template <typename... Ts>
    using PcktBinds = MPL::TypeList<Ts...>;

    template <typename TList>
    using PcktBindsTypes = MPL::Map<PcktBindType, TList>;

    template <typename TList>
    using PcktBindsIDModes = MPL::Map<PcktBindIDMode, TList>;

    template <typename TSettings, typename TPcktBinds>
    struct Config
    {
        using Settings = TSettings;
        using PcktBinds = TPcktBinds;
        using BindsTypes = PcktBindsTypes<PcktBinds>;
        using BindsIDModes = PcktBindsIDModes<PcktBinds>;

        // static_assert validity of settings
        // static_assert validity of packet binds

        template <typename T>
        static constexpr auto hasPcktBindFor() noexcept
        {
            return MPL::Contains<T, BindsTypes>{};
        }

        template <typename T>
        using PcktBindIDModeFor =
            MPL::Nth<MPL::IndexOf<T, BindsTypes>{}, BindsIDModes>;

        template <typename T>
        static constexpr auto getPcktBindIDImpl(
            typename Impl::PcktIDMode::Tag_Manual) noexcept
        {
            return T{};
        }

        template <typename T>
        static constexpr auto getPcktBindIDImpl(
            typename Impl::PcktIDMode::Tag_Automatic) noexcept
        {
            return MPL::IndexOf<T, BindsTypes>{};
        }

        template <typename T>
        static constexpr auto getPcktBindID() noexcept
        {
            using IDOfT = PcktBindIDModeFor<T>;
            return getPcktBindIDImpl<T>(IDOfT{});
        }

        template <typename T>
        using PcktTypes = MPL::TypeList<int, float, char>; // TODO
    };

    template <typename TConfig, typename T>
    auto toPayload(const T& mX)
    {
        static_assert(TConfig::template hasPcktBindFor<T>(), "");
        constexpr auto id(TConfig::template getPcktBindID<T>());

        nl::Impl::Payload p;
        p << id;

        using PcktTypes = typename TConfig::template PcktTypes<T>;
        MPL::forTypes<PcktTypes>([&p, &mX](auto t)
                                 {
                                     p << std::get<ECS_TYPE(t)>(mX.fields);
                                 });

        return p;
    }

    // TODO: dispatch

    template <typename TConfig, typename T>
    T fromPayload(nl::Impl::Payload& mP)
    {
        // TODO: assumes id has been read
        // typename TConfig::Settings::PcktIDType id;
        // mP >> id;

        T result;

        using PcktTypes = typename TConfig::template PcktTypes<T>;
        MPL::forTypes<PcktTypes>([&mP, &result](auto t)
                                 {
                                     mP >> std::get<ECS_TYPE(t)>(result.fields);
                                 });
    }

    // structure(...) and (...)destructure methods
    // payload -> packet
    // packet -> payload
}

namespace tests
{
    namespace nle = experiment;

    using MySettings = nle::Settings<std::size_t>;

    using MyPcktBinds =
        nle::PcktBinds<nle::PcktBind<int, MySettings::PcktIDMode::Manual<10>>,
                       nle::PcktBind<float, MySettings::PcktIDMode::Automatic>>;

    using MyConfig = nle::Config<MySettings, MyPcktBinds>;
}

template <typename T>
T getInput(const std::string& mTitle)
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

    auto processedCount(0u);

    auto fnProcess(
        [&processedCount](auto& server, auto& data, const auto& sender)
        {
            ++processedCount;

            ssvu::lo() << "Received some data from " << sender << "!\n";

            std::string str;
            data >> str;

            ssvu::lo() << "Data: " << str << "\n";

            server.send(sender, "I got your message!"s);
        });

    nl::ManagedHost<decltype(fnProcess)> server{27015, fnProcess};
    // auto server(nl::mkManagedHost(27015, fnProcess));
    // auto server(nl::makeManagedHost(27015, fnProcess));

    int cycles{20};

    while(server.isBusy()) {

        if(getInput<int>("Exit? (1)")) break;

        // NL_DEBUGLO() << "bsy";
        if(cycles-- <= 0) {
            // server.stop();
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // ssvu::lo() << "...\n";

        // if(processedCount > 0)
        //    ssvu::lo() << "Processed packets: " << processedCount << "\n\n";
    }

    NL_DEBUGLO() << "serverend\n";
}

void choiceClient()
{
    /*ssvu::lo() << "Insert port:\n";
    auto port(getInput<nl::Port>("Port"));
    nl::ManagedSocket client{port};*/

    auto fnProcess([](auto&, auto& data, const auto&)
                   {
                       std::string str;
                       data >> str;

                       ssvu::lo() << "Reply: " << str << "\n";
                   });

    nl::ManagedHost<decltype(fnProcess)> client{27016, fnProcess};

    while(client.isBusy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));



        nl::Impl::PayloadTarget myself(nl::IpAddr::getLocalAddress(), 27015);
        client.send(myself, "hello!"s);



        // ssvu::lo() << "...\n";
    }
}

namespace example
{
    NL_DEFINE_PCKT(RegistrationRequest,
                   (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT(RegistrationResponse, (((bool), valid)));

    NL_DEFINE_PCKT(LoginRequest,
                   (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT(LoginResponse, (((bool), valid)));

    namespace nle = experiment;
    using MySettings = nle::Settings<std::size_t>;
    using MyPcktIDMode = MySettings::PcktIDMode;

    namespace CtoS
    {
        using MyPcktBinds = nle::PcktBinds<
            nle::PcktBind<RegistrationRequest, MyPcktIDMode::Automatic>,
            nle::PcktBind<LoginRequest, MyPcktIDMode::Automatic>>;

        using MyConfig = nle::Config<MySettings, MyPcktBinds>;

        static_assert(MyConfig::getPcktBindID<RegistrationRequest>() == 0, "");
        static_assert(MyConfig::getPcktBindID<LoginRequest>() == 1, "");
    }

    // TODO:
    // * Both server and client need to know the packet types for sending/receiving
    // * Automatic IDs need to auto-increment only for server and client separately
    // * Need two "compile-time" auto-increment id counters, one for server, one for client
    // * Or just use a single one? (could support peer to peer, or something)

    namespace StoC
    {
        using MyPcktBinds = nle::PcktBinds<
            nle::PcktBind<RegistrationResponse, MyPcktIDMode::Automatic>,
            nle::PcktBind<LoginResponse, MyPcktIDMode::Automatic>>;

        using MyConfig = nle::Config<MySettings, MyPcktBinds>;

        static_assert(MyConfig::getPcktBindID<RegistrationResponse>() == 0, "");
        static_assert(MyConfig::getPcktBindID<LoginResponse>() == 1, "");
    }
}

int main()
{
    ssvu::lo("Choose") << "\n"
                       << "0. Server\n"
                       << "1. Client\n"
                       << "_. Exit\n";

    auto choice(getInput<int>("Choice"));

    if(choice == 0) {
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
