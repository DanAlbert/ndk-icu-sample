/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <exception>
#include <string>
#include <vector>

#include <unicode/udat.h>

class ICUException : public std::runtime_error {
public:
    ICUException(const UErrorCode error);
};

UDate MakeUDate(int year, int month, int day);
std::string UStringToString(const UChar* ustr);
std::vector<UChar> FormatDate(UDate date, const char* locale);
std::string DateToString(UDate date, const char* locale);