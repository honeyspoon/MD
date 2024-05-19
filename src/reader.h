
#pragma once

#include <fstream>
#include <print>

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

template <typename T> class Reader {
public:
  Reader *Derived() { return (static_cast<T *>(this)); }

  void read(char *buffer, size_t size) {
    std::println("sssssss");
    (static_cast<T *>(this)->read(buffer, size));
  }
  bool eof() {
    std::println("eof papaa {}", (static_cast<T *>(this)->eof()));
    return (static_cast<T *>(this)->eof());
  }
  bool error() { return Derived()->error(); }
  void print_error() { static_cast<T *>(this)->print_error(); }
  ~Reader() { std::println("reader dtor"); }
};

class FileReader : public Reader<FileReader> {
public:
  FileReader(const std::string file_name)
      : m_file(std::make_unique<std::ifstream>(
            file_name, std::ios::in | std::ios::binary)){};

  FileReader(const FileReader &) = delete;

  void read(char *buffer, size_t size) {
    // std::println("dd", size);
    m_file->read(buffer, size);
  }

  bool eof() {
    std::println("eof fils {}", m_file->eof());
    return m_file && m_file->eof();
  }

  bool error() { return m_file && m_file->fail(); }
  size_t gcount() { return m_file ? m_file->gcount() : 0; }
  void print_error() {
    std::ios_base::iostate state = m_file->rdstate();

    if (state & std::ios_base::eofbit) {
      std::println("End of file reached.");
    }
    if (state & std::ios_base::failbit) {
      std::println("Non-fatal I/O error occurred.");
    }
    if (state & std::ios_base::badbit) {
      std::println("Fatal I/O error occurred.");
    }

    std::perror("Error: ");
  }

private:
  std::unique_ptr<std::ifstream> m_file;
};

class CMappedFileReader : public Reader<CMappedFileReader> {
public:
  CMappedFileReader(const std::string file_name)
      : m_fd(open(file_name.c_str(), O_RDONLY)), m_offset(0), m_eof(false),
        m_file_size(0), m_file_data(nullptr) {
    struct stat st;
    if (fstat(m_fd, &st) == -1) {
      perror("fstat");
      close(m_fd);
      return;
    }
    m_file_size = st.st_size;

    m_file_data = static_cast<char *>(
        mmap(NULL, m_file_size, PROT_READ, MAP_PRIVATE, m_fd, 0));
    if (m_file_data == MAP_FAILED) {
      perror("mmap");
      close(m_fd);
      m_file_data = nullptr;
      return;
    }
  }

  CMappedFileReader(const CMappedFileReader &) = delete;

  void read(char *buffer, size_t size) {
    if (m_offset + size > m_file_size) {
      m_eof = true;
      size = m_file_size - m_offset;
    }
    std::memcpy(buffer, m_file_data + m_offset, size);
    m_offset += size;
  }

  bool eof() { return m_eof; }

  bool error() { return false; }

  void print_error() {}

  ~CMappedFileReader() { close(m_fd); }

private:
  int m_fd;
  size_t m_offset;
  bool m_eof;
  size_t m_file_size;
  char *m_file_data;
};

class CFileReader : public Reader<CFileReader> {
public:
  CFileReader(std::string file_name) { m_file = fopen(file_name.c_str(), "r"); }

  CFileReader(const CFileReader &) = delete;

  void read(char *buffer, size_t size) { fread(buffer, size, 1, m_file); }

  bool eof() { return feof(m_file); }

  bool error() { return ferror(m_file); }
  size_t gcount() { return m_file ? m_gcount : 0; }

  void print_error() {}

  ~CFileReader() { fclose(m_file); }

private:
  FILE *m_file;
  int m_gcount;
};

template <typename T>
std::unique_ptr<Reader<T>> create_reader(std::string file_name) {
  return std::make_unique<T>(file_name);
}
