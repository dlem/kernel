#pragma once

// visible (default console) output
bool putchar(const char c);
int kprintf(const char *pszFmt, ...);
bool cls();

// SAFE print-to-string functions
KRESULT sprintfn(char *buffer, int bufferSize, const char *format, ...);
KRESULT strcpyn(char *dst, int bufferSz, const char *src);
KRESULT strcatn(char *buffer, int bufferSize, const char *str);
KRESULT itoan(int iVal, char *psz, int iBase, int iSz, int *nWritten);

// UNSAFE print-to-string functions
int sprintf(char *buffer, const char *format, ...);
int strcpy(char *dst, const char *src);
int strcat(char *dst, const char *src);
char *itoa(int iVal, char *psz, int iBase, int iSz);

int strlen(const char *str);
char *strdup(const char *str);

void memcpy(void *buf, const void *src, size_t sz);
void memset(void *buf, char val, size_t sz);
void memzero(void *buf, size_t sz);

// loosely based on c++ STL vectors
template<typename T> class KVector {
  void *m_pData;                                // void due to bool specialization
  size_t m_size;
  size_t m_allocated;

public:
  class Reference {};
  class ConstReference {};
  KVector();
  ~KVector();
  KRESULT Copy(const KVector<bool> &other);
  size_t Size() const;
  size_t MaxSize() const;                     // max due to library limitation
  KRESULT Resize(size_t sz, T c=T());         // exact resize
  bool Empty() const;
  KRESULT Reserve(size_t sz);                 // "at least" resize
  Reference operator[](size_t n);             // without bounds checking
  ConstReference operator[](size_t n) const;  // without bounds checking
  Reference At(size_t n);                     // with bounds checking; setting is ignored if out of bounds
  ConstReference At(size_t n) const;
  Reference Front();
  ConstReference Front() const;
  Reference Back();
  ConstReference Back() const;
  KRESULT Assign(size_t n, const T &t);
  KRESULT PushBack(const T &t);
  void PopBack();                             // delete the last element
  void Clear();

private:
  DECLARE_NOCOPY(KVector);
};


