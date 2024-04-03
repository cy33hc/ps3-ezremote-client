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
#include "crypt.h"
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

		snprintf(download_path, sizeof(download_path) - 1, "%.9s_00", header->content_id + 7);
		sha1_hmac(tmdb_hmac_key, sizeof(tmdb_hmac_key), (uint8_t *)download_path, 12, hmac);

		snprintf(download_path, sizeof(download_path) - 1, "/tmdb/%.9s_00_%lX%lX%X/ICON0.PNG", header->content_id + 7,
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

	int DownloadRapOrRif(const std::string &remot_pkg_dir, PKGHeader *header)
	{
		char rap_url[512];
		char rif_url[512];
		char rap_path[512];
		char rif_path[512];
		int ret;

		snprintf(rap_url, sizeof(rap_url) - 1, "%s/%s.rap", remot_pkg_dir.c_str(), header->content_id);
		snprintf(rif_url, sizeof(rif_url) - 1, "%s/%s.rif", remot_pkg_dir.c_str(), header->content_id);
		snprintf(rap_path, sizeof(rap_path) - 1, "%s/%s.rap", temp_folder, header->content_id);
		snprintf(rif_path, sizeof(rif_path) - 1, "%s/%s.rif", temp_folder, header->content_id);

		if (remoteclient == nullptr)
			return 0;

		if (remoteclient->FileExists(rap_url))
		{
			ret = remoteclient->Get(rap_path, rap_url);
			if (ret == 0)
				return 0;
			return 1;
		}
		else
		{
			snprintf(rap_url, sizeof(rap_url) - 1, "%s/exdata/%s.rap", remot_pkg_dir.c_str(), header->content_id);
			if (remoteclient->FileExists(rap_url))
			{
				ret = remoteclient->Get(rap_path, rap_url);
				if (ret == 0)
					return 0;
				return 1;
			}
		}

		if (remoteclient->FileExists(rif_url))
		{
			ret = remoteclient->Get(rif_path, rif_url);
			if (ret == 0)
				return 0;
			return 1;
		}
		else
		{
			snprintf(rif_url, sizeof(rif_url) - 1, "%s/exdata/%s.rif", remot_pkg_dir.c_str(), header->content_id);
			if (remoteclient->FileExists(rif_url))
			{
				ret = remoteclient->Get(rif_path, rif_url);
				if (ret == 0)
					return 0;
				return 1;
			}
		}

		return 0;
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

		snprintf(temp_path, 255, "%s/%s.pkg", queue_task_dir, header->content_id);
		FS::Save(temp_path, "ezRemote-Client PS3", 19);
		truncate(temp_path, Util::BE64((uint8_t*)&header->pkg_size));

		if (!PDB::CreateQueuePDBFiles(queue_task_dir, url, header))
			return 0;

		return 1;
	}

	int CreateRif(char *rap, PKGHeader *header)
	{
		char path[256];
		char rif_path[256];
		char *lic_path = NULL;

		int ret;
		std::vector<DirEntry> home_list = FS::ListDir("/dev_hdd0/home", &ret);

		for(std::vector<DirEntry>::iterator it = home_list.begin(); it != home_list.end(); it++)
		{
			if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0)
			{
				snprintf(path, sizeof(path) - 1, "%s%s%s", "/dev_hdd0/home/", it->name, "/exdata/act.dat");
				if (FS::FileExists(path))
				{
					snprintf(path, sizeof(path) - 1, "%s%s%s", "/dev_hdd0/home/", it->name, "/exdata/");
					lic_path = path;
					break;
				}
			}
		}

		if (!lic_path)
		{
			return 1;
		}

		snprintf(rif_path, sizeof(rif_path)-1, "%s%s.rif", lic_path, header->content_id);
		if (!FS::FileExists(rif_path))
		{
			if (!rap2rif((uint8_t*)rap, header->content_id, lic_path))
			{
				return 0;
			}
		}

		return 1;
	}

	int InstallLocalPkg(const std::string &path, PKGHeader *header)
	{
		int ret;
		char install_task_dir[256];
		char temp_path[256];
		char icon_file[256];
		char rif_path[256];
		char home_path[256];

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

		snprintf(icon_file, sizeof(icon_file) - 1, "%s/ICON_FILE", install_task_dir);
		FS::Rename(temp_path, icon_file);

		snprintf(temp_path, 255, "%s/%s.pkg", install_task_dir, header->content_id);
		FS::Rename(path, temp_path);

		snprintf(temp_path, 255, "%s/%s.rap", temp_folder, header->content_id);
		if (FS::FileExists(temp_path))
		{
			std::vector<char> rap_data = FS::Load(temp_path);

			CreateRif(rap_data.data(), header);
		}
		else
		{
			snprintf(temp_path, 255, "%s/%s.rif", temp_folder, header->content_id);
			if (FS::FileExists(temp_path))
			{
				int err;
				std::vector<DirEntry> home_list = FS::ListDir("/dev_hdd0/home", &err);
				for (std::vector<DirEntry>::iterator it = home_list.begin(); it != home_list.end(); it++)
				{
					if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0)
					{
						snprintf(rif_path, sizeof(rif_path) - 1, "%s%s%s", "/dev_hdd0/home/", it->name, "/exdata/act.dat");
						if (FS::FileExists(rif_path))
						{
							snprintf(rif_path, sizeof(rif_path) - 1, "/dev_hdd0/home/%s/exdata/%s.rif", it->name, header->content_id);
							if (!FS::FileExists(rif_path))
								FS::Rename(temp_path, rif_path);
							break;
						}
					}
				}
			}
		}

		if (!PDB::CreateInstallPDBFiles(install_task_dir, header))
			return 0;

		return 1;
	}
}
