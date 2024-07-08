
extern int open(const char * filename);

int main()
{
  int rc;
  rc = open("/dev/tty0");
  return rc;
}
