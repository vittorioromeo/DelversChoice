#ifndef EXAMPLE_DDL_DDL_DEFINITION_H
#define EXAMPLE_DDL_DDL_DEFINITION_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/char_sequence.h>

namespace example_ddl
{
  namespace TblUser_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct Username
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "username";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T username;
            T& operator()() { return username; }
            const T& operator()() const { return username; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct PwdHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "pwd_hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T pwdHash;
            T& operator()() { return pwdHash; }
            const T& operator()() const { return pwdHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  }

  struct TblUser: sqlpp::table_t<TblUser,
               TblUser_::Id,
               TblUser_::Username,
               TblUser_::PwdHash>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tbl_user";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tblUser;
        T& operator()() { return tblUser; }
        const T& operator()() const { return tblUser; }
      };
    };
  };
  namespace TblChannel_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct Name
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct IdUser
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id_user";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T idUser;
            T& operator()() { return idUser; }
            const T& operator()() const { return idUser; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct TblChannel: sqlpp::table_t<TblChannel,
               TblChannel_::Id,
               TblChannel_::Name,
               TblChannel_::IdUser>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tbl_channel";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tblChannel;
        T& operator()() { return tblChannel; }
        const T& operator()() const { return tblChannel; }
      };
    };
  };
  namespace TblUserChannel_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct IdUser
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id_user";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T idUser;
            T& operator()() { return idUser; }
            const T& operator()() const { return idUser; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct IdChannel
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id_channel";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T idChannel;
            T& operator()() { return idChannel; }
            const T& operator()() const { return idChannel; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct TblUserChannel: sqlpp::table_t<TblUserChannel,
               TblUserChannel_::Id,
               TblUserChannel_::IdUser,
               TblUserChannel_::IdChannel>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tbl_user_channel";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tblUserChannel;
        T& operator()() { return tblUserChannel; }
        const T& operator()() const { return tblUserChannel; }
      };
    };
  };
  namespace TblMessage_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
    };
    struct Contents
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "contents";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T contents;
            T& operator()() { return contents; }
            const T& operator()() const { return contents; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct IdUser
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id_user";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T idUser;
            T& operator()() { return idUser; }
            const T& operator()() const { return idUser; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct IdChannel
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id_channel";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T idChannel;
            T& operator()() { return idChannel; }
            const T& operator()() const { return idChannel; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct TblMessage: sqlpp::table_t<TblMessage,
               TblMessage_::Id,
               TblMessage_::Contents,
               TblMessage_::IdUser,
               TblMessage_::IdChannel>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tbl_message";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T tblMessage;
        T& operator()() { return tblMessage; }
        const T& operator()() const { return tblMessage; }
      };
    };
  };
}
#endif
