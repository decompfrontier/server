#include "BfWebController.hpp"
#include "core/Utils.hpp"
#include "core/System.hpp"
#include "WebTerms.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

using namespace drogon;
namespace fs = std::filesystem;

bool hasExtension(const std::string& path, const std::string& ext) {
    size_t pos = path.find(ext);
    return pos != std::string::npos && pos == path.length() - ext.length();
}

// Parse dimensions from filename (e.g., "MapVillage_640x0809.png" -> 640, 809)
bool parseDimensions(const std::string& filename, int& width, int& height) {
    size_t lastUnderscore = filename.find_last_of('_');
    size_t dotPos = filename.find_last_of('.');
    if (lastUnderscore == std::string::npos || dotPos == std::string::npos || lastUnderscore >= dotPos) {
        return false;
    }

    std::string dimStr = filename.substr(lastUnderscore + 1, dotPos - lastUnderscore - 1);
    size_t xPos = dimStr.find('x');
    if (xPos == std::string::npos) return false;

    try {
        width = std::stoi(dimStr.substr(0, xPos));
        height = std::stoi(dimStr.substr(xPos + 1));
        return true;
    }
    catch (...) {
        return false;
    }
}

// Generate a minimal PNG with specified dimensions
std::vector<char> generatePlaceholderPng(int width, int height) {
    std::vector<char> pngData;

    // PNG signature
    const char signature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    pngData.insert(pngData.end(), signature, signature + 8);

    // IHDR chunk
    uint32_t ihdrLength = 13;
    ihdrLength = htonl(ihdrLength);
    pngData.insert(pngData.end(), (char*)&ihdrLength, (char*)&ihdrLength + 4);
    const char ihdrType[] = "IHDR";
    pngData.insert(pngData.end(), ihdrType, ihdrType + 4);

    uint32_t w = htonl(width);
    uint32_t h = htonl(height);
    pngData.insert(pngData.end(), (char*)&w, (char*)&w + 4);
    pngData.insert(pngData.end(), (char*)&h, (char*)&h + 4);

    const char ihdrData[] = { 8, 2, 0, 0, 0 }; // Bit depth 8, RGB, no compression, filter, interlace
    pngData.insert(pngData.end(), ihdrData, ihdrData + 5);

    uint32_t ihdrCrc = 0; // Simplified CRC (client may not check)
    ihdrCrc = htonl(ihdrCrc);
    pngData.insert(pngData.end(), (char*)&ihdrCrc, (char*)&ihdrCrc + 4);

    // IDAT chunk (minimal data for a blank image)
    uint32_t idatLength = 12;
    idatLength = htonl(idatLength);
    pngData.insert(pngData.end(), (char*)&idatLength, (char*)&idatLength + 4);
    const char idatType[] = "IDAT";
    pngData.insert(pngData.end(), idatType, idatType + 4);

    // Minimal data: 1 scanline of transparent pixels (RGBA)
    std::vector<char> idatData;
    idatData.push_back(0); // Filter type
    for (int x = 0; x < width; ++x) {
        idatData.push_back(0); idatData.push_back(0); idatData.push_back(0); idatData.push_back(0); // Transparent pixel
    }
    for (int y = 1; y < height; ++y) {
        idatData.push_back(0);
        for (int x = 0; x < width; ++x) {
            idatData.push_back(0); idatData.push_back(0); idatData.push_back(0); idatData.push_back(0);
        }
    }
    // Simplified compression (no real zlib compression)
    pngData.insert(pngData.end(), idatData.begin(), idatData.end());

    uint32_t idatCrc = 0;
    idatCrc = htonl(idatCrc);
    pngData.insert(pngData.end(), (char*)&idatCrc, (char*)&idatCrc + 4);

    // IEND chunk
    uint32_t iendLength = 0;
    iendLength = htonl(iendLength);
    pngData.insert(pngData.end(), (char*)&iendLength, (char*)&iendLength + 4);
    const char iendType[] = "IEND";
    pngData.insert(pngData.end(), iendType, iendType + 4);
    uint32_t iendCrc = 0;
    iendCrc = htonl(iendCrc);
    pngData.insert(pngData.end(), (char*)&iendCrc, (char*)&iendCrc + 4);

    return pngData;
}

void BfWebController::HandleWebPage(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
    Utils::DumpInfoToDrogon(rq, "WebController");

    if (rq->getPath() == "/bf/web/terms.htm") {
        callback(HttpResponse::newFileResponse((const unsigned char*)WEB_TERMS_DATA, strlen(WEB_TERMS_DATA)));
        return;
    }

    auto path = System::Instance().GetContentRootPath() + rq->getPath();
    LOG_INFO << "Requesting path: " << path;

    if (fs::exists(path) && !fs::is_directory(path)) {
        LOG_INFO << "Serving existing file: " << path;
        auto resp = HttpResponse::newFileResponse(path);
        if (hasExtension(path, ".png")) resp->setContentTypeCode(CT_IMAGE_PNG);
        else if (hasExtension(path, ".csv")) resp->setContentTypeCode(CT_TEXT_CSV);
        else if (hasExtension(path, ".dat")) resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        else resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        callback(resp);
        return;
    }

    Utils::AddMissingDlcFile(rq->getPath());
    LOG_INFO << "Generating placeholder for: " << path;

    try {
        fs::path dirPath = path.substr(0, path.find_last_of('/'));
        if (!dirPath.empty() && !fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        std::ofstream outFile(path, std::ios::binary);
        if (outFile.is_open()) {
            if (hasExtension(path, ".png")) {
                std::string filename = rq->getPath().substr(rq->getPath().find_last_of('/') + 1);
                int width = 1, height = 1;
                parseDimensions(filename, width, height);
                auto placeholderPng = generatePlaceholderPng(width, height);
                outFile.write(placeholderPng.data(), placeholderPng.size());
            }
            else if (hasExtension(path, ".csv")) {
                std::string placeholderCsv = "placeholder,data\n1,2";
                outFile.write(placeholderCsv.data(), placeholderCsv.size());
            }
            else if (hasExtension(path, ".dat")) {
                std::string placeholderDat = "placeholder_binary_data";
                outFile.write(placeholderDat.data(), placeholderDat.size());
            }
            else {
                std::string placeholderGeneric = "placeholder";
                outFile.write(placeholderGeneric.data(), placeholderGeneric.size());
            }
            outFile.close();
            LOG_INFO << "Generated placeholder for: " << path;
        }
        else {
            LOG_ERROR << "Failed to create placeholder: " << path;
            callback(HttpResponse::newNotFoundResponse());
            return;
        }

        auto resp = HttpResponse::newFileResponse(path);
        if (hasExtension(path, ".png")) resp->setContentTypeCode(CT_IMAGE_PNG);
        else if (hasExtension(path, ".csv")) resp->setContentTypeCode(CT_TEXT_CSV);
        else if (hasExtension(path, ".dat")) resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        else resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        callback(resp);
    }
    catch (const std::exception& e) {
        LOG_ERROR << "Error generating placeholder: " << e.what();
        callback(HttpResponse::newNotFoundResponse());
    }
}

void BfWebController::HandleDefault(const HttpRequestPtr& rq, std::function<void(const HttpResponsePtr&)>&& callback)
{
    Utils::DumpInfoToDrogon(rq, "WebController");
    callback(HttpResponse::newHttpResponse());
}