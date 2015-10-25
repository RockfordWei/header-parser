#pragma once

#include "tokenizer.h"
#include "options.h"
#include <string>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

enum class ScopeType
{
  kGlobal,
  kNamespace,
  kClass
};

enum class AccessControlType 
{
  kPublic,
  kPrivate,
  kProtected
};

class Parser : private Tokenizer
{
public:
  Parser(const Options& options);
  virtual ~Parser();

  // No copying of parser
  Parser(const Parser& other) = delete;
  Parser(Parser&& other) = delete;

  // Parses the given input
  bool Parse(const char* input);

  /// Returns the result of a previous parse
  std::string result() const { return std::string(buffer_.GetString(), buffer_.GetString() + buffer_.GetSize()); }

protected:
  /// Called to parse the next statement. Returns false if there are no more statements.
  bool ParseStatement();
  bool ParseDeclaration(Token &token);
  void ParseDirective();
  bool SkipDeclaration(Token &token);
  void ParseEnum(Token &token);
  void ParseMacroMeta();
  void ParseMetaSequence();

  void PushScope(const std::string& name, ScopeType scopeType, AccessControlType accessControlType);
  void PopScope();

  void ParseNamespace();
  bool ParseAccessControl(const Token& token, AccessControlType& type);

  AccessControlType current_access_control_type() const { return topScope_->currentAccessControlType; }
  void WriteCurrentAccessControlType();

  void WriteAccessControlType(AccessControlType type);
  void ParseClass(Token &token);
  void ParseFunction(Token &token);
  void ParseType();

  std::string ParseTypename();

  void WriteToken(const Token &token);
  void ParseCustomMacro(Token & token, const std::string& macroName);
private:
  Options options_;
  rapidjson::StringBuffer buffer_;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer_;

  struct Scope
  {
    ScopeType type;
    std::string name;
    AccessControlType currentAccessControlType;
  };

  Scope scopes_[64];
  Scope *topScope_;
};


