#pragma once

#include <string>

#include "auth.grpc.pb.h"
#include "storage/db.hpp"

namespace ozobus::services {

class AuthServiceImpl final : public auth::AuthService::Service {
 public:
  explicit AuthServiceImpl(std::string database_path);

  grpc::Status UserDetails(grpc::ServerContext* context,
                           const auth::UserDetailsRequest* request,
                           auth::UserDetailsResponse* response) override;

 private:
  ozobus::storage::AuthUserRepository::Repository repository_;
};

}  // namespace ozobus::services