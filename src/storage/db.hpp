#pragma once

#include <mutex>
#include <string>

#include <sqlite3.h>

namespace ozobus::storage::AuthUserRepository {

class Repository {
 public:
  explicit Repository(std::string database_path);
  ~Repository();

  Repository(const Repository&) = delete;
  Repository& operator=(const Repository&) = delete;

  class UserDetailsStatement {
   public:
    UserDetailsStatement(UserDetailsStatement&& other) noexcept;
    UserDetailsStatement& operator=(UserDetailsStatement&& other) noexcept;
    ~UserDetailsStatement();

    sqlite3_stmt* get() const { return stmt_; }
    operator sqlite3_stmt*() const { return stmt_; }

   private:
    friend class Repository;

    UserDetailsStatement(std::unique_lock<std::mutex> lock, sqlite3_stmt* stmt);

    std::unique_lock<std::mutex> lock_;
    sqlite3_stmt* stmt_ = nullptr;
  };

  UserDetailsStatement UserDetailsByUsername(const std::string& username);

 private:
  sqlite3* db_ = nullptr;
  sqlite3_stmt* user_details_by_username_stmt_ = nullptr;
  std::mutex mutex_;
};

}  // namespace ozobus::storage::AuthUserRepository