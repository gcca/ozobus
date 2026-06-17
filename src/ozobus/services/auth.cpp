#include "ozobus/services/auth.hpp"

#include "storage/db.hpp"

namespace ozobus::services {

AuthServiceImpl::AuthServiceImpl(std::string database_path)
    : database_path_(std::move(database_path)) {}

grpc::Status AuthServiceImpl::UserDetails(
    grpc::ServerContext* /*context*/,
    const auth::UserDetailsRequest* request,
    auth::UserDetailsResponse* response) {
  const auto result =
      ozobus::storage::AuthUserRepository::UserDetailsByUsername(
          database_path_, request->username());

  switch (result.status) {
    case ozobus::storage::AuthUserRepository::LookupStatus::kOk:
      response->set_user_id(result.user.user_id);
      response->set_username(result.user.username);
      response->set_is_active(result.user.is_active);
      response->set_created_at(result.user.created_at);
      return grpc::Status::OK;
    case ozobus::storage::AuthUserRepository::LookupStatus::kNotFound:
      return {grpc::StatusCode::NOT_FOUND, result.message};
    case ozobus::storage::AuthUserRepository::LookupStatus::kError:
      return {grpc::StatusCode::INTERNAL, result.message};
  }

  return {grpc::StatusCode::INTERNAL, "unexpected lookup status"};
}

}  // namespace ozobus::services