// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SOURCEMANAGER_HPP
#define VUG_SOURCEMANAGER_HPP

#include <string>

class SourceFile {
   public:
    SourceFile(std::string name, std::string text)
        : _name(std::move(name)),
          _text(std::move(text)) {
        size_t start = 0;

        while (start < _text.size()) {
            size_t end = _text.find_first_of("\r\n", start);

            if (end == std::string::npos) {
                _textLines.emplace_back(_text.data() + start, _text.size() - start);
                break;
            }

            _textLines.emplace_back(_text.data() + start, end - start);
            if (_text[end] == '\r' && end + 1 < _text.size() && _text[end + 1] == '\n') {
                end++;
            }

            start = end + 1;
        }
    }

    [[nodiscard]] const std::string& getName() const {
        return _name;
    }
    [[nodiscard]] const std::string& getText() const {
        return _text;
    }
    [[nodiscard]] inline std::string_view getLine(uint64_t line) const {
        return _textLines[line - 1];
    }

   private:
    std::string _name;
    std::string _text;
    std::vector<std::string_view> _textLines;
};

class SourceManager {
   public:
    SourceManager() = default;

    void insertSourceFile(SourceFile file);
    SourceFile findSourceFile(const std::string& name);

   private:
    std::unordered_map<std::string, SourceFile> _files;
};

#endif  // VUG_SOURCEMANAGER_HPP
