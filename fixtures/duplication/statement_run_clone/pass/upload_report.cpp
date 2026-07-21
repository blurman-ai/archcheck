// Same problem domain and rough shape as download_metrics.cpp (setup -> request ->
// process -> return) but NO run of copied statements: every middle line differs in
// call, argument and order. Statement-run slicing must not manufacture a clone out
// of two functions that merely share a domain vocabulary.
int uploadReport(const Config& cfg, const Report& report, std::string& outError)
{
  outError.clear();
  if (report.rows.empty())
  {
    outError = "Nothing to upload.";
    return 0;
  }
  const std::string endpoint = cfg.host + "/v2/reports";
  auto payload = serializeReport(report, cfg.compact);
  Signer signer(cfg.secret);
  const std::string signature = signer.sign(payload, cfg.nonce);
  UploadOptions options;
  options.chunkBytes = cfg.chunkBytes;
  options.parallelism = cfg.parallelism;
  options.checksum = crc32(payload);
  auto uploader = makeUploader(endpoint, signature, options);
  int accepted = 0;
  for (auto& chunk : uploader.split(payload))
  {
    auto ack = uploader.send(chunk);
    if (!ack.ok)
    {
      outError = "Chunk rejected at offset " + std::to_string(chunk.offset);
      return -1;
    }
    accepted += ack.rows;
  }
  return accepted;
}
