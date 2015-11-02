#include "../include/NetLayer/NetLayer.hpp"
#include "./utils/test_utils.hpp"
#include <string>

int main()
{
    NL_DEFINE_PCKT(
        RegistrationRequest, (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT_1(RegistrationResponse, ((bool), valid));

    NL_DEFINE_PCKT(
        LoginRequest, (((std::string), user), ((std::string), pass)));

    NL_DEFINE_PCKT_1(LoginResponse, ((bool), valid));

    static_assert(std::is_base_of<nl::Impl::Pckt<std::string, std::string>,
                      RegistrationRequest>{},
        "");

    static_assert(
        std::is_base_of<nl::Impl::Pckt<bool>, RegistrationResponse>{}, "");

    static_assert(std::is_base_of<nl::Impl::Pckt<std::string, std::string>,
                      LoginRequest>{},
        "");

    static_assert(std::is_base_of<nl::Impl::Pckt<bool>, LoginResponse>{}, "");

    {
        LoginResponse lr{nl::init_fields{}, false};
        TEST_ASSERT_OP(lr.valid(), ==, false);

        lr.valid() = true;
        TEST_ASSERT_OP(lr.valid(), ==, true);
    }

    {
        LoginResponse lr{nl::init_fields{}, true};
        TEST_ASSERT_OP(lr.valid(), ==, true);

        lr.valid() = false;
        TEST_ASSERT_OP(lr.valid(), ==, false);

        lr = nl::make_pckt<LoginResponse>(true);
        TEST_ASSERT_OP(lr.valid(), ==, true);

        auto lr_rhs = nl::make_pckt<LoginResponse>(false);
        lr = lr_rhs;
        TEST_ASSERT_OP(lr.valid(), ==, false);
        TEST_ASSERT_OP(lr, ==, lr_rhs);
    }

    {
        auto lr(nl::make_pckt<LoginResponse>(false));
        TEST_ASSERT_OP(lr.valid(), ==, false);
    }

    {
        auto lr(nl::make_pckt<LoginResponse>(true));
        TEST_ASSERT_OP(lr.valid(), ==, true);
    }

    {
        auto lr(nl::make_pckt<LoginResponse>(true));
        TEST_ASSERT_OP(lr.valid(), ==, true);

        auto pl(nl::make_payload(nl::PAddress{"127.0.0.1", 27015}, lr));

        LoginResponse t;
        pl.data >> t;

        TEST_ASSERT_OP(t.valid(), ==, true);
    }

    {
        auto lr(nl::make_pckt<LoginResponse>(true));
        TEST_ASSERT_OP(lr.valid(), ==, true);

        auto pl(nl::make_payload(nl::PAddress{"127.0.0.1", 27015}, lr));

        auto t(nl::make_deserialized<LoginResponse>(pl.data));
        TEST_ASSERT_OP(t.valid(), ==, true);
    }

    return 0;
}
