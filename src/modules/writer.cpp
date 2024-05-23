module;

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

export module writer;

import std;
import log;

export template <typename T>
concept Writable = requires(T t, char *buffer, size_t size) {
  { t.write(buffer, size) } -> std::same_as<void>;
  { t.error() } -> std::same_as<bool>;
};

export class FileWriter {
public:
  FileWriter(const std::string& file_name)
      : m_file(std::make_unique<std::ofstream>(
            file_name, std::ios::out | std::ios::binary)){};

  FileWriter(const FileWriter &) = delete;
  FileWriter& operator=(const FileWriter&) = delete;

  void write(const char *buffer, const size_t size) { m_file->write(buffer, size); }

  bool error() { return m_file && m_file->fail(); }
  ~FileWriter() {
    m_file->flush();
    m_file->close();
  }
private:
  std::unique_ptr<std::ofstream> m_file;
};

static_assert(Writable<FileWriter>);
