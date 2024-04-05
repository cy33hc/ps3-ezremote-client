
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <mars/base.h>
#include <sys/file.h>
#include <sysutil/osk.h>
#include "ime_dialog.h"
#include "common.h"
#include "fs.h"
#include "lang.h"
#include "windows.h"
#include "zip_util.h"

extern "C"
{
#include <zip.h>
#include <un7zip.h>
#include <unrar.h>
}

#define TRANSFER_SIZE (128 * 1024)

namespace ZipUtil
{
    static char filename_extracted[256];

    void callback_zip(zip_t *zf, double progress, void *client_data)
    {
        if (client_data != nullptr)
        {
            int64_t *bytes_transfered = (int64_t *)client_data;
            *bytes_transfered = progress * 100000;
        }
    }

    void callback_7zip(const char *fileName, unsigned long fileSize, unsigned fileNum, unsigned numFiles)
    {
        sprintf(activity_message, "%s %s (%d/%d): %s", lang_strings[STR_EXTRACTING], filename_extracted, fileNum, numFiles, fileName);
    }

    int callback_rar(UINT msg, LPARAM UserData, LPARAM P1, LPARAM P2)
    {
        if (msg == UCM_PROCESSDATA)
        {
            int64_t *bytes = (int64_t*) UserData;
            *bytes += P2;
            return 1;
        }
    }

    int ZipAddFile(zip_t *zf, const std::string &path, int filename_start)
    {
        int res;
        struct zip_source *source = zip_source_file(zf, path.c_str(), 0, 0);
        if (!source)
        {
            return 0;
        }

        res = zip_add(zf, path.substr(filename_start).c_str(), source);
        if (res < 0)
            return res;

        return 1;
    }

    int ZipAddFolder(zip_t *zf, const std::string &path, int filename_start)
    {
        int res;

        // Open new file in zip
        std::string folder = path.substr(filename_start);
        if (folder[folder.length() - 1] != '/')
            folder = folder + "/";

        res = zip_add_dir(zf, folder.c_str());

        if (res < 0)
            return res;

        return 1;
    }

