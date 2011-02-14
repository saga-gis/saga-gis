
#if !defined(AFX_DEBUGSTREAM_H__2EA05226_A91E_4396_A8DF_8311F9CF2F48__INCLUDED_)
#define AFX_DEBUGSTREAM_H__2EA05226_A91E_4396_A8DF_8311F9CF2F48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <sstream>
//#include <windows.h>

// copyright 1999, James M. Curran
#if defined (_DEBUG)
 using std::ostream;
 class DebugStream : public ostream
 {
 private:
  class DebugStreamBuf : public std::basic_sstreambuf
  {
  protected:
   virtual int sync()
   {
    sputc('\0');
    OutputDebugString(str());

    freeze(false);
    //setp(pbase(), pbase(), epptr());
    setp(pbase(), epptr());
    return 0;
   }
  };

  DebugStreamBuf m_buf;

 public:
  DebugStream() : ostream(&m_buf)
  {}

  ~DebugStream()
  { m_buf.pubsync();}

 };

#else // defined (_DEBUG)

 class DebugStream
 {
 public:
  template <typename T>
  inline const DebugStream& operator<<(T) const
    {return(*this);}

  typedef std::basic_ostream<char>& ( *
endl_type)(std::basic_ostream<char>&);

  inline const DebugStream& operator<<(const endl_type T) const
    {return(*this);}
 } ;

#endif

extern DebugStream dout;




#endif // !defined(AFX_DEBUGSTREAM_H__2EA05226_A91E_4396_A8DF_8311F9CF2F48__INCLUDED_)
