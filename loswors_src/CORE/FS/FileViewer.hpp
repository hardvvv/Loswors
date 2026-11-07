#pragma once

#include <string>
#include <vector>

namespace FileViewer {

    /**
     * Selects a file for writing to an existing buffer of char*.
     * @param outPath Buffer to store the path (used as the initial path if it's not empty).
     * @param maxLen Full size of the allocated buffer outPath in bytes.
     * @param title Window title.
     * @param filterPatterns Array of file extensions, e.g.: {"*.png", "*.jpg"}
     * @param filterDescription Description of the filter, e.g.: "Images"
     * @return true if a file is selected, false if the selection is canceled (outPath remains unchanged).
     */
    bool OpenFile(
        char* outPath,
        int maxLen,
        const char* title,
        const std::vector<std::string>& filterPatterns = {},
        const char* filterDescription = nullptr
    );

    /**
     * Selects a folder and writes the selected path to the buffer.
     * @param outPath Buffer to write the path to (used as the initial path if it's not empty).
     * @param maxLen Total size of the allocated buffer outPath in bytes.
     * @param title Title of the window.
     * @return true if a folder is selected, false if the selection is canceled (outPath remains unchanged).
     */
    bool OpenFolder(
        char* outPath,
        int maxLen,
        const char* title = "Select folder:"
    );

}