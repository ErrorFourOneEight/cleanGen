#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tstring.h>

namespace fs = std::filesystem;

const std::string PREFIX = "[CLEAN] ";

int main() {
    std::vector<fs::path> mp3files;

    // Collect all .mp3 files in the current directory
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // Case-insensitive extension check
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".mp3") {
                mp3files.push_back(entry.path());
            }
        }
    }

    if (mp3files.empty()) {
        std::cout << "No .mp3 files found in the current directory." << std::endl;
        return 0;
    }

    std::cout << "Found " << mp3files.size() << " MP3 file(s).\n" << std::endl;

    int success = 0, skipped = 0, failed = 0;

    for (const auto& path : mp3files) {
        std::string filename  = path.filename().string();
        std::string stem      = path.stem().string();
        std::string parentStr = path.parent_path().string();

        // --- 1. Update the ID3 title tag ---
        TagLib::FileRef f(path.c_str());

        if (f.isNull() || !f.tag()) {
            std::cerr << "  [SKIP]  Could not read tags: " << filename << std::endl;
            ++skipped;
            continue;
        }

        TagLib::Tag* tag = f.tag();
        std::string currentTitle = tag->title().to8Bit(true);

        // Only prefix if not already prefixed
        if (currentTitle.rfind(PREFIX, 0) != 0) {
            std::string newTitle = PREFIX + currentTitle;
            tag->setTitle(TagLib::String(newTitle, TagLib::String::UTF8));
            if (!f.save()) {
                std::cerr << "  [FAIL]  Could not save tags: " << filename << std::endl;
                ++failed;
                continue;
            }
        } else {
            std::cout << "  [SKIP]  Tag already prefixed: " << filename << std::endl;
        }

        // --- 2. Rename the file ---
        if (filename.rfind(PREFIX, 0) != 0) {
            std::string newFilename = PREFIX + filename;
            fs::path newPath = path.parent_path() / newFilename;

            std::error_code ec;
            fs::rename(path, newPath, ec);
            if (ec) {
                std::cerr << "  [FAIL]  Could not rename: " << filename
                          << " -> " << ec.message() << std::endl;
                ++failed;
                continue;
            }
            std::cout << "  [OK]    " << filename << "  ->  " << newFilename << std::endl;
        } else {
            std::cout << "  [SKIP]  Filename already prefixed: " << filename << std::endl;
        }

        ++success;
    }

    std::cout << "\nDone.  Processed: " << success
              << "  Skipped: " << skipped
              << "  Failed: "  << failed << std::endl;

    return (failed > 0) ? 1 : 0;
}
