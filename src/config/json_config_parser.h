// Create: 2023-02-09
// Author: CasinoHe
// Purpose: a json config parser
#pragma once

#include "log/logger.h"
#ifdef USE_BOOST_JSON_PARSER
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#endif
#ifdef USE_RAPIDJSON
#include <rapidjson/document.h>
#endif
#include <string>
#include <memory>
#include <type_traits>

namespace multiplayer_server
{
  class JsonConfigParser
  {
  public:
    JsonConfigParser(const std::string &config_file_path);
    ~JsonConfigParser();

    // Parse config file
    [[nodiscard]] bool parse();

    // Get config value
    template <typename T>
    T get(const std::string &key, const T &default_value) const 
    {
#ifdef USE_BOOST_JSON_PARSER
      return config_tree_.get<T>(key, default_value);
#endif
#ifdef USE_RAPIDJSON
      // if has the key, return the value, otherwise return the default value
      if (config_tree_.HasMember(key.c_str()))
      {
        // get rapidjson::Value
        auto &value = config_tree_[key.c_str()];

        // because of if constexpr, compiler will optimize the code
        // every instance of this function will only have one if-else branch
        if constexpr (std::is_same_v<T, int>)
        {
          if (value.IsInt())
          {
            return value.GetInt();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not int");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, unsigned int>)
        {
          if (value.IsUint())
          {
            return value.GetUint();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not unsigned int");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, int64_t>)
        {
          if (value.IsInt64())
          {
            return value.GetInt64();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not int64_t");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, uint64_t>)
        {
          if (value.IsUint64())
          {
            return value.GetUint64();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not uint64_t");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, double>)
        {
          if (value.IsDouble())
          {
            return value.GetDouble();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not double");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, float>)
        {
          if (value.IsFloat())
          {
            return value.GetFloat();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not float");
            return default_value;
          }
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
          if (value.IsBool())
          {
            return value.GetBool();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not bool");
            return default_value;
          }
        }
        // std::string is a special case, because of std::string is not a primitive type
        // first, remove const and reference, then check if it is std::string
        else if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>)
        {
          if (value.IsString())
          {
            return value.GetString();
          }
          else
          {
            logger_->error("JsonConfigParser::get() {} failed, error: {}", key, "value is not string");
            return default_value;
          }
        }
        else
        {
          // static_assert and find out what's the type of T
          static_assert(false, "JsonConfigParser::get() failed, error: unsupported type");
        }
      }
      else
      {
        return default_value;
      }
#endif
    }

  private:
    std::string config_file_path_;
#ifdef USE_BOOST_JSON_PARSER
    boost::property_tree::ptree config_tree_;
#endif
#ifdef USE_RAPIDJSON
    rapidjson::Document config_tree_;
#endif
    std::shared_ptr<LoggerImp> logger_;
  };
}