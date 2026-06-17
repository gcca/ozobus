#pragma once

#include "auth.grpc.pb.h"

namespace ozobus::services {

class AuthServiceImpl final : public auth::AuthService::Service {
 public:
  grpc::Status UserDetails(grpc::ServerContext* context,
                           const auth::UserDetailsRequest* request,
                           auth::UserDetailsResponse* response) override;
};

}  // namespace ozobus::services
