#include "archcheck/scan/flag_argument_scan.h"

#include <array>
#include <cctype>
#include <string>
#include <string_view>
#include <unordered_set>

#include "archcheck/scan/function_body_scan.h"

namespace archcheck::scan
{

namespace
{

using duplication::Token;

// A bool-typed parameter whose name reads like a flag. Prefix match on a word
// boundary so `node`/`normal` (start "no") do NOT trip, but `enableX`/`skip_y` do.
bool flagLikeName(const std::string &name)
{
  static constexpr std::array<std::string_view, 10> kPrefixes = {"enable", "disable", "use",   "force", "skip",
                                                                 "with",   "without", "allow", "is",    "no"};
  std::string low;
  for (char c : name)
    low.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
  for (std::string_view p : kPrefixes)
  {
    if (low.rfind(p, 0) != 0)
      continue;
    if (low.size() == p.size())
      return true; // exactly the prefix word
    const char next = name[p.size()];
    if (next == '_' || (std::isupper(static_cast<unsigned char>(next)) != 0))
      return true; // boundary: snake_ or camelCase
  }
  return false;
}

// +1 for an opening bracket, -1 for a closing one, 0 otherwise. Lets the param
// walk skip anything nested (template args, default-arg parens, init lists).
int bracketDelta(const std::string &s)
{
  if (s == "(" || s == "[" || s == "{" || s == "<")
    return 1;
  if (s == ")" || s == "]" || s == "}" || s == ">")
    return -1;
  return 0;
}

// Bracket depth for walking call arguments: only ()[]{} — NOT <>, which inside a
// call are far more often comparison operators (`f(a < b, true, false)`) than
// template brackets. Counting <> here would unbalance the depth and drop literals.
int callBracketDelta(const std::string &s)
{
  if (s == "(" || s == "[" || s == "{")
    return 1;
  if (s == ")" || s == "]" || s == "}")
    return -1;
  return 0;
}

// One parameter segment [lo, hi): a by-value bool parameter (not bool* / bool&,
// which are out-parameters). Sets `name` to its last identifier.
bool segmentIsBoolFlag(const std::vector<Token> &t, std::size_t lo, std::size_t hi, std::string &name)
{
  bool hasBool = false;
  bool hasPtrRef = false;
  int depth = 0;
  for (std::size_t k = lo; k < hi && k < t.size(); ++k)
  {
    const std::string &s = t[k].sym;
    const int d = bracketDelta(s);
    if (d != 0)
      depth += d;
    else if (depth > 0)
      continue;
    else if (s == "bool")
      hasBool = true;
    else if (s == "*" || s == "&")
      hasPtrRef = true;
    else if (s == "id")
      name = t[k].raw;
  }
  return hasBool && !hasPtrRef;
}

// Count top-level by-value bool parameters in [open+1, close). lastName receives
// the last such parameter's identifier (for the single-flag name check).
int countBoolParams(const std::vector<Token> &t, std::size_t open, std::size_t close, std::string &lastName)
{
  int count = 0;
  int depth = 0;
  std::size_t segStart = open + 1;
  for (std::size_t k = open + 1; k <= close && k < t.size(); ++k)
  {
    const int d = (k == close) ? 0 : bracketDelta(t[k].sym);
    if (d != 0)
    {
      depth += d;
      continue;
    }
    if (k != close && !(depth == 0 && t[k].sym == ","))
      continue;
    std::string name;
    if (segmentIsBoolFlag(t, segStart, k, name))
    {
      ++count;
      lastName = name;
    }
    segStart = k + 1;
  }
  return count;
}

// ARG.1 — flag the signature of each discovered function carrying boolean flag
// parameters (>= 2 bool params, or one bool param with a flag-like name).
void detectFlagSignatures(const std::vector<FunctionSpan> &functions, const std::vector<Token> &tokens,
                          const std::string &file, rules::ViolationList &out)
{
  std::unordered_set<std::string> seen; // dedup declaration vs definition
  for (const auto &fn : functions)
  {
    std::string lastName;
    const int boolParams = countBoolParams(tokens, fn.paramOpen, fn.paramClose, lastName);
    const bool many = boolParams >= 2;
    const bool flagged = many || (boolParams == 1 && flagLikeName(lastName));
    if (!flagged)
      continue;
    const std::string key = fn.qualifiedName + "/" + std::to_string(fn.paramArity);
    if (!seen.insert(key).second)
      continue;
    std::string msg =
        many ? ("function '" + fn.qualifiedName + "' takes " + std::to_string(boolParams) + " boolean flag parameters")
             : ("function '" + fn.qualifiedName + "' takes a boolean flag parameter '" + lastName + "'");
    out.push_back({"ARG.1.flag_argument_signature", file, fn.startLine, std::move(msg)});
  }
}

// Count top-level boolean literals passed inside the call whose '(' is at `open`.
int countCallBoolLiterals(const std::vector<Token> &tokens, std::size_t open)
{
  int depth = 0;
  int boolLiterals = 0;
  for (std::size_t j = open; j < tokens.size(); ++j)
  {
    const int d = callBracketDelta(tokens[j].sym);
    if (d != 0)
    {
      depth += d;
      if (depth == 0)
        break; // matched the call's closing ')'
      continue;
    }
    if (depth == 1 && (tokens[j].sym == "true" || tokens[j].sym == "false"))
      ++boolLiterals;
  }
  return boolLiterals;
}

// ARG.2 — a call site passing >= 2 boolean literals positionally ("f(true,
// false)"): each literal's meaning is opaque at the call, a sibling smell to the
// flag signature. Candidate = identifier directly followed by "(". Control headers
// (if/for/while/switch/catch) tokenize as keywords, never "id", so they are never
// candidates. A function definition's own parameter parens are skipped (its default
// args are ARG.1's territory, not a call).
void detectBooleanLiteralCalls(const std::vector<FunctionSpan> &functions, const std::vector<Token> &tokens,
                               const std::string &file, rules::ViolationList &out)
{
  std::unordered_set<std::size_t> signatureParen;
  for (const auto &fn : functions)
    signatureParen.insert(fn.paramOpen);
  for (std::size_t k = 0; k + 1 < tokens.size(); ++k)
  {
    if (tokens[k].sym != "id" || tokens[k + 1].sym != "(" || signatureParen.count(k + 1) != 0)
      continue;
    const int boolLiterals = countCallBoolLiterals(tokens, k + 1);
    if (boolLiterals >= 2)
      out.push_back({"ARG.2.boolean_literal_call", file, tokens[k].line,
                     "call passes " + std::to_string(boolLiterals) + " boolean literals positionally"});
  }
}

} // namespace

rules::ViolationList detectFlagArguments(const std::vector<Token> &tokens, const std::string &file)
{
  rules::ViolationList out;
  const auto functions = discoverFunctions(tokens);
  detectFlagSignatures(functions, tokens, file, out);
  detectBooleanLiteralCalls(functions, tokens, file, out);
  return out;
}

} // namespace archcheck::scan
