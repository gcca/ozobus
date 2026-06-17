#include "ozobus/services/auth.hpp"

#include <sqlite3.h>

#include <string>

namespace ozobus::services {

AuthServiceImpl::AuthServiceImpl(std::string database_path)
    : database_path_(std::move(database_path)) {}

grpc::Status AuthServiceImpl::UserDetails(
    grpc::ServerContext* /*context*/,
    const auth::UserDetailsRequest* request,
    auth::UserDetailsResponse* response) {
  sqlite3* db = nullptr;
  if (sqlite3_open(database_path_.c_str(), &db) != SQLITE_OK) {
    const std::string message =
        db != nullptr ? sqlite3_errmsg(db) : "failed to open database";
    if (db != nullptr) {
      sqlite3_close(db);
    }
    return {grpc::StatusCode::INTERNAL, message};
  }

  const char* sql =
      "SELECT id, username, is_active, strftime('%s', created_at) "
      "FROM auth_user WHERE username = ? LIMIT 1";

  sqlite3_stmt* stmt = nullptr;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    const std::string message = sqlite3_errmsg(db);
    sqlite3_close(db);
    return {grpc::StatusCode::INTERNAL, message};
  }

  sqlite3_bind_text(stmt, 1, request->username().c_str(), -1, SQLITE_TRANSIENT);

  const int step = sqlite3_step(stmt);
  if (step == SQLITE_DONE) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return {grpc::StatusCode::NOT_FOUND, "user not found"};
  }
  if (step != SQLITE_ROW) {
    const std::string message = sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return {grpc::StatusCode::INTERNAL, message};
  }

  response->set_user_id(std::to_string(sqlite3_column_int64(stmt, 0)));
  response->set_username(
      reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
  response->set_is_active(sqlite3_column_int(stmt, 2) != 0);
  response->set_created_at(sqlite3_column_int64(stmt, 3));

  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return grpc::Status::OK;
}

}  // namespace ozobus::services