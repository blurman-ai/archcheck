// A large function whose only overlap with fetch_fields.cpp is a run of statements
// sitting in the MIDDLE of the body — offset from the start by the user-specific
// setup and from the end by the user-specific pagination. Neither #190 (one braced
// block) nor #195 (a run flush with bodyLo/bodyHi) can reach it; only #191 can.
int fetchUsers(const Config& cfg, std::vector<User>& outUsers, std::string& outError)
{
  outUsers.clear();
  outError.clear();
  if (cfg.apiToken.empty() || cfg.domain.empty())
  {
    outError = "Missing domain or API token.";
    return -1;
  }
  int requested = cfg.pageSize > 0 ? cfg.pageSize : 50;
  int startAt = cfg.startAt;
  bool includeInactive = cfg.includeInactive;

  // ---- shared middle run (byte-identical with fetch_fields.cpp) ----
  const std::string base = normalizeBaseUrl(cfg.domain);
  const std::string cred = cfg.email + ":" + cfg.apiToken;
  const std::string authHeader = "Basic " + base64Encode(cred);
  HttpHeaders headers;
  headers.set("Accept", "application/json");
  headers.set("Authorization", authHeader);
  headers.set("User-Agent", "Fixture/1.0 Client");
  RedirectPolicy redirect(true, true);
  Timeout timeout(30, 60);
  Session session(headers, redirect, timeout);
  // ---- end shared middle run ----

  std::string url = base + "/rest/api/3/users/search?maxResults=" + std::to_string(requested);
  int fetched = 0;
  while (true)
  {
    auto response = session.get(url + "&startAt=" + std::to_string(startAt));
    if (response.status != 200)
    {
      outError = "User query failed: " + std::to_string(response.status);
      return -2;
    }
    auto batch = parseUserArray(response.body, includeInactive);
    for (const auto& user : batch)
    {
      outUsers.push_back(user);
      ++fetched;
    }
    if (batch.size() < static_cast<std::size_t>(requested))
    {
      break;
    }
    startAt += requested;
  }
  return fetched;
}
