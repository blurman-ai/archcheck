int refreshCache(int key)
{
  int status = 0;
  int cachePage = key;
  int cacheLimit = 10;
  status += prepare_cache_client(cachePage);
  status += attach_cache_auth(cacheLimit);
  status += load_cache_key(key);
  status += validate_cache_entry(cachePage);
  status += write_cache_record(cacheLimit);
  status += publish_cache_event(status);
  return status;
}
