#ifndef __ARCHIVE_ADDIN_H__
#define __ARCHIVE_ADDIN_H__

class ArchiveSource;
class RemoteArchiveSource;

// std::string ws2s(const std::wstring& s);

class ArchiveAddin : public AddinBase {
public:
    OpcUa_StatusCode Init(ProcessRequestCallback func);

    MPLC_ARCHIVE_API static ArchiveSource* GetArchiveSource(int64_t archiveId);
    MPLC_ARCHIVE_API static ArchiveSource* GetArchiveSourceByIndex(int archiveIndex);
    MPLC_ARCHIVE_API static void AddSourceFactory(std::string name,
                                                  ArchiveSourceFactory* sourceFactory);

    MPLC_ARCHIVE_API ArchiveSource* FindArchiveSourceByItem(int64_t id, const std::string& path);

    static MPLC_ARCHIVE_API ArchiveAddin* GetInstance();

protected:
    virtual OpcUa_StatusCode OnConfigEvent(ConfigProcessorMode mode, ControllerConfig* config);

private:
    OpcUa_StatusCode CreateNewSource(std::string name,
                                     int64_t archiveId,
                                     const Value* archiveSettings);
    OpcUa_StatusCode InitSources();

    static ArchiveAddin* Instance;

    typedef std::map<int64_t, ArchiveSource*> ArchiveSourceMap;
    ArchiveSourceMap _archiveSourcesMap;

    typedef std::map<std::string, ArchiveSourceFactory*> ArchiveSourceFactoryMap;
    ArchiveSourceFactoryMap _archiveSourceFactoryMap;
    std::vector<ArchiveSource*> _archiveSources;
};

#endif  //__ARCHIVE_ADDIN_H__
