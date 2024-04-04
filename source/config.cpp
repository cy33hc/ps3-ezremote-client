#include <string>
#include <map>
#include <vector>
#include <regex>
#include <stdlib.h>
#include "config.h"
#include "fs.h"
#include "lang.h"
#include "util.h"
#include "crypt.h"
#include "base64.h"

extern "C"
{
#include "inifile.h"
}

bool swap_xo;
RemoteSettings *remote_settings;
char local_directory[255];
char remote_directory[255];
char app_ver[6];
char last_site[32];
char display_site[32];
std::vector<std::string> langs;
char language[128];
std::vector<std::string> sites;
std::vector<std::string> http_servers;
std::set<std::string> text_file_extensions;
std::set<std::string> image_file_extensions;
std::map<std::string, RemoteSettings> site_settings;
PackageUrlInfo install_pkg_url;
int max_edit_file_size;
bool show_hidden_files;
char temp_folder[256];
std::map<std::string, PS3Account> ps3_accounts;
PS3Account *selected_ps3account;

unsigned char cipher_key[32] = {'s', '5', 'v', '8', 'y', '/', 'B', '?', 'E', '(', 'H', '+', 'M', 'b', 'Q', 'e', 'T', 'h', 'W', 'm', 'Z', 'q', '4', 't', '7', 'w', '9', 'z', '$', 'C', '&', 'F'};
unsigned char cipher_iv[16] = {'Y', 'p', '3', 's', '6', 'v', '9', 'y', '$', 'B', '&', 'E', ')', 'H', '@', 'M'};

RemoteClient *remoteclient;

