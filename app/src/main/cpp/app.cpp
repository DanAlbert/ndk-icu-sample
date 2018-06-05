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

#include <jni.h>
#include <unicode/udat.h>

#include "calendar.h"
#include "jnihelpers.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_icu4csample_MainActivity_getDateString(JNIEnv* env,
                                                        jobject /* this */,
                                                        int year, int month,
                                                        int day,
                                                        jstring jlocale) {
  JNIString locale(env, jlocale);

  try {
    UDate date = MakeUDate(year, month, day);
    return env->NewStringUTF(DateToString(date, locale.str()).c_str());
  } catch (const ICUException& ex) {
    JNIThrowExceptionFmt(env, "java/lang/RuntimeException", "ICU Error: %s",
                         ex.what());
    return nullptr;
  }
}
