#ifndef LAUNCHER_CONFIG_H
#define LAUNCHER_CONFIG_H

#include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>

#include "clients/remote_client.h"

#define APP_FOLDER "/dev_hdd0/game/RMTC00001/USRDIR"
#define APP_FONTS_DIR APP_FOLDER "/fonts"
#define APP_LANGS_DIR APP_FOLDER "/langs"

#define APP_ID "ezremote-client"
#define DATA_PATH "/dev_hdd0/tmp/" APP_ID
#define CONFIG_INI_FILE DATA_PATH "/config.ini"
#define COOKIE_FILE DATA_PATH "/cookies.txt"
#define TMP_EDITOR_FILE DATA_PATH "/tmp_editor.txt"
#define TMP_SFO_PATH DATA_PATH "/tmp_pkg.sfo"
#define TMP_ICON_PATH DATA_PATH "/tmp_icon.png"
#define TMP_FOLDER_PATH DATA_PATH "/tmp"
#define CACERT_FILE APP_FOLDER "/cacert.pem"

#define PKG_RAP_FOLDER "/dev_hdd0/exdata"
#define PKG_QUEUE_FOLDER "/dev_hdd0/vsh/task"
#define PKG_INSTALL_FOLDER "/dev_hdd0/vsh/game_pkg"

#define CONFIG_GLOBAL "Global"

#define CONFIG_SHOW_HIDDEN_FILES "show_hidden_files"

#define CONFIG_REMOTE_SERVER_NAME "remote_server_name"
#define CONFIG_REMOTE_SERVER_URL "remote_server_url"
#define CONFIG_REMOTE_SERVER_USER "remote_server_user"
#define CONFIG_REMOTE_SERVER_PASSWORD "remote_server_password"
#define CONFIG_REMOTE_SERVER_HTTP_PORT "remote_server_http_port"
#define CONFIG_REMOTE_HTTP_SERVER_TYPE "remote_server_http_server_type"
#define CONFIG_REMOTE_DEFAULT_DIRECTORY "remote_server_default_directory"
#define CONFIG_ENABLE_RPI "remote_server_enable_rpi"

#define CONFIG_VERSION "config_version"
#define CONFIG_VERSION_NUM 1

#define CONFIG_MAX_EDIT_FILE_SIZE "max_edit_file_size"

#define CONFIG_LAST_SITE "last_site"

#define CONFIG_LOCAL_DIRECTORY "local_directory"
#define CONFIG_TMP_FOLDER_PATH "temp_folder"

#define CONFIG_LANGUAGE "language"

#define HTTP_SERVER_APACHE "Apache"
#define HTTP_SERVER_MS_IIS "Microsoft IIS"
#define HTTP_SERVER_NGINX "Nginx"
#define HTTP_SERVER_NPX_SERVE "Serve"
#define HTTP_SERVER_RCLONE "RClone"

#define MAX_EDIT_FILE_SIZE 32768

struct RemoteSettings
{
    char site_name[32];
    char server[256];
    char username[33];
    char password[128];
    ClientType type;
    uint32_t supported_actions;
    char http_server_type[24];
    char default_directory[256];
    bool enable_rpi;
};

struct PackageUrlInfo
{
    char url[512];
    char username[33];
    char password[25];
};

extern std::vector<std::string> sites;
extern std::vector<std::string> http_servers;
extern std::set<std::string> text_file_extensions;
extern std::set<std::string> image_file_extensions;
extern std::map<std::string, RemoteSettings> site_settings;
extern char local_directory[255];
extern char remote_directory[255];
extern char app_ver[6];
extern char last_site[32];
extern char display_site[32];
extern std::vector<std::string> langs;
extern char language[128];
extern RemoteSettings *remote_settings;
extern RemoteClient *remoteclient;
extern PackageUrlInfo install_pkg_url;
extern int max_edit_file_size;
extern unsigned char cipher_key[32];
extern unsigned char cipher_iv[16];
extern bool show_hidden_files;
extern char temp_folder[256];
extern bool show_hidden_files;

namespace CONFIG
{
    void LoadConfig();
    void SaveConfig();
    void SaveGlobalConfig();
    void SaveLocalDirecotry(const std::string &path);
    void SaveFavoriteUrl(int index, char *url);
    void SetClientType(RemoteSettings *settings);
}
#endif
