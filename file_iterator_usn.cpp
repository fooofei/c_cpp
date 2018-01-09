
#include "file_iterator_usn.h"


#include <Windows.h>
#include <map>
#include <vector>
#include <list>
#include "strings_algorithm.h"
#include "ensurecleanup.h"

struct base::file_iterator_usn_t::file_iterator_usn_impl_t
{
  /* use the addr- sizeof(uint16_t) to store size, reference the OLECHAR */
  struct name_t
  {
    wchar_t * addr;

    name_t() {
      clear();
    }
    ~name_t() {
      clear();
    }

    void clear() {
      std::memset(this, 0, sizeof(*this));
    }

    /* the struct USN_RECORD's FileNameLength is WORD type, so the size is uint16_t */
    uint16_t size() const {
      return *(((uint16_t*)addr) - 1);
    }
    const wchar_t * c_str() const {
      return addr;
    }

    int push(const wchar_t * arg0, uint16_t arg1) {
      if (arg1 == size()) {
        std::wmemcpy(addr, arg0, arg1);
        addr[arg1] = 0;
        return 0;
      }
      return -1;
    }


  };

  struct pool_t
  {
    uint8_t * pool_data_;
    uint8_t * end_;
    uint8_t * cursor_;

    pool_t() :pool_data_(0) {}
    ~pool_t() {
      clear();
    }
    void clear()
    {
      if (pool_data_) {
        free(pool_data_);
      }
      end_ = 0;
      pool_data_ = 0;
      cursor_ = 0;
    }

    /* about 300M size */
    int create_pool(size_t c)
    {
      clear();
      pool_data_ = (uint8_t*)malloc(c);
      if (!pool_data_) {
        return -1;
      }
      std::memset(pool_data_, 0, c);
      cursor_ = pool_data_;
      end_ = pool_data_ + c;
      return 0;
    }

    /* outer give size(wchar_t count , not bytes size) */
    int alloc_name_size(name_t * pv, uint16_t size)
    {
      size_t need_size;
      need_size = (size + 1) * sizeof(wchar_t) + sizeof(uint16_t);
      if (need_size <= size_t(end_ - cursor_))
      {
        std::memset(cursor_, 0, need_size);
        uint16_t * pu16 = (uint16_t *)cursor_;
        *pu16 = size;
        pv->addr = (wchar_t*)(pu16 + 1);
        cursor_ += need_size;
        return 0;
      }
      return -1;
    }

  };

  typedef std::map<DWORDLONG, name_t>	namemap_t;
  typedef std::map<DWORDLONG, DWORDLONG> frnmap_t;

  std::wstring value_;
  namemap_t namemap_;
  frnmap_t frnmap_;
  pool_t memory_pool_;

  // make full path
  namemap_t::const_iterator iter_cursor_;
  std::wstring drive_;

  void clear()
  {
    value_.clear();
    namemap_.clear();
    memory_pool_.clear();
    frnmap_.clear();
    iter_cursor_ = namemap_.end();
    drive_.clear();
  }

