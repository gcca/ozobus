#include "storage/db.hpp"

#include <sqlite3.h>

#include <string>

namespace ozobus::storage::AuthUserRepository {

namespace {

LookupResult MakeError(std::string message) {
  return {LookupStatus::kError, std::move(message), {}};
}

}  // namespace

LookupResult UserDetailsByUsername(const std::string& database_path,
                                   const std::string& username) {
  sqlite3* db = nullptr;
  if (sqlite3_open(database_path.c_str(), &db) != SQLITE_OK) {
    const std::string message =
        db != nullptr ? sqlite3_errmsg(db) : "failed to open database";
    if (db != nullptr) {
      sqlite3_close(db);
    }
    return MakeError(message);
  }

  const char* sql =
      "SELECT id, username, is_active, strftime('%s', created_at) "
      "FROM auth_user WHERE username = ? LIMIT 1";

  sqlite3_stmt* stmt = nullptr;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    const std::string message = sqlite3_errmsg(db);
    sqlite3_close(db);
    return MakeError(message);
  }

  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

  const int step = sqlite3_step(stmt);
  if (step == SQLITE_DONE) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return {LookupStatus::kNotFound, "user not found", {}};
  }
  if (step != SQLITE_ROW) {
    const std::string message = sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return MakeError(message);
  }

  UserDetails user;
  user.user_id = std::to_string(sqlite3_column_int64(stmt, 0));
  user.username =
      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
  user.is_active = sqlite3_column_int(stmt, 2) != 0;
  user.created_at = sqlite3_column_int64(stmt, 3);

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return {LookupStatus::kOk, {}, std::move(user)};
}

}  // namespace ozobus::storage::AuthUserRepository