namespace CONFIG
{
    void GetPS3ActivatedAccounts()
    {
        int ret;
        char path[256];

        std::vector<DirEntry> home_list = FS::ListDir("/dev_hdd0/home", &ret);
        for(std::vector<DirEntry>::iterator it = home_list.begin(); it != home_list.end(); it++)
		{
			if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0)
			{
				snprintf(path, sizeof(path) - 1, "%s%s%s", "/dev_hdd0/home/", it->name, "/exdata/act.dat");
				if (FS::FileExists(path))
				{
					PS3Account account;
                    snprintf(account.account_id, sizeof(account.account_id)-1, "%s", it->name);
                    snprintf(account.home_dir, sizeof(account.home_dir)-1, "/dev_hdd0/home/%s", it->name);
                    snprintf(path, sizeof(path) - 1, "%s%s%s", "/dev_hdd0/home/", it->name, "/localusername");
                    if (FS::FileExists(path))
                    {
                        std::vector<char> local_username = FS::Load(path);
                        snprintf(account.username, sizeof(account.username) - 1, "%s", local_username.data());
                    }
                    else
                    {
                        snprintf(account.username, sizeof(account.username) - 1, "%s", it->name);
                    }

                    ps3_accounts.insert(std::make_pair(std::string(account.username), account));
				}
			}
		}
    }

    int Encrypt(const std::string &text, std::string &encrypt_text)
    {
        unsigned char tmp_encrypt_text[text.length() * 2];
        int encrypt_text_len;
        memset(tmp_encrypt_text, 0, sizeof(tmp_encrypt_text));
        int ret = openssl_encrypt((unsigned char *)text.c_str(), text.length(), cipher_key, cipher_iv, tmp_encrypt_text, &encrypt_text_len);
        if (ret == 0)
            return 0;
        return Base64::Encode(std::string((const char *)tmp_encrypt_text, encrypt_text_len), encrypt_text);
    }

    int Decrypt(const std::string &text, std::string &decrypt_text)
    {
        std::string tmp_decode_text;
        int ret = Base64::Decode(text, tmp_decode_text);
        if (ret == 0)
            return 0;

        unsigned char tmp_decrypt_text[tmp_decode_text.length() * 2];
        int decrypt_text_len;
        memset(tmp_decrypt_text, 0, sizeof(tmp_decrypt_text));
        ret = openssl_decrypt((unsigned char *)tmp_decode_text.c_str(), tmp_decode_text.length(), cipher_key, cipher_iv, tmp_decrypt_text, &decrypt_text_len);
        if (ret == 0)
            return 0;

        decrypt_text.clear();
        decrypt_text.append(std::string((const char *)tmp_decrypt_text, decrypt_text_len));

        return 1;
    }

    void SetClientType(RemoteSettings *setting)
    {
        if (strncmp(setting->server, "smb://", 6) == 0)
        {
            setting->type = CLIENT_TYPE_SMB;
        }
        else if (strncmp(setting->server, "ftp://", 6) == 0)
        {
            setting->type = CLIENT_TYPE_FTP;
        }
        else if (strncmp(setting->server, "sftp://", 7) == 0)
        {
            setting->type = CLIENT_TYPE_SFTP;
        }
        else if (strncmp(setting->server, "webdav://", 9) == 0 || strncmp(setting->server, "webdavs://", 10) == 0)
        {
            setting->type = CLIENT_TYPE_WEBDAV;
        }
        else if (strncmp(setting->server, "https://drive.google.com", 24) == 0)
        {
            setting->type = CLIENT_TYPE_GOOGLE;
        }
        else if (strncmp(setting->server, "http://", 7) == 0 || strncmp(setting->server, "https://", 8) == 0)
        {
            setting->type = CLIENT_TYPE_HTTP_SERVER;
        }
        else if (strncmp(setting->server, "nfs://", 6) == 0)
        {
            setting->type = CLIENT_TYPE_NFS;
        }
        else
        {
            setting->type = CLINET_TYPE_UNKNOWN;
        }
    }

    void LoadConfig()
    {
        if (!FS::FolderExists(DATA_PATH))
        {
            FS::MkDirs(DATA_PATH);
        }

        GetPS3ActivatedAccounts();

        sites = {"Site 1", "Site 2", "Site 3", "Site 4", "Site 5", "Site 6", "Site 7", "Site 8", "Site 9", "Site 10",
                 "Site 11", "Site 12", "Site 13", "Site 14", "Site 15", "Site 16", "Site 17", "Site 18", "Site 19", "Site 20"};

        langs = { "Default", "Arabic", "Catalan", "Croatian", "Dutch", "English", "Euskera", "French", "Galego", "German", "Greek", 
                  "Hungarian", "Indonesian", "Italiano", "Japanese", "Korean", "Polish", "Portuguese_BR", "Russian", "Romanian", "Ryukyuan", "Spanish", "Turkish",
                  "Simplified Chinese", "Traditional Chinese", "Thai", "Ukrainian"};

        http_servers = {HTTP_SERVER_APACHE, HTTP_SERVER_MS_IIS, HTTP_SERVER_NGINX, HTTP_SERVER_NPX_SERVE, HTTP_SERVER_RCLONE};
        text_file_extensions = { ".txt", ".ini", ".log", ".json", ".xml", ".html", ".xhtml", ".conf", ".config" };
        image_file_extensions = { ".bmp", ".jpg", ".jpeg", ".png", ".webp" };

        OpenIniFile(CONFIG_INI_FILE);

        int version = ReadInt(CONFIG_GLOBAL, CONFIG_VERSION, 0);
        bool conversion_needed = false;
        if (version < CONFIG_VERSION_NUM)
        {
            conversion_needed = true;
        }
        WriteInt(CONFIG_GLOBAL, CONFIG_VERSION, CONFIG_VERSION_NUM);

        // Load global config
        sprintf(language, "%s", ReadString(CONFIG_GLOBAL, CONFIG_LANGUAGE, "Default"));
        WriteString(CONFIG_GLOBAL, CONFIG_LANGUAGE, language);

        sprintf(local_directory, "%s", ReadString(CONFIG_GLOBAL, CONFIG_LOCAL_DIRECTORY, "/"));
        WriteString(CONFIG_GLOBAL, CONFIG_LOCAL_DIRECTORY, local_directory);

        max_edit_file_size = ReadInt(CONFIG_GLOBAL, CONFIG_MAX_EDIT_FILE_SIZE, MAX_EDIT_FILE_SIZE);
        WriteInt(CONFIG_GLOBAL, CONFIG_MAX_EDIT_FILE_SIZE, max_edit_file_size);

        show_hidden_files = ReadBool(CONFIG_GLOBAL, CONFIG_SHOW_HIDDEN_FILES, false);
        WriteBool(CONFIG_GLOBAL, CONFIG_SHOW_HIDDEN_FILES, show_hidden_files);

        sprintf(temp_folder, ReadString(CONFIG_GLOBAL, CONFIG_TMP_FOLDER_PATH, TMP_FOLDER_PATH));
        WriteString(CONFIG_GLOBAL, CONFIG_TMP_FOLDER_PATH, temp_folder);

        if (!FS::FolderExists(temp_folder))
        {
            FS::MkDirs(temp_folder);
        }

        char temp_account_id[12];
        snprintf(temp_account_id, 11, "%s", ReadString(CONFIG_GLOBAL, CONFIG_PS3_ACCOUNT, ""));
        selected_ps3account = nullptr;
        if (strlen(temp_account_id) == 0)
        {
            if (ps3_accounts.size() > 0)
            {
                // select the first ps3 account by default if none is selected
                selected_ps3account = &ps3_accounts.begin()->second;
                WriteString(CONFIG_GLOBAL, CONFIG_PS3_ACCOUNT, selected_ps3account->account_id);
            }
        }
        else
        {
            if (ps3_accounts.size() > 0)
            {
                for (std::map<std::string, PS3Account>::iterator it = ps3_accounts.begin(); it != ps3_accounts.end(); it++)
                {
                    if (strcmp(temp_account_id, it->second.account_id) == 0)
                    {
                        selected_ps3account = &it->second;
                    }
                }
            }
        }

        //sprintf(compressed_file_path, "%s", ReadString(CONFIG_HTTP_SERVER, CONFIG_HTTP_SERVER_COMPRESSED_FILE_PATH, CONFIG_DEFAULT_COMPRESSED_FILE_PATH));
        //WriteString(CONFIG_HTTP_SERVER, CONFIG_HTTP_SERVER_COMPRESSED_FILE_PATH, compressed_file_path);

        for (int i = 0; i < sites.size(); i++)
        {
            RemoteSettings setting;
            memset(&setting, 0, sizeof(RemoteSettings));
            sprintf(setting.site_name, "%s", sites[i].c_str());

            sprintf(setting.server, "%s", ReadString(sites[i].c_str(), CONFIG_REMOTE_SERVER_URL, ""));
            if (conversion_needed && strlen(setting.server) > 0)
            {
                std::string tmp = std::string(setting.server);
                tmp = std::regex_replace(tmp, std::regex("http://"), "webdav://");
                tmp = std::regex_replace(tmp, std::regex("https://"), "webdavs://");
                sprintf(setting.server, "%s", tmp.c_str());
            }
            WriteString(sites[i].c_str(), CONFIG_REMOTE_SERVER_URL, setting.server);

            sprintf(setting.username, "%s", ReadString(sites[i].c_str(), CONFIG_REMOTE_SERVER_USER, ""));
            WriteString(sites[i].c_str(), CONFIG_REMOTE_SERVER_USER, setting.username);

            char tmp_password[128];
            sprintf(tmp_password, "%s", ReadString(sites[i].c_str(), CONFIG_REMOTE_SERVER_PASSWORD, ""));
            std::string encrypted_password = tmp_password;
            if (strlen(tmp_password) > 0)
            {
                std::string decrypted_password;
                int ret = Decrypt(tmp_password, decrypted_password);
                if (ret == 0)
                    sprintf(setting.password, "%s", tmp_password);
                else
                    sprintf(setting.password, "%s", decrypted_password.c_str());
                Encrypt(setting.password, encrypted_password);
            }
            WriteString(sites[i].c_str(), CONFIG_REMOTE_SERVER_PASSWORD, encrypted_password.c_str());

            setting.enable_bd = ReadBool(sites[i].c_str(), CONFIG_ENABLE_BACKGROUND_DOWNLOAD, false);
            WriteBool(sites[i].c_str(), CONFIG_ENABLE_BACKGROUND_DOWNLOAD, setting.enable_bd);

            sprintf(setting.http_server_type, "%s", ReadString(sites[i].c_str(), CONFIG_REMOTE_HTTP_SERVER_TYPE, HTTP_SERVER_APACHE));
            WriteString(sites[i].c_str(), CONFIG_REMOTE_HTTP_SERVER_TYPE, setting.http_server_type);

            sprintf(setting.default_directory, "%s", ReadString(sites[i].c_str(), CONFIG_REMOTE_DEFAULT_DIRECTORY, "/"));
            WriteString(sites[i].c_str(), CONFIG_REMOTE_DEFAULT_DIRECTORY, setting.default_directory);

            SetClientType(&setting);
            site_settings.insert(std::make_pair(sites[i], setting));
        }

        sprintf(last_site, "%s", ReadString(CONFIG_GLOBAL, CONFIG_LAST_SITE, sites[0].c_str()));
        WriteString(CONFIG_GLOBAL, CONFIG_LAST_SITE, last_site);

        remote_settings = &site_settings[std::string(last_site)];
        sprintf(remote_directory, "%s", remote_settings->default_directory);

        WriteIniFile(CONFIG_INI_FILE);
        CloseIniFile();
    }

    void SaveConfig()
    {
        OpenIniFile(CONFIG_INI_FILE);

        std::string encrypted_text;
        if (strlen(remote_settings->password) > 0)
            Encrypt(remote_settings->password, encrypted_text);
        else
            encrypted_text = remote_settings->password;
        WriteString(last_site, CONFIG_REMOTE_SERVER_URL, remote_settings->server);
        WriteString(last_site, CONFIG_REMOTE_SERVER_USER, remote_settings->username);
        WriteString(last_site, CONFIG_REMOTE_SERVER_PASSWORD, remote_settings->password);
        WriteString(last_site, CONFIG_REMOTE_HTTP_SERVER_TYPE, remote_settings->http_server_type);
        WriteString(last_site, CONFIG_REMOTE_DEFAULT_DIRECTORY, remote_settings->default_directory);
        WriteBool(last_site, CONFIG_ENABLE_BACKGROUND_DOWNLOAD, remote_settings->enable_bd);
        WriteString(CONFIG_GLOBAL, CONFIG_LAST_SITE, last_site);
               
        WriteIniFile(CONFIG_INI_FILE);
        CloseIniFile();
    }

    void SaveGlobalConfig()
    {
        OpenIniFile(CONFIG_INI_FILE);

        WriteString(CONFIG_GLOBAL, CONFIG_TMP_FOLDER_PATH, temp_folder);
        WriteBool(CONFIG_GLOBAL, CONFIG_SHOW_HIDDEN_FILES, show_hidden_files);
        WriteString(CONFIG_GLOBAL, CONFIG_LANGUAGE, language);
        if (selected_ps3account != nullptr)
            WriteString(CONFIG_GLOBAL, CONFIG_PS3_ACCOUNT, selected_ps3account->account_id);
        //WriteString(CONFIG_HTTP_SERVER, CONFIG_HTTP_SERVER_COMPRESSED_FILE_PATH, compressed_file_path);

        WriteIniFile(CONFIG_INI_FILE);
        CloseIniFile();

        if (!FS::FolderExists(temp_folder))
        {
            FS::MkDirs(temp_folder);
        }

        //if (!FS::FolderExists(compressed_file_path))
        //{
        //    FS::MkDirs(compressed_file_path);
        //}
    }

    void SaveLocalDirecotry(const std::string &path)
    {
        OpenIniFile(CONFIG_INI_FILE);
        WriteString(CONFIG_GLOBAL, CONFIG_LOCAL_DIRECTORY, path.c_str());
        WriteIniFile(CONFIG_INI_FILE);
        CloseIniFile();
    }
}
