#include <cstdlib>
#include <cstring>
#include <cstdio>

int stack_ptr;
long stack[100];

long Pop() {
  long value = stack[stack_ptr];
  stack_ptr--;
  return value;
}

void Push(long value) {
  stack_ptr++;
  stack[stack_ptr] = value;
}

extern "C" void SyscallExit(int exit_code);

extern "C" void main(int argc, char** argv) {

  stack_ptr = -1;
  long c;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "+") == 0) {
      long b = Pop();
      long a = Pop();
      c = a + b;
    } else if (strcmp(argv[i], "-") == 0) {
      long b = Pop();
      long a = Pop();
      c = a - b;
    } else {
      c = atol(argv[i]);
    }

    Push(c);
  }

  long result = 0;
  if (stack_ptr >= 0) {
    result = Pop();
  }

  printf("%ld\n", result);
  SyscallExit(static_cast<int>(result));
}
