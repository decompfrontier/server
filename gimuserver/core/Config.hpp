#pragma once

#include <string>

class Config
{
public:
	static Config& Instance() { return m_cfg; }

	bool Load(const std::string& path);

	const auto& GetWallpaperInfo() const { return m_wallpaper; }
	const auto& GetContentRootPath() const { return m_contentRoot; }

protected:
	static Config m_cfg;

	std::string m_wallpaper, m_contentRoot;
};
