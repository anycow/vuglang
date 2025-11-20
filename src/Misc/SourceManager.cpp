// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "SourceManager.hpp"

void SourceManager::insertSourceFile(SourceFile file) {
    _files.insert({file.getName(), std::move(file)});
}

SourceFile SourceManager::findSourceFile(const std::string& file) {
    return _files.find(file)->second;
}
