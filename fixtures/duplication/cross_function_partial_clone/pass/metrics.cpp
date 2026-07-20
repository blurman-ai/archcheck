int refreshMetrics(int key)
{
  int status = 0;
  int metricsPage = key;
  int metricsLimit = 10;
  status += prepare_metrics_client(metricsPage);
  status += attach_metrics_auth(metricsLimit);
  status += load_metric_key(key);
  status += validate_metric_entry(metricsPage);
  status += write_metric_record(metricsLimit);
  status += publish_metric_event(status);
  return status;
}