    int ZipAddPath(zip_t *zf, const std::string &path, int filename_start)
    {
        bytes_to_download = 100000;
        zip_register_progress_callback_with_state(zf, 0.001f, callback_zip, nullptr, &bytes_transfered);

        DIR *dfd = opendir(path.c_str());
        if (dfd != NULL)
        {
            int ret = ZipAddFolder(zf, path, filename_start);
            if (ret <= 0)
                return ret;

            struct dirent *dirent;
            do
            {
                dirent = readdir(dfd);
                if (stop_activity)
                    return 1;
                if (dirent != NULL && strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
                {
                    int new_path_length = path.length() + strlen(dirent->d_name) + 2;
                    char *new_path = (char *)malloc(new_path_length);
                    snprintf(new_path, new_path_length, "%s%s%s", path.c_str(), FS::hasEndSlash(path.c_str()) ? "" : "/", dirent->d_name);

                    int ret = 0;

                    if (dirent->d_type & DT_DIR)
                    {
                        ret = ZipAddPath(zf, new_path, filename_start);
                    }
                    else
                    {
                        sprintf(activity_message, "%s %s", lang_strings[STR_COMPRESSING], new_path);
                        ret = ZipAddFile(zf, new_path, filename_start);
                    }

                    free(new_path);

                    // Some folders are protected and return 0x80010001. Bypass them
                    if (ret <= 0)
                    {
                        closedir(dfd);
                        return ret;
                    }
                }
            } while (dirent != NULL);

            closedir(dfd);
        }
        else
        {
            sprintf(activity_message, "%s %s", lang_strings[STR_COMPRESSING], path.c_str());
            return ZipAddFile(zf, path, filename_start);
        }

        return 1;
    }

    CompressFileType getCompressFileType(const std::string &file)
    {
        char buf[8];

        memset(buf, 0, 8);
        int ret = FS::Head(file, buf, 8);
        if (ret == 0)
            return COMPRESS_FILE_TYPE_UNKNOWN;

        if (strncmp(buf, (const char *)MAGIC_7Z_1, 6) == 0)
            return COMPRESS_FILE_TYPE_7Z;
        else if (strncmp(buf, (const char *)MAGIC_ZIP_1, 4) == 0 || strncmp(buf, (const char *)MAGIC_ZIP_2, 4) == 0 || strncmp(buf, (const char *)MAGIC_ZIP_3, 4) == 0)
            return COMPRESS_FILE_TYPE_ZIP;
        else if (strncmp(buf, (const char *)MAGIC_RAR_1, 7) == 0 || strncmp(buf, (const char *)MAGIC_RAR_2, 8) == 0)
            return COMPRESS_FILE_TYPE_RAR;

        return COMPRESS_FILE_TYPE_UNKNOWN;
    }

    int ExtractZip(const DirEntry &zip_file, const std::string &dir)
    {
        char path[256];
        uint8_t *buffer;
        std::string dest_dir = std::string(dir);
        if (dest_dir[dest_dir.length() - 1] != '/')
        {
            dest_dir = dest_dir + "/";
        }

        struct zip *archive = zip_open(zip_file.path, ZIP_CHECKCONS, NULL);
        int files = zip_get_num_files(archive);

        if (files <= 0)
        {
            return 0;
        }

        buffer = (uint8_t *)malloc(TRANSFER_SIZE);
        if (!buffer)
            return 0;

        for (int i = 0; i < files; i++)
        {
            const char *filename = zip_get_name(archive, i, 0);

            snprintf(activity_message, 255, "%s %s: %s", lang_strings[STR_EXTRACTING], zip_file.name, filename);

            if (!filename)
                continue;

            if (filename[0] == '/')
                filename++;

            snprintf(path, sizeof(path) - 1, "%s%s", dest_dir.c_str(), filename);
            FS::MkDirs(path, true);

            if (filename[strlen(filename) - 1] == '/')
                continue;

            struct zip_stat st;
            if (zip_stat_index(archive, i, 0, &st))
            {
                continue;
            }
            struct zip_file *zfd = zip_fopen_index(archive, i, 0);
            if (!zfd)
            {
                continue;
            }

            FILE *tfd = fopen(path, "wb");
            if (!tfd)
            {
                free(buffer);
                zip_fclose(zfd);
                zip_close(archive);
                return 0;
            }

            uint64_t pos = 0, count;
            bytes_transfered = 0;
            bytes_to_download = st.size;
            while (pos < st.size)
            {
                count = MIN(TRANSFER_SIZE, st.size - pos);
                if (zip_fread(zfd, buffer, count) != count)
                {
                    free(buffer);
                    fclose(tfd);
                    zip_fclose(zfd);
                    zip_close(archive);
                    return 0;
                }
                bytes_transfered += count;
                fwrite(buffer, count, 1, tfd);
                pos += count;
            }

            zip_fclose(zfd);
            fclose(tfd);
            sysLv2FsChmod(path, S_IFMT | 0777);
        }

        if (archive)
        {
            zip_close(archive);
        }
        free(buffer);

        return 1;
    }

    int Extract7Zip(const DirEntry &file, const std::string &dir)
    {
        file_transfering = false;
        FS::MkDirs(dir, true);
        sprintf(filename_extracted, "%s", file.name);
        int res = Extract7zFileEx(file.path, dir.c_str(), callback_7zip, DEFAULT_IN_BUF_SIZE);
        return res == 0;
    }

    int ExtractRar(const DirEntry &rar_file, const std::string &dir)
    {
        int err = 0;
        uint64_t progress = 0, numFiles = 0;
        HANDLE hArcData; // Archive Handle
        struct RAROpenArchiveDataEx rarOpenArchiveData;
        struct RARHeaderData rarHeaderData;

        memset(&rarOpenArchiveData, 0, sizeof(rarOpenArchiveData));
        memset(&rarHeaderData, 0, sizeof(rarHeaderData));
        rarOpenArchiveData.ArcName = (char *)rar_file.path;
        rarOpenArchiveData.OpenMode = RAR_OM_EXTRACT;

        hArcData = RAROpenArchiveEx(&rarOpenArchiveData);
        if (rarOpenArchiveData.OpenResult != ERAR_SUCCESS)
        {
            return 0;
        }
        RARSetCallback(hArcData, callback_rar, &bytes_transfered);

        while (RARReadHeader(hArcData, &rarHeaderData) == ERAR_SUCCESS)
        {
            sprintf(activity_message, "%s %s: %s", lang_strings[STR_EXTRACTING], rar_file.name, rarHeaderData.FileName);

            bytes_to_download = rarHeaderData.UnpSize;
            bytes_transfered = 0;
            if (RARProcessFile(hArcData, RAR_EXTRACT, (char *)dir.c_str(), NULL) != ERAR_SUCCESS)
            {
                err++;
                continue;
            }
        }

        RARCloseArchive(hArcData);
        return (err==0);
    }

    int Extract(const DirEntry &file, const std::string &dir)
    {
        CompressFileType fileType = getCompressFileType(file.path);

        if (fileType == COMPRESS_FILE_TYPE_ZIP)
            return ExtractZip(file, dir);
        else if (fileType == COMPRESS_FILE_TYPE_7Z)
            return Extract7Zip(file, dir);
        else if (fileType == COMPRESS_FILE_TYPE_RAR)
            return ExtractRar(file, dir);
        else
        {
            sprintf(status_message, "%s - %s", file.name, lang_strings[STR_UNSUPPORTED_FILE_FORMAT]);
            return -1;
        }
        return 1;
    }
}