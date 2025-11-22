// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SOURCEMANAGER_HPP
#define VUG_SOURCEMANAGER_HPP

#include <string>

class SourceFile {
   public:
    constexpr SourceFile(std::string name, std::string text)
        : mName(std::move(name)),
          mText(std::move(text)) {
        size_t start = 0;

        while (start < mText.size()) {
            size_t end = mText.find_first_of("\r\n", start);

            if (end == std::string::npos) {
                mTextLines.emplace_back(mText.data() + start, mText.size() - start);
                break;
            }

            mTextLines.emplace_back(mText.data() + start, end - start);
            if (mText[end] == '\r' && end + 1 < mText.size() && mText[end + 1] == '\n') {
                end++;
            }

            start = end + 1;
        }
    }

    [[nodiscard]] constexpr const std::string& getName() const {
        return mName;
    }
    [[nodiscard]] constexpr const std::string& getText() const {
        return mText;
    }
    [[nodiscard]] constexpr std::string_view getLine(const uint64_t line) const {
        return mTextLines[line - 1];
    }

   private:
    std::string mName;
    std::string mText;
    std::vector<std::string_view> mTextLines;
};

class SourceManager {
   public:
    SourceManager() = default;

    void insertSourceFile(SourceFile file);
    SourceFile findSourceFile(const std::string& name);

   private:
    std::unordered_map<std::string, SourceFile> mFiles;
};

#endif  // VUG_SOURCEMANAGER_HPP
