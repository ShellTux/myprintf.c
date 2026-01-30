#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#else
#include <unistd.h>
#endif

// int64_t sum(const int64_t a, const int64_t b, const int64_t c) {
//   return a + b + c;
// }

int64_t sum_(const size_t count, ...) {
  int64_t result = 0;

  va_list args;
  va_start(args, count);
  {
    for (size_t i = 0; i < count; ++i) {
      result += va_arg(args, int64_t);
    }
  }
  va_end(args);

  return result;
}

#define sum(...)                                                               \
  sum_(sizeof((const int64_t[]){__VA_ARGS__}) /                                \
           sizeof(*(const int64_t[]){__VA_ARGS__}),                            \
       __VA_ARGS__)

size_t int64_len(int64_t n) {
  size_t len = n < 0 ? 1 : 0;

  if (n < 0) {
    n *= -1;
  }

  // TODO: deal with sign
  do {
    n /= 10;
    len += 1;
  } while (n > 0);

  return len;
}

struct Template {
  enum {
    TEMPLATE_STRING,
    TEMPLATE_SIGNED_NUMBER,
    TEMPLATE_ARRAY,
  } kind;
  union {
    char *string;
    int64_t signed_number;
    struct CArray {
      void *items;
      size_t count;
    } array;
  };
  union {
    size_t (*string)(char *buf, const char *src, const size_t count);
    size_t (*signed_number)(char *buf, const int64_t signed_number,
                            const size_t count);
    size_t (*array)(char *buf, const struct CArray array, const size_t count);
  } write_function;
  size_t char_count;
};

size_t write_template(char *b, const char specifier,
                      const struct Template *template) {
  switch (specifier) {

  case 's': {
    assert(template->kind == TEMPLATE_STRING);
    strncpy(b, template->string, template->char_count);
  } break;

  case 'i': {
    assert(template->kind == TEMPLATE_SIGNED_NUMBER);
    int64_t n = template->signed_number;
    if (n < 0) {
      b[0] = '-';
      n *= -1;
    }

    for (int64_t i = template->char_count - 1; i >= 0 && n > 0; --i) {
      b[i] = '0' + (n % 10);
      n /= 10;
    }

  } break;

  default: {
    assert(0 && "Unknown template specifier");
  } break;
  }
  return template->char_count;
}

int64_t mydprintf(const int fd, const char *restrict format, ...) {
  int64_t count = 0;

  struct Template *templates = NULL;
  size_t templates_count = 0;
  for (char *f = (char *)format; *f != '\0'; ++f) {
    if (*f != '%') {
      continue;
    }

    switch (f[1]) {
    case 's':
    case 'i':
      templates_count += 1;
      break;
    default: {
      assert(0 && "Unknown template specifier");
    } break;
    }
  }

  templates = calloc(templates_count, sizeof(*templates));
  size_t template_index = 0;

  va_list args;
  va_start(args, format);
  for (char *f = (char *)format; *f; ++f) {
    if (*f != '%') {
      count += 1;
      continue;
    }

    struct Template *template = &templates[template_index];

    ++f;
    switch (*f) {
    case 's': {
      char *string_arg = va_arg(args, char *);
      *template = (struct Template){
          .char_count = strlen(string_arg),
          .kind = TEMPLATE_STRING,
          .string = string_arg,
      };
      template_index++;
      count += template->char_count;
    } break;
    case 'i': {
      int64_t int_arg = va_arg(args, int64_t);
      *template = (struct Template){
          .char_count = int64_len(int_arg),
          .kind = TEMPLATE_SIGNED_NUMBER,
          .signed_number = int_arg,
      };
      template_index++;
      count += template->char_count;
    } break;
    default: {
      assert(0 && "Unknown template specifier");
    } break;
    }
  }
  va_end(args);

  char *buf = calloc(count, sizeof(*buf));
  template_index = 0;
  for (char *b = buf, *f = (char *)format; *f; ++f, ++b) {
    if (*f != '%') {
      *b = *f;
      continue;
    }

    const struct Template template = templates[template_index++];

    ++f;
    b += write_template(b, *f, &template) - 1;
  }

#ifdef _WIN32
#error "Implement write"
#else
  count = write(fd, buf, count);
#endif

  free(buf);
  buf = NULL;

  free(templates);
  templates = NULL;

  return count;
}

#define myprintf(...) mydprintf(STDOUT_FILENO, __VA_ARGS__)

int main(void) {
  const char string[] = "Hello world!\n";
  // assert(write(STDOUT_FILENO, string, sizeof(string) - 1) != 0);

  assert(myprintf("Hello world!\n") == 13);
  assert(myprintf("Foo %s\n", "bar") == 8);
  assert(myprintf("%s %s %s\n", "foo", "bar", "baz") == 12);
  assert(myprintf("sum = %i\n", sum(1, 2, 3, 4, 5, 6)) == 9);
  assert(myprintf("%i + %i + %i + %i + %i = %i\n", 1, 2, 3, 4, 5,
                  sum(1, 2, 3, 4, 5)) == 23);

  mydprintf(STDERR_FILENO, "%i\n", (int64_t)-10348);
  myprintf("{i32} + {int} = {i32}\n", 34, 35, 69);

  myprintf("{i32:<5 :=3}\n", 32);
  myprintf("{i32:>10 :=3}\n", 32);

  myprintf("{string;=*}\n", string, string + 6);
  myprintf("{string;=?}\n", string, 5);
  myprintf("{string;=5}\n", string);

  const int a[] = {1, 2, 3};
  myprintf("{array[int]}\n", 3, a);
  myprintf("{[int]}\n", 3, a);

  printf("%zu\n", 3ul);
  return 0;
}
