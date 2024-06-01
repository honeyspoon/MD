module;

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>

export module reader;
import std;
import mlog;

export template <typename T>
concept Readable = requires(T t, std::byte *buffer, size_t size) {
  { t.read(buffer, size) } -> std::same_as<void>;
  { bool(t) } -> std::same_as<bool>;
  { t.eof() } -> std::same_as<bool>;
  { t.error() } -> std::same_as<bool>;
};

export class StreamReader {
 public:
  StreamReader(std::istream &stream) : m_stream(stream), m_eof(false) {}

  void read(std::byte *buffer, size_t size) {
    m_stream.read(reinterpret_cast<char *>(buffer), size);
    m_eof = m_stream.eof();
  }

  bool eof() {
    return m_eof;
  }

  bool error() {
    return m_stream.fail();
  }

  operator bool() {
    return !eof() and !error();
  }

 private:
  std::istream &m_stream;
  bool m_eof;
};

static_assert(Readable<StreamReader>);

export class FileReader {
 public:
  FileReader(const std::string file_name)
      : m_file(std::make_unique<std::ifstream>(
            file_name, std::ios::in | std::ios::binary)){};

  FileReader(const FileReader &) = delete;

  void read(std::byte *buffer, size_t size) {
    m_file->read(std::bit_cast<char *>(buffer), size);
  }

  bool eof() {
    return m_file && m_file->eof();
  }

  bool error() {
    return m_file && m_file->fail();
  }

  operator bool() {
    return !eof() and !error();
  }

 private:
  std::unique_ptr<std::ifstream> m_file;
};

static_assert(Readable<FileReader>);

export class CMappedFileReader {
 public:
  CMappedFileReader(const std::string file_name)
      : m_fd(open(file_name.c_str(), O_RDONLY)),
        m_offset(0),
        m_eof(false),
        m_file_size(0),
        m_file_data(nullptr) {
    struct stat st;
    if (fstat(m_fd, &st) == -1) {
      close(m_fd);
      return;
    }
    m_file_size = st.st_size;

    m_file_data = static_cast<uint8_t *>(
        mmap(NULL, m_file_size, PROT_READ, MAP_PRIVATE, m_fd, 0));
    if (m_file_data == MAP_FAILED) {
      close(m_fd);
      m_file_data = nullptr;
      return;
    }
  }

  CMappedFileReader(const CMappedFileReader &) = delete;
  CMappedFileReader operator=(const CMappedFileReader &) = delete;
  CMappedFileReader(CMappedFileReader &&) = delete;
  CMappedFileReader operator=(CMappedFileReader &&) = delete;

  void read(std::byte *buffer, size_t size) {
    auto size_left = static_cast<size_t>(m_file_size - m_offset);
    if (size > size_left) {
      m_eof = true;
      size = size_left;
    }
    std::memcpy(buffer, m_file_data + m_offset, size);
    m_offset += size;
  }

  bool eof() {
    return m_eof;
  }

  bool error() {
    return false;
  }

  operator bool() {
    return !eof() and !error();
  }

  ~CMappedFileReader() {
    close(m_fd);
  }

 private:
  int m_fd;
  off_t m_offset;
  bool m_eof;
  off_t m_file_size;
  uint8_t *m_file_data;
};

static_assert(Readable<CMappedFileReader>);

export class CFileReader {
 public:
  CFileReader(std::string file_name) {
    m_file = std::fopen(file_name.c_str(), "r");
  }

  CFileReader(const CFileReader &) = delete;
  CFileReader operator=(const CFileReader &) = delete;
  CFileReader(CFileReader &&) = delete;
  CFileReader operator=(CFileReader &&) = delete;

  void read(std::byte *buffer, size_t size) {
    std::fread(buffer, size, 1, m_file);
  }

  bool eof() {
    return std::feof(m_file);
  }

  bool error() {
    return std::ferror(m_file);
  }

  operator bool() {
    return !eof() and !error();
  }

  ~CFileReader() {
    std::fclose(m_file);
  }

 private:
  std::FILE *m_file;
  size_t m_gcount;
};

static_assert(Readable<CFileReader>);
