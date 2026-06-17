#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <CLI11.hpp>
#include <grpcpp/grpcpp.h>

#include "ozobus/services/auth.hpp"

namespace {

std::string ServerAddress(std::uint16_t port) {
  return "0.0.0.0:" + std::to_string(port);
}

static void SetCLIArgs(CLI::App& app, std::uint16_t& port) {
  port = 50051;
  app.add_option("-p,--port", port, "Port to listen on")
      ->check(CLI::Range(1, 65535))
      ->capture_default_str();
}

}  // namespace

int main(int argc, char* argv[]) {
  CLI::App app{"ozobus"};
  std::uint16_t port;
  SetCLIArgs(app, port);

  CLI11_PARSE(app, argc, argv);

  const std::string server_address = ServerAddress(port);

  grpc::EnableDefaultHealthCheckService(true);

  ozobus::services::AuthServiceImpl auth_service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&auth_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  if (server == nullptr) {
    std::cerr << "Failed to start ozobus on " << server_address << '\n';
    return EXIT_FAILURE;
  }

  grpc::HealthCheckServiceInterface* health_service =
      server->GetHealthCheckService();
  if (health_service != nullptr) {
    health_service->SetServingStatus(true);
  }

  std::cout << "ozobus listening on " << server_address << '\n';
  server->Wait();

  return EXIT_SUCCESS;
}
