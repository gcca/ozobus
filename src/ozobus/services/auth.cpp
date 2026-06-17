#include "ozobus/services/auth.hpp"

#include "storage/db.hpp"

namespace ozobus::services {

AuthServiceImpl::AuthServiceImpl(std::string database_path)
    : database_path_(std::move(database_path)) {}

grpc::Status AuthServiceImpl::UserDetails(
    grpc::ServerContext* /*context*/,
    const auth::UserDetailsRequest* request,
    auth::UserDetailsResponse* response) {
  try {
    return ozobus::storage::AuthUserRepository::UserDetailsByUsername(
        database_path_, request->username(),
        [&](sqlite3_stmt* stmt) -> grpc::Status {
          const int step = sqlite3_step(stmt);
          if (step == SQLITE_DONE) {
            return {grpc::StatusCode::NOT_FOUND, "user not found"};
          }
          if (step != SQLITE_ROW) {
            return {grpc::StatusCode::INTERNAL,
                    sqlite3_errmsg(sqlite3_db_handle(stmt))};
          }

          response->set_user_id(std::to_string(sqlite3_column_int64(stmt, 0)));
          response->set_username(
              reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
          response->set_is_active(sqlite3_column_int(stmt, 2) != 0);
          response->set_created_at(sqlite3_column_int64(stmt, 3));

          return grpc::Status::OK;
        });
  } catch (const std::runtime_error& error) {
    return {grpc::StatusCode::INTERNAL, error.what()};
  }
}

}  // namespace ozobus::services