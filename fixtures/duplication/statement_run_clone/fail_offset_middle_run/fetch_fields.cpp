// Twin of fetch_users.cpp: a large, otherwise-divergent, partly-renamed function
// that shares ONLY the middle session-setup run. The prologue clears an extra output
// and the epilogue parses fields + components, so the shared run is offset by a
// DIFFERENT amount here than in fetch_users.cpp — flush with neither end.
int fetchFields(const Config& cfg, std::vector<Field>& outFields,
                std::vector<Component>& outComponents, std::string& outError)
{
  outFields.clear();
  outComponents.clear();
  outError.clear();
  if (cfg.apiToken.empty() || cfg.domain.empty())
  {
    outError = "Missing domain or API token.";
    return -1;
  }
  bool expandSchema = cfg.expandSchema;

  // ---- shared middle run (byte-identical with fetch_users.cpp) ----
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

  auto fieldResponse = session.get(base + "/rest/api/3/field");
  if (fieldResponse.status != 200)
  {
    outError = "Field query failed: " + std::to_string(fieldResponse.status);
    return -2;
  }
  outFields = parseFieldCatalog(fieldResponse.body, expandSchema);

  auto componentResponse = session.get(base + "/rest/api/3/component");
  if (componentResponse.status != 200)
  {
    outError = "Component query failed: " + std::to_string(componentResponse.status);
    return -3;
  }
  outComponents = parseComponentList(componentResponse.body);
  for (auto& component : outComponents)
  {
    component.normalize();
  }
  return static_cast<int>(outFields.size() + outComponents.size());
}
