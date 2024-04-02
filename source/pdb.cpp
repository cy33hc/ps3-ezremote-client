#include <stdio.h>
#include <sys/stat.h>
#include "config.h"
#include "common.h"
#include "fs.h"
#include "pdb_data.h"
#include "pkg_header.h"
#include <mini18n.h>

static uint32_t queue_task_id = 10000002;
static uint32_t install_task_id = 80000002;

namespace PDB
{
	void write_pdb_string(FILE *fp, const char *header, const char *pdbstr)
	{
		unsigned int pdbstr_len = strlen(pdbstr) + 1;

		FS::Write(fp, header, 4);
		FS::Write(fp, header, 4);
		FS::Write(fp, &pdbstr_len, 4);
		FS::Write(fp, &pdbstr_len, 4);
		FS::Write(fp, pdbstr, pdbstr_len);
	}

	void write_pdb_int64(FILE *fp, const char *header, const uint64_t *pdb_u64)
	{
		FS::Write(fp, header, 4);
		FS::Write(fp, "\x00\x00\x00\x08\x00\x00\x00\x08", 8);
		FS::Write(fp, pdb_u64, 8);
	}

	uint32_t GetNextQueueTaskId()
	{
		char path[256] = "";
		int found = false;
		struct stat sb;

		while (!found)
		{
			snprintf(path, sizeof(path) - 1, "%s/%d", PKG_QUEUE_FOLDER, queue_task_id);

			if ((stat(path, &sb) == 0) && S_ISDIR(sb.st_mode))
			{
				// there is already a directory with the ID, try again...
				queue_task_id++;
			}
			else
			{
				found = 1;
			}
		}

		return queue_task_id;
	}

	uint32_t GetNextInstallTaskId()
	{
		char path[256] = "";
		int found = false;
		struct stat sb;

		while (!found)
		{
			snprintf(path, sizeof(path) - 1, "%s/%d", PKG_INSTALL_FOLDER, install_task_id);

			if ((stat(path, &sb) == 0) && S_ISDIR(sb.st_mode))
			{
				// there is already a directory with the ID, try again...
				install_task_id++;
			}
			else
			{
				found = 1;
			}
		}

		return install_task_id;
	}

	int CreateQueuePDBFiles(const std::string &pkg_task_folder, const std::string &url, PKGHeader *header)
	{
		// Create files
		char temp_str[256];
		char szIconFile[256];

		uint64_t pkg_size = BE64(header->pkg_size);

		snprintf(szIconFile, sizeof(szIconFile)-1, "%s/ICON_FILE", pkg_task_folder.c_str());

		snprintf(temp_str, sizeof(temp_str)-1, "%s/d0.pdb", pkg_task_folder.c_str());
		FILE *fpPDB = FS::Create(temp_str);
		if (!fpPDB)
		{
			return 0;
		}

		// write - d0.pdb
		FS::Write(fpPDB, pkg_d0top_data, d0top_data_size);

		// 000000CE - Download expected size (in bytes)
		write_pdb_int64(fpPDB, PDB_HDR_SIZE, &pkg_size);

		// 000000CB - PKG file name
		snprintf(temp_str, sizeof(temp_str)-1, "%s.pkg", header->content_id);
		write_pdb_string(fpPDB, PDB_HDR_FILENAME, temp_str);

		// 000000CC - date/time
		write_pdb_string(fpPDB, PDB_HDR_DATETIME, "Mon, 11 Dec 2017 11:45:10 GMT");

		// 000000CA - PKG Link download URL
		write_pdb_string(fpPDB, PDB_HDR_URL, url.c_str());

		// 0000006A - Icon location / path (PNG w/o extension)
		write_pdb_string(fpPDB, PDB_HDR_ICON, szIconFile);

		// 00000069 - Display title
		snprintf(temp_str, sizeof(temp_str)-1, "\xE2\x98\x85 %s \x22%s\x22", _("Download"), _(header->content_id));
		write_pdb_string(fpPDB, PDB_HDR_TITLE, temp_str);

		// 000000D9 - Content id
		write_pdb_string(fpPDB, PDB_HDR_CONTENT, header->content_id);

		FS::Write(fpPDB, pkg_d0end_data, pkg_d0end_data_size);
		fclose(fpPDB);

		return 1;
	}

	int CreateInstallPDBFiles(const std::string &pkg_task_folder, PKGHeader *header)
	{
		FILE *fp1;
		FILE *fp2;
		char temp_buffer[256];

		uint64_t pkg_size = BE64(header->pkg_size);

		snprintf(temp_buffer, sizeof(temp_buffer), "%s/d0.pdb", pkg_task_folder.c_str());
		fp1 = FS::Create(temp_buffer);

		snprintf(temp_buffer, sizeof(temp_buffer), "%s/d1.pdb", pkg_task_folder.c_str());
		fp2 = FS::Create(temp_buffer);

		if (!fp1 || !fp2)
		{
			return 0;
		}

		FS::Write(fp1, install_data_pdb, install_data_pdb_size);
		FS::Write(fp2, install_data_pdb, install_data_pdb_size);

		// 000000D0 - Downloaded size (in bytes)
		write_pdb_int64(fp1, PDB_HDR_DLSIZE, &pkg_size);
		write_pdb_int64(fp2, PDB_HDR_DLSIZE, &pkg_size);

		// 000000CE - Package expected size (in bytes)
		write_pdb_int64(fp1, PDB_HDR_SIZE, &pkg_size);
		write_pdb_int64(fp2, PDB_HDR_SIZE, &pkg_size);

		// 00000069 - Display title
		snprintf(temp_buffer, sizeof(temp_buffer), "Install %s", header->content_id);
		write_pdb_string(fp1, PDB_HDR_TITLE, temp_buffer);
		write_pdb_string(fp2, PDB_HDR_TITLE, temp_buffer);

		// 000000CB - PKG file name
		snprintf(temp_buffer, sizeof(temp_buffer), "%s.pkg", header->content_id);
		write_pdb_string(fp1, PDB_HDR_FILENAME, temp_buffer);
		write_pdb_string(fp2, PDB_HDR_FILENAME, temp_buffer);

		// 00000000 - Icon location / path (PNG w/o extension)
		snprintf(temp_buffer, sizeof(temp_buffer), "%s/ICON_FILE", pkg_task_folder.c_str());
		write_pdb_string(fp2, PDB_HDR_UNUSED, temp_buffer);

		// 0000006A - Icon location / path (PNG w/o extension)
		write_pdb_string(fp1, PDB_HDR_ICON, temp_buffer);
		write_pdb_string(fp2, PDB_HDR_ICON, temp_buffer);

		FS::Write(fp1, pkg_d0end_data, pkg_d0end_data_size);
		FS::Write(fp2, pkg_d0end_data, pkg_d0end_data_size);

		fclose(fp1);
		fclose(fp2);

		snprintf(temp_buffer, sizeof(temp_buffer), "%s/f0.pdb", pkg_task_folder);
		FS::Save(temp_buffer, temp_buffer, 0);

		return 1;
	}
}