  int set_drive(const std::wstring & drive1)
  {
    BOOL b;
    std::wstring drive(drive1);
    int err = -1;

    /* only accept c c: c:\ */
    if (drive.size() == 1) {
      drive.append(L":");
    }
    if (drive.size() == 2) {
      drive.append(L"\\");
    }
    if (drive.size() > 3) {
      drive.erase(3);
    }

    clear();

    for (;;)
    {
      std::wstring temp;
      std::wstring temp2;
      std::wstring volumename;
      base::ensure_close_file_t hvolume;
      DWORD BytesReturned = 0;

      temp.resize(12);

      /* volume MUST be NTFS, not others (FAT and so on )*/
      b = GetVolumeInformationW(drive.c_str(), NULL, 0,
        NULL, NULL, NULL, &temp[0], (DWORD)temp.size());
      temp2.assign(temp.c_str());
      if (!base::samewith(temp2, L"NTFS")) {
        break;
      }

      volumename.assign(L"\\\\.\\");
      volumename.append(drive.c_str(), 2);

      /* There MUST run as Admin */
      hvolume.close();
      hvolume = CreateFileW(volumename.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, // MUST have FILE_SHARE_WRITE
        NULL,
        OPEN_EXISTING, // MUST is OPEN_EXISTING, not CREATE_ALWAYS
        FILE_ATTRIBUTE_READONLY, // not be FILE_ATTRIBUTE_NORMAL
        NULL
      );

      if (hvolume.is_invalid()) {
        break;
      }

      BytesReturned = 0;
      NTFS_VOLUME_DATA_BUFFER ntfsVolData;
      b = DeviceIoControl(hvolume, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0,
        &ntfsVolData, sizeof(ntfsVolData), &BytesReturned, NULL);
      if (!b) {
        break;
      }


      /* test the total_file_count is a little larger than <iterator_cycle_count> x 2
      so we can use this size to allocate a large memory
      https://stackoverflow.com/questions/5572171/getting-number-of-files-in-a-partition
      */
      const uint32_t every_name_max_size = 0xFF;
      LONGLONG total_file_count = (ntfsVolData.MftValidDataLength.QuadPart / ntfsVolData.BytesPerFileRecordSegment);
      if (total_file_count > SIZE_MAX || total_file_count * every_name_max_size > SIZE_MAX) {
        /* out of memory */
        break;
      }
      err = memory_pool_.create_pool(size_t(total_file_count * every_name_max_size));
      if (err) {
        break;
      }

      /* init usn
       msdn:http://msdn.microsoft.com/en-us/library/aa364558%28v=VS.85%29.aspx
      */
      /* not use */
//       CREATE_USN_JOURNAL_DATA cujd;
//       std::memset(&cujd, 0, sizeof(cujd));
//       b = DeviceIoControl(hvolume, FSCTL_CREATE_USN_JOURNAL, &cujd,
//         sizeof(cujd), NULL, 0, &BytesReturned, NULL);
//       if (!b) break;


      // msdn:http://msdn.microsoft.com/en-us/library/aa364583%28v=VS.85%29.aspx
      USN_JOURNAL_DATA usn;
      b = DeviceIoControl(hvolume, FSCTL_QUERY_USN_JOURNAL, NULL,
        0, &usn, sizeof(USN_JOURNAL_DATA), &BytesReturned, NULL);
      if (!b) break;


      // msdn:http://msdn.microsoft.com/en-us/library/aa364563%28v=VS.85%29.aspx
      MFT_ENUM_DATA med;
      const DWORD kbuffer_size = 4096;
      std::vector<uint8_t> buf;
      DWORD dwUsnDataSize;

      // temp values
      DWORD dwRetBytes;
      const USN_RECORD *  pusn_record;
      const uint8_t * pcursor;
      DWORD max_name_size = 0;

      std::memset(&med, 0, sizeof(med));
      med.HighUsn = usn.NextUsn;

      for (;;)
      {
        dwUsnDataSize = 0;
        buf.clear();
        buf.resize(kbuffer_size);
        b = DeviceIoControl(hvolume, FSCTL_ENUM_USN_DATA, &med, sizeof(med),
          &buf[0], kbuffer_size, &dwUsnDataSize, NULL);
        if (!b) {
          break;
        }
        if (dwUsnDataSize <= sizeof(USN)) {
          break;
        }
        if (dwUsnDataSize > buf.size()) {
          break;
        }

        /* skip one usn, the first usn is the next query begin off*/
        dwRetBytes = dwUsnDataSize - sizeof(USN);
        pusn_record = (const USN_RECORD *)(&buf[0] + sizeof(USN));
        pcursor = (const uint8_t *)(pusn_record);

        for (;;)
        {
          if (dwRetBytes < sizeof(USN_RECORD)) {
            break;
          }

          if (pusn_record->FileNameOffset >= dwRetBytes) {
            break;
          }
          if (pusn_record->FileNameLength > dwRetBytes - pusn_record->FileNameOffset) {
            break;
          }
          if (pusn_record->RecordLength > dwRetBytes) {
            break;
          }

          //Ref:http://blog.csdn.net/xexiyong/article/details/17151573
          //Ref:http://www.microsoft.com/msj/1099/journal2/journal2.aspx

          /* not fullpath name */
          const wchar_t * p_name = (const wchar_t *)(pcursor + pusn_record->FileNameOffset);
          /* in bytes size, not terminaing by 0 */
          uint16_t p_name_size = uint16_t(pusn_record->FileNameLength / sizeof(wchar_t));

          //max_name_size = std::max<DWORD>(p_name_size, max_name_size);
          name_t name;
          name.clear();
          err = memory_pool_.alloc_name_size(&name, p_name_size);
          if (err) {
            break;
          }
          err = name.push(p_name, p_name_size);
          if (err) {
            break;
          }

          namemap_[pusn_record->FileReferenceNumber] = name;
          frnmap_[pusn_record->FileReferenceNumber] = pusn_record->ParentFileReferenceNumber;
          dwRetBytes -= pusn_record->RecordLength;

          // 2014_05_03 try to get the file Real path butfailed .
          //GetPathFROMFRN(hVol,pUsnRecord->FileReferenceNumber,fileVolPath,MAX_PATH*2);

          pusn_record = (const USN_RECORD *)(pcursor + pusn_record->RecordLength);
          pcursor = (const uint8_t *)pusn_record;
        }
        med.StartFileReferenceNumber = *((USN*)(&buf[0]));
      }


      /* this will effect software Everything to rebuild cache. */
      // msdn:http://msdn.microsoft.com/en-us/library/aa364561%28v=VS.85%29.aspx
//       DELETE_USN_JOURNAL_DATA dujd;
//       dujd.DeleteFlags = USN_DELETE_FLAG_DELETE;
//       dujd.UsnJournalID = usn.UsnJournalID;
//       b = DeviceIoControl(hvolume, FSCTL_DELETE_USN_JOURNAL,
//         &dujd, sizeof(dujd),
//         NULL, 0, &BytesReturned, NULL);
//       if (!b) break;

      drive_ = drive;
      iter_cursor_ = namemap_.begin();
      err = 0;
      break;
    }

    return err;
  }

