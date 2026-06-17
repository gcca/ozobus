#include "ozobus/services/auth.hpp"

namespace ozobus::services {

AuthServiceImpl::AuthServiceImpl(std::string database_path)
    : repository_(std::move(database_path)) {}

grpc::Status AuthServiceImpl::UserDetails(
    grpc::ServerContext* /*context*/,
    const auth::UserDetailsRequest* request,
    auth::UserDetailsResponse* response) {
  auto stmt = repository_.UserDetailsByUsername(request->username());

  const int step = sqlite3_step(stmt.get());
  if (step == SQLITE_DONE) {
    return {grpc::StatusCode::NOT_FOUND, "user not found"};
  }
  if (step != SQLITE_ROW) {
    return {grpc::StatusCode::INTERNAL, sqlite3_errmsg(sqlite3_db_handle(stmt.get()))};
  }

  response->set_user_id(std::to_string(sqlite3_column_int64(stmt.get(), 0)));
  response->set_username(
      reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1)));
  response->set_is_active(sqlite3_column_int(stmt.get(), 2) != 0);
  response->set_created_at(sqlite3_column_int64(stmt.get(), 3));

  return grpc::Status::OK;
}

}  // namespace ozobus::services