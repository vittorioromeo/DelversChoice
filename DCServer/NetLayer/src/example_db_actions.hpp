#pragma once

#include "../include/NetLayer/NetLayer.hpp"

namespace db_actions
{
    using namespace example;
    using namespace example_ddl;

    namespace impl
    {
        template <typename T, typename TF>
        bool execute_if_not_empty(T&& r, TF&& f)
        {
            if(r.empty()) return false;

            f(r.front());
            return true;
        }
    }

    template <typename TF>
    void for_channels(TF&& f)
    {
        auto result(
            db()(select(all_of(tbl_channel)).from(tbl_channel).where(true)));

        for(const auto& row : result) f(row);
    }

    template <typename TF>
    bool message_by_id(int id, TF&& f)
    {
        auto result(db()(select(all_of(tbl_message))
                             .from(tbl_message)
                             .where(tbl_message.id == id)));

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool user_by_id(int id, TF&& f)
    {
        auto result(db()(
            select(all_of(tbl_user)).from(tbl_user).where(tbl_user.id == id)));

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool user_by_username(const std::string& username, TF&& f)
    {
        auto result(db()(select(all_of(tbl_user))
                             .from(tbl_user)
                             .where(tbl_user.username == username)));

        return impl::execute_if_not_empty(result, f);
    }

    bool has_user_by_id(int id)
    {
        return user_by_id(id, [](const auto&)
            {
            });
    }

    bool has_user_by_username(const std::string& username)
    {
        return user_by_username(username, [](const auto&)
            {
            });
    }

    template <typename TF>
    bool channel_by_id(int id, TF&& f)
    {
        auto result(db()(select(all_of(tbl_channel))
                             .from(tbl_channel)
                             .where(tbl_channel.id == id)));

        return impl::execute_if_not_empty(result, f);
    }

    template <typename TF>
    bool channel_by_name(const std::string& name, TF&& f)
    {
        auto result(db()(select(all_of(tbl_channel))
                             .from(tbl_channel)
                             .where(tbl_channel.name == name)));

        return impl::execute_if_not_empty(result, f);
    }

    bool has_channel_by_id(int id)
    {
        return channel_by_id(id, [](const auto&)
            {
            });
    }

    bool has_channel_by_name(const std::string& name)
    {
        return channel_by_name(name, [](const auto&)
            {
            });
    }

    auto create_user(const std::string& user, const std::string& pass)
    {
        return db()(insert_into(tbl_user).set(tbl_user.username = user,
            tbl_user.pwdHash = utils::hash_pwd(pass)));
    }

    auto create_channel(int user_id, const std::string& name)
    {
        return db()(
            insert_into(tbl_channel)
                .set(tbl_channel.idUser = user_id, tbl_channel.name = name));
    }

    auto delete_channel(int id)
    {
        assert(has_channel_by_id(id));
        return db()(remove_from(tbl_channel).where(tbl_channel.id == id));
    }

    auto add_user_to_channel(int user_id, int channel_id)
    {
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(insert_into(tbl_user_channel)
                        .set(tbl_user_channel.idUser = user_id,
                            tbl_user_channel.idChannel = channel_id));
    }

    auto remove_user_from_channel(int user_id, int channel_id)
    {
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(remove_from(tbl_user_channel)
                        .where(tbl_user_channel.idUser == user_id &&
                               tbl_user_channel.idChannel == channel_id));
    }

    auto create_message(
        int user_id, int channel_id, const std::string& contents)
    {
        assert(has_channel_by_id(channel_id));
        assert(has_user_by_id(user_id));

        return db()(insert_into(tbl_message)
                        .set(tbl_message.idUser = user_id,
                            tbl_message.idChannel = channel_id,
                            tbl_message.contents = contents));
    }

    auto is_user_in_channel(int user_id, int channel_id)
    {
        auto result(db()(select(all_of(tbl_user_channel))
                             .from(tbl_user_channel)
                             .where(tbl_user_channel.idUser == user_id &&
                                    tbl_user_channel.idChannel == channel_id)));

        return !result.empty();
    }

    template <typename TF>
    void for_users_subscribed_to(int channel_id, TF&& f)
    {
        if(!has_channel_by_id(channel_id)) return;

        auto result(db()(select(all_of(tbl_user_channel))
                             .from(tbl_user_channel)
                             .where(tbl_user_channel.idChannel == channel_id)));

        for(const auto& row : result) f(row.idUser);
    }
}
