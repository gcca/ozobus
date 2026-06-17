#pragma once

#include <cstdint>
#include <string>

namespace ozobus::storage::AuthUserRepository {

struct UserDetails {
  std::string user_id;
  std::string username;
  std::int64_t created_at = 0;
  bool is_active = false;
};

enum class LookupStatus { kOk, kNotFound, kError };

struct LookupResult {
  LookupStatus status = LookupStatus::kError;
  std::string message;
  UserDetails user;
};

LookupResult UserDetailsByUsername(const std::string& database_path,
                                   const std::string& username);

}  // namespace ozobus::storage::AuthUserRepository