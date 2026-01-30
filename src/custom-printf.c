#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int custom_printf(const char *format, ...);
int custom_fprintf(FILE *stream, const char *format, ...);
int custom_dprintf(int fd, const char *format, ...);
int custom_sprintf(char *str, const char *format, ...);
int custom_snprintf(size_t size, char str[size], const char *format, ...);
int custom_vprintf(const char *format, va_list ap);
int custom_vfprintf(FILE *stream, const char *format, va_list ap);
int custom_vdprintf(int fd, const char *format, va_list ap);
int custom_vsprintf(char *str, const char *format, va_list ap);
int custom_vsnprintf(size_t size, char str[size], const char *format,
                     va_list ap);

int main() {
  // Example usage of each function
  char buffer[100];

  // Using custom_print
  custom_printf("Hello %s! You've used %d%% of your quota.\n", "User", 75);

  // Using custom_fprint to print to a file
  FILE *file = fopen("output.txt", "w");
  if (file) {
    custom_fprintf(file, "Written to file: %s finished with code %d.\n", "Task",
                   0);
    fclose(file);
  } else {
    custom_printf("Failed to open file for writing.\n");
  }

  // Using custom_dprint to write to a file descriptor (standard output)
  custom_dprintf(STDOUT_FILENO, "Using custom_dprint: %s\n",
                 "Hello, File Descriptor!");

  // Using custom_sprint to format a string into a buffer
  custom_sprintf(buffer, "Buffer content: %s with code %d\n", "Data", 42);
  custom_printf("%s", buffer); // Print the buffer content

  // Using custom_snprint to safely format a string with size limit
  custom_snprintf(sizeof(buffer), buffer, "Safe Print: %s, count = %d\n",
                  "Items", 5);
  custom_printf("%s", buffer); // Print the safe formatted buffer content
}

int custom_printf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = custom_vprintf(format, ap);
  va_end(ap);
  return result;
}

int custom_fprintf(FILE *stream, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = custom_vfprintf(stream, format, ap);
  va_end(ap);
  return result;
}

int custom_dprintf(int fd, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = custom_vdprintf(fd, format, ap);
  va_end(ap);
  return result;
}

int custom_sprintf(char *str, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = custom_vsprintf(str, format, ap);
  va_end(ap);
  return result;
}

int custom_snprintf(size_t size, char str[size], const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int result = custom_vsnprintf(size, str, format, ap);
  va_end(ap);
  return result;
}

int custom_vprintf(const char *format, va_list ap) {
  return vprintf(format, ap); // Using standard vprintf for simplicity
}

int custom_vfprintf(FILE *stream, const char *format, va_list ap) {
  return vfprintf(stream, format, ap); // Using standard vfprintf for simplicity
}

int custom_vdprintf(int fd, const char *format, va_list ap) {
  // Using vsnprintf to format into a buffer
  char buffer[1024];
  int written = vsnprintf(buffer, sizeof(buffer), format, ap);
  return write(fd, buffer, written); // Write to file descriptor
}

int custom_vsprintf(char *str, const char *format, va_list ap) {
  return vsprintf(str, format, ap); // Using standard vsprintf for simplicity
}

int custom_vsnprintf(size_t size, char str[size], const char *format,
                     va_list ap) {
  return vsnprintf(str, size, format,
                   ap); // Using standard vsnprintf for simplicity
}
