// Same shape as upload_report.cpp but genuinely unrelated code: the setup, the
// request construction and the per-item loop share no verbatim statement run.
int downloadMetrics(const Config& cfg, std::vector<Metric>& outMetrics, std::string& outError)
{
  outMetrics.clear();
  if (cfg.window <= 0)
  {
    outError = "Invalid window.";
    return 0;
  }
  const std::string query = buildMetricQuery(cfg.namespace_, cfg.window);
  auto client = MetricClient::connect(cfg.host, cfg.port);
  client.authenticate(cfg.apiKey);
  Cursor cursor = client.openStream(query);
  int received = 0;
  while (cursor.hasNext())
  {
    Sample sample = cursor.next();
    if (sample.invalid())
    {
      continue;
    }
    Metric metric;
    metric.name = sample.series;
    metric.value = smooth(sample.value, cfg.alpha);
    metric.at = sample.timestamp;
    outMetrics.push_back(metric);
    ++received;
  }
  cursor.close();
  return received;
}