  int next()
  {
    DWORDLONG currrent;
    DWORDLONG parent;

    std::list<name_t> v;

    if (iter_cursor_ != namemap_.end())
    {
      currrent = iter_cursor_->first;
      v.push_front(iter_cursor_->second);


      frnmap_t::const_iterator frn_it;
      namemap_t::const_iterator name_it;

      for (;;)
      {
        frn_it = frnmap_.find(currrent);
        if (frn_it == frnmap_.end()) {
          break;
        }
        parent = frn_it->second;
        name_it = namemap_.find(parent);
        if (name_it == namemap_.end()) {
          break;
        }
        currrent = name_it->first;
        v.push_front(name_it->second);
      }

      std::wstring temp;
      temp.append(drive_);
      base::removebackslash(temp);
      for (std::list<name_t>::const_iterator it = v.begin(); it != v.end(); ++it)
      {
        temp.append(L"\\", 1);
        temp.append(it->c_str(), it->size());
      }

      value_ = temp;
      iter_cursor_++;
      return 0;
    }


    return -1;
  }

  const std::wstring & value() const
  {
    return value_;
  }
};


NAMESPACE_BASE_BEGIN

file_iterator_usn_t::file_iterator_usn_t()
{
  p_ = 0;
  p_ = new(std::nothrow)file_iterator_usn_impl_t();
  p_->clear();
}

file_iterator_usn_t::~file_iterator_usn_t()
{
  p_->clear();
  delete p_;
  p_ = 0;
}

void file_iterator_usn_t::clear()
{
  p_->clear();
}


int
file_iterator_usn_t::set_drive(const std::wstring & s)
{
  return p_->set_drive(s);
}

int
file_iterator_usn_t::next()
{
  return p_->next();
}

const std::wstring &
file_iterator_usn_t::value() const
{
  return p_->value();
}

int
file_iterator_usn_t::create()
{
  if (!p_) {
    p_ = new(std::nothrow)file_iterator_usn_impl_t();
    if (!p_) {
      return -1;
    }
    p_->clear();
  }
  return 0;
}

void
file_iterator_usn_t::release()
{
  if (p_) {
    p_->clear();
    delete p_;
  }
  
  p_ = 0;
}

NAMESPACE_END;


int 
main()
{
    return 0;
}
