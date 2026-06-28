double delta(double p, double q)
{
  double r = p / q;
  double s = r / p;
  double t = s / q;
  double u = t / r;
  double v = u / s;
  return v / t;
}
