#pragma once

#include <glaze/glaze.hpp>

#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

template <typename T>
inline T LoadJson(std::string_view path)
{
	T obj{};
	const std::string pathString(path);
	std::string buffer;
	const auto& ec = glz::read_file_json(obj, pathString, buffer);
	if (ec)
	{
		throw std::runtime_error(std::format("Cannot read JSON file \"{}\", error:\n{}", pathString, glz::format_error(ec, buffer)));
	}

	return obj;
}

template <typename T>
inline T LoadJson(std::string_view root, std::string_view file)
{
	return LoadJson<T>(std::string(root) + "/" + std::string(file));
}
