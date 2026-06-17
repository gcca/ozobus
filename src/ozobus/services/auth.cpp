#include "ozobus/services/auth.hpp"

namespace ozobus::services {

grpc::Status AuthServiceImpl::UserDetails(
    grpc::ServerContext* /*context*/,
    const auth::UserDetailsRequest* request,
    auth::UserDetailsResponse* response) {

  response->set_user_id("usr-001");
  response->set_username(request->username());
  response->set_email(request->username() + "@example.com");
  response->add_roles("admin");
  response->add_roles("viewer");
  response->set_created_at(1718582400);  // 2024-06-17 00:00:00 UTC
  response->set_is_active(true);

  return grpc::Status::OK;
}

}  // namespace ozobus::services
