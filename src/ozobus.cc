#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

namespace {

std::string ServerAddress(int argc, char** argv) {
  if (argc > 1) {
    return argv[1];
  }

  const char* port = std::getenv("PORT");
  if (port != nullptr && port[0] != '\0') {
    return std::string("0.0.0.0:") + port;
  }

  return "0.0.0.0:50051";
}

}  // namespace

int main(int argc, char** argv) {
  const std::string server_address = ServerAddress(argc, argv);

  grpc::EnableDefaultHealthCheckService(true);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if (server == nullptr) {
    std::cerr << "Failed to start ozobus on " << server_address << '\n';
    return 1;
  }

  grpc::HealthCheckServiceInterface* health_service =
      server->GetHealthCheckService();
  if (health_service != nullptr) {
    health_service->SetServingStatus(true);
  }

  std::cout << "ozobus listening on " << server_address << '\n';
  server->Wait();

  return 0;
}
