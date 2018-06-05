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

#include "jnihelpers.h"

#include <stdio.h>

#include <string>

#include <jni.h>

#include "logging.h"

JNIString::JNIString(JNIEnv* env, jstring java_string)
    : env_(env), java_string_(java_string) {
  string_ = env_->GetStringUTFChars(java_string_, nullptr);
}

JNIString::~JNIString() {
  env_->ReleaseStringUTFChars(java_string_, string_);
}

const char* JNIString::str() const {
  return string_;
}

template <typename T>
class scoped_local_ref {
 public:
  explicit scoped_local_ref(C_JNIEnv* env, T localRef = NULL)
      : mEnv(env), mLocalRef(localRef) {
  }

  ~scoped_local_ref() {
    reset();
  }

  scoped_local_ref(const scoped_local_ref&) = delete;
  void operator=(const scoped_local_ref&) = delete;

  void reset(T localRef = NULL) {
    if (mLocalRef != NULL) {
      (*mEnv)->DeleteLocalRef(reinterpret_cast<JNIEnv*>(mEnv), mLocalRef);
      mLocalRef = localRef;
    }
  }

  T get() const {
    return mLocalRef;
  }

 private:
  C_JNIEnv* const mEnv;
  T mLocalRef;
};

static jclass findClass(C_JNIEnv* env, const char* className) {
  JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
  return (*env)->FindClass(e, className);
}

/*
 * Returns a human-readable summary of an exception object.  The buffer will
 * be populated with the "binary" class name and, if present, the
 * exception message.
 */
static bool getExceptionSummary(C_JNIEnv* env, jthrowable exception,
                                std::string& result) {
  JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
  /* get the name of the exception's class */
  scoped_local_ref<jclass> exceptionClass(
      env, (*env)->GetObjectClass(e, exception));  // can't fail
  scoped_local_ref<jclass> classClass(
      env, (*env)->GetObjectClass(
               e, exceptionClass.get()));  // java.lang.Class, can't fail
  jmethodID classGetNameMethod = (*env)->GetMethodID(
      e, classClass.get(), "getName", "()Ljava/lang/String;");
  scoped_local_ref<jstring> classNameStr(
      env, (jstring)(*env)->CallObjectMethod(e, exceptionClass.get(),
                                             classGetNameMethod));
  if (classNameStr.get() == NULL) {
    (*env)->ExceptionClear(e);
    result = "<error getting class name>";
    return false;
  }
  const char* classNameChars =
      (*env)->GetStringUTFChars(e, classNameStr.get(), NULL);
  if (classNameChars == NULL) {
    (*env)->ExceptionClear(e);
    result = "<error getting class name UTF-8>";
    return false;
  }
  result += classNameChars;
  (*env)->ReleaseStringUTFChars(e, classNameStr.get(), classNameChars);
  /* if the exception has a detail message, get that */
  jmethodID getMessage = (*env)->GetMethodID(
      e, exceptionClass.get(), "getMessage", "()Ljava/lang/String;");
  scoped_local_ref<jstring> messageStr(
      env, (jstring)(*env)->CallObjectMethod(e, exception, getMessage));
  if (messageStr.get() == NULL) {
    return true;
  }
  result += ": ";
  const char* messageChars =
      (*env)->GetStringUTFChars(e, messageStr.get(), NULL);
  if (messageChars != NULL) {
    result += messageChars;
    (*env)->ReleaseStringUTFChars(e, messageStr.get(), messageChars);
  } else {
    result += "<error getting message>";
    (*env)->ExceptionClear(e);  // clear OOM
  }
  return true;
}

int jniThrowException(C_JNIEnv* env, const char* className, const char* msg) {
  JNIEnv* e = reinterpret_cast<JNIEnv*>(env);
  if ((*env)->ExceptionCheck(e)) {
    /* TODO: consider creating the new exception with this as "cause" */
    scoped_local_ref<jthrowable> exception(env, (*env)->ExceptionOccurred(e));
    (*env)->ExceptionClear(e);
    if (exception.get() != NULL) {
      std::string text;
      getExceptionSummary(env, exception.get(), text);
      ALOGW("Discarding pending exception (%s) to throw %s", text.c_str(),
            className);
    }
  }
  scoped_local_ref<jclass> exceptionClass(env, findClass(env, className));
  if (exceptionClass.get() == NULL) {
    ALOGE("Unable to find exception class %s", className);
    /* ClassNotFoundException now pending */
    return -1;
  }
  if ((*env)->ThrowNew(e, exceptionClass.get(), msg) != JNI_OK) {
    ALOGE("Failed throwing '%s' '%s'", className, msg);
    /* an exception, most likely OOM, will now be pending */
    return -1;
  }
  return 0;
}

int jniThrowExceptionFmt(C_JNIEnv* env, const char* className, const char* fmt,
                         va_list args) {
  char msgBuf[512];
  vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
  return jniThrowException(env, className, msgBuf);
}

int JNIThrowExceptionFmt(JNIEnv* env, const char* className, const char* fmt,
                         ...) {
  va_list args;
  va_start(args, fmt);
  return jniThrowExceptionFmt(&env->functions, className, fmt, args);
  va_end(args);
}
