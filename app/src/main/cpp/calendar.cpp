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

#include "calendar.h"

#include <memory>

#include <unicode/umsg.h>
#include <unicode/ustring.h>

ICUException::ICUException(const UErrorCode error) : std::runtime_error(u_errorName(error)) {
}

UDate MakeUDate(int year, int month, int day) {
    UErrorCode status = U_ZERO_ERROR;
    std::unique_ptr<UCalendar, decltype(&ucal_close)> cal(
            ucal_open(nullptr, 0, "en_US", UCAL_GREGORIAN, &status), &ucal_close);
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }

    status = U_ZERO_ERROR;
    ucal_setDate(cal.get(), year, month, day, &status);
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }

    status = U_ZERO_ERROR;
    UDate date = ucal_getMillis(cal.get(), &status);
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }

    return date;
}

std::string UStringToString(const UChar* ustr) {
    int32_t utf8_len = 0;
    UErrorCode status = U_ZERO_ERROR;
    u_strToUTF8(nullptr, 0, &utf8_len, ustr, -1, &status);
    if (status != U_BUFFER_OVERFLOW_ERROR) {
        throw ICUException(status);
    }

    std::string str;
    str.resize(utf8_len);
    status = U_ZERO_ERROR;
    u_strToUTF8(str.data(), str.size(), nullptr, ustr, -1, &status);
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }
    return str;
}

std::vector<UChar> FormatDate(UDate date, const char* locale) {
    UErrorCode status = U_ZERO_ERROR;
    UChar fmt[] = u"{0, date, long}";
    int32_t formatted_len = u_formatMessage(locale, fmt, u_strlen(fmt), nullptr, 0, &status, date);
    if (status != U_BUFFER_OVERFLOW_ERROR) {
        throw ICUException(status);
    }

    status = U_ZERO_ERROR;
    std::vector<UChar> formatted(formatted_len + 1);
    u_formatMessage(locale, fmt, u_strlen(fmt), formatted.data(), formatted_len, &status, date);
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }

    return formatted;
}

std::string DateToString(UDate date, const char* locale) {
    std::vector<UChar> formatted = FormatDate(date, locale);
    return UStringToString(formatted.data());
}