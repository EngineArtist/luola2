#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <minizip/unzip.h>

#include "datafile.h"
#include "paths.h"

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

//! Base class for DataSource implementations
class DataSourceImpl {
    public:
        virtual ~DataSourceImpl();

        virtual std::streamsize read(char *s, std::streamsize n) = 0;

        virtual bool isError() const = 0;
        virtual string errorString() const = 0;
};

DataSourceImpl::~DataSourceImpl() {}

//! Base class for data file implementations
class DataFileImpl {
public:
    DataFileImpl(fs::path path)
        : m_path(path), m_reserved(false)
    {
    }

    virtual ~DataFileImpl() {}
    virtual bool isError() const = 0;
    virtual string errorString() const = 0;
    virtual DataSourceImpl *getSource(const string& resource) = 0;

    string name() const
    {
        return m_path.native();
    }

    bool reserve()
    {
        if(m_reserved)
            return false;
        m_reserved = true;
        return true;
    }

    void release()
    {
        m_reserved = false;
    }

protected:
    const fs::path m_path;

private:
    bool m_reserved;
};

//! A data source that wraps a file
class DataSourceFile : public DataSourceImpl {
    public:
        DataSourceFile(const fs::path& path)
            : in_(path.native(), std::ifstream::in)
        {
        }

        bool isError() const
        {
            return !error_.empty();
        }

        string errorString() const
        {
            return error_;
        }

        std::streamsize read(char *s, std::streamsize n)
        {
            in_.read(s, n);
            if(in_.bad()) {
                error_ = "read error";
                return -1;
            } else if(in_.eof()) {
                std::streamsize count = in_.gcount();
                if(count==0)
                    return -1;
                return count;
            }
            return n;
        }

    private:
        std::ifstream in_;
        string error_;
};

static string zipErrorToString(int error)
{
    std::stringstream ss;
    switch(error) {
        case UNZ_OK: ss << "no error"; break;
        case UNZ_BADZIPFILE: ss << "bad zip file"; break;
        case UNZ_INTERNALERROR: ss << "minizip internal error"; break;
        case UNZ_CRCERROR: ss << "zip CRC error"; break;
        case 1: ss << "unable to open zip file"; break;
        case 2: ss << "zip entry already open"; break;
        default: ss << "unknown minizip error #" << error; break;
    }
    return ss.str();
}

//! A data source for zip file entries
class DataSourceZip : public DataSourceImpl {
    public:
        DataSourceZip(DataFileImpl *df, unzFile zip, const string& filename)
            : df_(df), zip_(zip)
        {
            bool reserved = df->reserve();
            assert(reserved);
            if(!reserved) {
                error_ = 2;
                zip_ = nullptr;
            } else {
                error_ = unzLocateFile(zip, filename.c_str(), 1);
                if(error_ == UNZ_OK) {
                    unz_file_info info;
                    error_ = unzGetCurrentFileInfo(zip, &info, 0, 0, 0, 0, 0, 0);
                    if(error_ != UNZ_OK) {
                        zip_ = nullptr;
                        df_->release();
                    } else {
                        len_ = info.uncompressed_size;
                        error_ = unzOpenCurrentFile(zip_);
                        if(error_ != UNZ_OK) {
                            zip_ = nullptr;
                            df_->release();
                        }
                    }
                }
            }
        }

        ~DataSourceZip()
        {
            if(zip_) {
                unzCloseCurrentFile(zip_);
                df_->release();
            }
        }

        bool isError() const
        {
            return error_ != UNZ_OK;
        }

        string errorString() const
        {
            return zipErrorToString(error_);
        }

        std::streamsize read(char *s, std::streamsize n)
        {
            int read = unzReadCurrentFile(zip_, s, n);
            if(read > 0)
                len_ -= read;
            return read;
        }

    private:
        DataFileImpl *df_;
        unzFile zip_;
        int error_;
        unsigned int len_;
};

//! Directory based data file implementation
class DataFileDir : public DataFileImpl {
public:
    DataFileDir(const fs::path& path)
        : DataFileImpl(path)
    {
    }

    DataSourceImpl *getSource(const string& resource)
    {
        return new DataSourceFile(m_path / resource);
    }

    bool isError() const
    {
        return false;
    }

    string errorString() const
    {
        return "";
    }
};

//! ZIP file based data file implementation
class DataFileZip : public DataFileImpl {
public:
    DataFileZip(const fs::path& path)
        : DataFileImpl(path), m_error(0)
    {
        m_zip = unzOpen(path.c_str());
        if(!m_zip)
            m_error = 1;
    }

    ~DataFileZip()
    {
        unzClose(m_zip);
    }

    DataSourceImpl *getSource(const string& source)
    {
        return new DataSourceZip(this, m_zip, source);
    }

    bool isError() const
    {
        return m_error != UNZ_OK;
    }

    string errorString() const
    {
        return zipErrorToString(m_error);
    }

    bool inuse;

private:
    unzFile m_zip;
    int m_error;
};

DataFile::DataFile(const string& name)
{
    fs::path path = Paths::get().findDataFile(name);
    if(fs::exists(path)) {
        if(is_directory(path)) {
            p_ = shared_ptr<DataFileImpl>(new DataFileDir(path));
        } else {
            p_ = shared_ptr<DataFileImpl>(new DataFileZip(path));
        }
    } 
}

bool DataFile::isError() const
{
    if(p_)
        return p_->isError();
    return true;
}

string DataFile::name() const
{
    if(p_)
        return p_->name();
    return string();
}

string DataFile::errorString() const
{
    if(p_)
        return p_->errorString();
    return "datafile not found";
}

DataSource::DataSource(DataFile& datafile, const string& resource)
    : p_(shared_ptr<DataSourceImpl>(datafile.p_->getSource(resource)))
{
}

std::streamsize DataSource::read(char *s, std::streamsize n)
{
    return p_->read(s, n);
}

bool DataSource::isError() const
{
    return p_->isError();
}

string DataSource::errorString() const
{
    return p_->errorString();
}

