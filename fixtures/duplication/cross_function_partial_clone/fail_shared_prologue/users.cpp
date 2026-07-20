int fetchUsers(int requestId)
{
  int status = 0;
  int page = requestId;
  int limit = 100;
  int retry = 0;
  int cursor = 0;
  status += prepare_client(page);
  status += attach_auth(limit);
  status += add_header(retry);
  status += add_timeout(cursor);
  status += validate_request(page, limit);
  status += send_query(cursor);
  status += parse_status(retry);
  status += collect_headers(limit);
  status += map_error(status);
  status += finish_request(cursor);

  int users = 0;
  users += decode_user_name(status);
  users += decode_user_email(page);
  users += decode_user_groups(limit);
  users += decode_user_flags(retry);
  return users + status;
}
