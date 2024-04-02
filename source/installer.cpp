#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <polarssl/sha1.h>
#include <polarssl/sha256.h>
#include "clients/webdav.h"
#include "clients/remote_client.h"
#include "clients/baseclient.h"
#include "pdb_data.h"
#include "pdb.h"
#include "installer.h"
#include "util.h"
#include "config.h"
#include "windows.h"
#include "lang.h"
#include "fs.h"
#include "pkg_header.h"

static BaseClient *playstation_client;

namespace INSTALLER
{
	int DownloadIcon(PKGHeader *header, const std::string &icon_path)
	{
		char icon_url[256];
		char icon_file[256];
		uint8_t hmac[20];
		uint32_t sz;

		if (playstation_client == nullptr)
		{
			playstation_client = new BaseClient();
			if (!playstation_client->Connect("http://tmdb.np.dl.playstation.net", "", ""))
			{
				return 0;
			}
		}

		char download_path[256];

		snprintf(download_path, sizeof(download_path)-1, "%.9s_00", header->content_id + 7);
		sha1_hmac(tmdb_hmac_key, sizeof(tmdb_hmac_key), (uint8_t *)download_path, 12, hmac);

		snprintf(download_path, sizeof(download_path)-1, "/tmdb/%.9s_00_%llX%llX%X/ICON0.PNG", header->content_id + 7,
				 ((uint64_t *)hmac)[0],
				 ((uint64_t *)hmac)[1],
				 ((uint32_t *)hmac)[4]);

		if (!playstation_client->FileExists(download_path))
		{
			return 0;
		}

		int ret = playstation_client->Get(icon_path, download_path);

		return ret;
	}

	std::string getRemoteUrl(const std::string path, bool encodeUrl)
	{
		if (strlen(remote_settings->username) == 0 && strlen(remote_settings->password) == 0 &&
			(remoteclient->clientType() == CLIENT_TYPE_WEBDAV || remoteclient->clientType() == CLIENT_TYPE_HTTP_SERVER))
		{
			std::string full_url = WebDAVClient::GetHttpUrl(remote_settings->server + path);
			size_t scheme_pos = full_url.find("://");
			if (scheme_pos == std::string::npos)
				return "";
			size_t root_pos = full_url.find("/", scheme_pos + 3);
			std::string host = full_url.substr(0, root_pos);
			std::string path = full_url.substr(root_pos);

			if (encodeUrl)
			{
				path = CHTTPClient::EncodeUrl(path);
			}

			return host + path;
		}

		return "";
	}

	bool canInstallRemotePkg(const std::string &url)
	{
		if (strlen(remote_settings->username) == 0 && strlen(remote_settings->password) == 0 &&
			(remoteclient->clientType() == CLIENT_TYPE_WEBDAV || remoteclient->clientType() == CLIENT_TYPE_HTTP_SERVER))
		{
			return true;
		}
		return false;
	}

	int InstallRemotePkg(const std::string &url, PKGHeader *header)
	{
		if (url.empty())
			return 0;

		int ret;
		char queue_task_dir[256];
		char temp_path[256];

		uint32_t install_task_id = PDB::GetNextQueueTaskId();
		snprintf(queue_task_dir, sizeof(queue_task_dir) - 1, "%s/%d", PKG_QUEUE_FOLDER, install_task_id);

		FS::MkDirs(queue_task_dir);

		snprintf(temp_path, 255, "%s/ICON_FILE", queue_task_dir);
		if (!DownloadIcon(header, temp_path))
		{
			FS::Save(temp_path, iconfile_data, iconfile_data_size);
		}

		if (!PDB::CreateQueuePDBFiles(queue_task_dir, url, header))
			return 0;

		return 1;
	}

	int InstallLocalPkg(const std::string &path, PKGHeader *header)
	{
		int ret;
		char install_task_dir[256];
		char temp_path[256];
		char icon_file[256];

		if (header->pkg_magic != PKG_MAGIC)
			return 0;

		uint32_t install_task_id = PDB::GetNextInstallTaskId();
		snprintf(install_task_dir, sizeof(install_task_dir) - 1, "%s/%d", PKG_INSTALL_FOLDER, install_task_id);

		FS::MkDirs(install_task_dir);

		snprintf(temp_path, 255, "%s/%s.png", TMP_FOLDER_PATH, header->content_id);
		if (!DownloadIcon(header, temp_path))
		{
			FS::Save(temp_path, iconfile_data, iconfile_data_size);
		}
		snprintf(icon_file, sizeof(icon_file)-1, "%s/ICON_FILE", install_task_dir);
		FS::Rename(temp_path, icon_file);

		snprintf(temp_path, 255, "%s/%s.pkg", install_task_dir, header->content_id);
		FS::Rename(path, temp_path);

		if (!PDB::CreateInstallPDBFiles(install_task_dir, header))
			return 0;

		return 1;
	}
}
