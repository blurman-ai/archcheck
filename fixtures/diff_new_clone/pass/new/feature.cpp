int featureToggle(const char *name, int flags)
{
  int score = 0;
  while (*name != '\0')
  {
    score += static_cast<int>(*name) ^ flags;
    flags = (flags << 1) | (score & 1);
    ++name;
  }
  return score & 0xFF;
}
