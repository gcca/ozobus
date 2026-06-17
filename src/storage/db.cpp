#include "storage/db.hpp"

#include <stdexcept>
#include <utility>

namespace ozobus::storage::AuthUserRepository {

namespace {

void ThrowOnOpenError(sqlite3* db, const std::string& database_path) {
  const std::string message =
      db != nullptr ? sqlite3_errmsg(db) : "failed to open database";
  if (db != nullptr) {
    sqlite3_close(db);
  }
  throw std::runtime_error("failed to open database " + database_path + ": " +
                           message);
}

void ThrowOnPrepareError(sqlite3* db, const char* sql) {
  throw std::runtime_error(std::string("failed to prepare query: ") + sql +
                           ": " + sqlite3_errmsg(db));
}

}  // namespace

Repository::UserDetailsStatement::UserDetailsStatement(
    std::unique_lock<std::mutex> lock, sqlite3_stmt* stmt)
    : lock_(std::move(lock)), stmt_(stmt) {}

Repository::UserDetailsStatement::UserDetailsStatement(
    UserDetailsStatement&& other) noexcept
    : lock_(std::move(other.lock_)), stmt_(other.stmt_) {
  other.stmt_ = nullptr;
}

Repository::UserDetailsStatement& Repository::UserDetailsStatement::operator=(
    UserDetailsStatement&& other) noexcept {
  if (this != &other) {
    lock_ = std::move(other.lock_);
    stmt_ = other.stmt_;
    other.stmt_ = nullptr;
  }
  return *this;
}

Repository::UserDetailsStatement::~UserDetailsStatement() = default;

Repository::Repository(std::string database_path) {
  if (sqlite3_open_v2(database_path.c_str(), &db_,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX,
                      nullptr) != SQLITE_OK) {
    ThrowOnOpenError(db_, database_path);
  }

  const char* sql =
      "SELECT id, username, is_active, strftime('%s', created_at) "
      "FROM auth_user WHERE username = ? LIMIT 1";

  if (sqlite3_prepare_v2(db_, sql, -1, &user_details_by_username_stmt_,
                         nullptr) != SQLITE_OK) {
    ThrowOnPrepareError(db_, sql);
  }
}

Repository::~Repository() {
  if (user_details_by_username_stmt_ != nullptr) {
    sqlite3_finalize(user_details_by_username_stmt_);
  }
  if (db_ != nullptr) {
    sqlite3_close(db_);
  }
}

Repository::UserDetailsStatement Repository::UserDetailsByUsername(
    const std::string& username) {
  std::unique_lock<std::mutex> lock(mutex_);

  sqlite3_reset(user_details_by_username_stmt_);
  sqlite3_clear_bindings(user_details_by_username_stmt_);
  sqlite3_bind_text(user_details_by_username_stmt_, 1, username.c_str(), -1,
                    SQLITE_TRANSIENT);

  return UserDetailsStatement(std::move(lock), user_details_by_username_stmt_);
}

}  // namespace ozobus::storage::AuthUserRepository