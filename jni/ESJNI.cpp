//
//  ESJNI.cpp
//
//  Created by Steve Pucci 09 Jun 2011
//  Copyright Emerald Sequoia LLC 2011. All rights reserved.
//

#include "ESJNI.hpp"
#include "ESJNIDefs.hpp"

#include "ESErrorReporter.hpp"

ESJNIBase::ESJNIBase()
:   _retained(false),
    _javaObject(NULL)
{}


ESJNIBase::ESJNIBase(jobject jobj)
:   _retained(false),
    _javaObject(jobj)    
{
}

 
ESJNIBase::ESJNIBase(jobject jobj,
                     bool    retained)
:   _retained(retained),
    _javaObject(jobj)    
{
}

 
ESJNIBase::ESJNIBase(const ESJNIBase &other)
:   _retained(other._retained),
    _javaObject(other._javaObject)
{
}

void 
ESJNIBase::release(JNIEnv *jniEnv) {
    ESAssert(_retained);
    if (_retained) {
        ESAssert(_javaObject);
        jniEnv->DeleteGlobalRef(_javaObject);
        _javaObject = NULL;
        _retained = false;  // Note that this does nothing to other glue-handle objects which might be pointed at the retained ptr
    }
}

void 
ESJNIBase::DeleteLocalRef(JNIEnv *jniEnv) {
    ESAssert(!_retained);
    ESAssert(_javaObject);
    jniEnv->DeleteLocalRef(_javaObject);
    _javaObject = NULL;
}

ESJNI_java_lang_String::ESJNI_java_lang_String()
:   _esStringInitialized(false),
    _javaStringRetained(false),
    _javaString(NULL)
{
}

ESJNI_java_lang_String::ESJNI_java_lang_String(jobject js)
:   _esStringInitialized(false),
    _javaStringRetained(false),
    _javaString((jstring)js)
{
}

ESJNI_java_lang_String::ESJNI_java_lang_String(jstring js)
:   _esStringInitialized(false),
    _javaStringRetained(false),
    _javaString(js)
{
}

ESJNI_java_lang_String::ESJNI_java_lang_String(const std::string &str)
:   _esStringInitialized(true),
    _esString(str),
    _javaStringRetained(false),
    _javaString(NULL)
{
}
 
ESJNI_java_lang_String::ESJNI_java_lang_String(const char *str)
:   _esStringInitialized(true),
    _esString(str),
    _javaStringRetained(false),
    _javaString(NULL)
{
}
 
ESJNI_java_lang_String::ESJNI_java_lang_String(const ESJNI_java_lang_String &other)
:   _esStringInitialized(other._esStringInitialized),
    _esString(other._esString),
    _javaStringRetained(false),
    _javaString(other._javaString)
{
    ESAssert(!other._javaStringRetained);
}

ESJNI_java_lang_String::~ESJNI_java_lang_String() {
    ESAssert(!_javaStringRetained);  // Can't release without jniEnv
}

jobject 
ESJNI_java_lang_String::toJObject(JNIEnv *jniEnv) {
    if (_javaString) {
        return _javaString;
    }
    if (_esStringInitialized) {
        return (jobject)jniEnv->NewStringUTF(_esString.c_str());
    }
    return NULL;
}

jstring 
ESJNI_java_lang_String::toJString(JNIEnv *jniEnv) {
    if (_javaString) {
        return _javaString;
    }
    if (_esStringInitialized) {
        return jniEnv->NewStringUTF(_esString.c_str());
    }
    return NULL;
}

void 
ESJNI_java_lang_String::retainJString(JNIEnv *jniEnv) {
    ESAssert(!_javaStringRetained);
    if (_javaString) {
        _javaString = (jstring)jniEnv->NewGlobalRef(_javaString);
        _javaStringRetained = true;
    } else if (_esStringInitialized) {
        _javaString = (jstring)jniEnv->NewGlobalRef((jobject)jniEnv->NewStringUTF(_esString.c_str()));
        _javaStringRetained = true;
    } else {
        ESAssert(false);  // Nothing to retain
    }
}

void 
ESJNI_java_lang_String::releaseJString(JNIEnv *jniEnv) {
    ESAssert(_javaStringRetained);
    ESAssert(_javaString);
    jniEnv->DeleteGlobalRef(_javaString);
    _javaString = NULL;
}

std::string 
ESJNI_java_lang_String::toESString(JNIEnv *jniEnv) {
    if (!_esStringInitialized) {
        if (_javaString) {
            const char *utf = jniEnv->GetStringUTFChars(_javaString, NULL);
            _esString = utf;
            _esStringInitialized = true;
            jniEnv->ReleaseStringUTFChars(_javaString, utf);
        } else {
            return "";
        }
    }
    ESAssert(_esStringInitialized);
    return _esString;
}

ESJNI_java_lang_String &
ESJNI_java_lang_String::operator=(ESJNI_java_lang_String &other) {
    ESAssert(!_javaString);  // One-time use for now
    _javaString = other._javaString;
    _javaStringRetained = other._javaStringRetained;
    _esString = other._esString;
    _esStringInitialized = other._esStringInitialized;
    return *this;
}

ESJNI_java_lang_String &
ESJNI_java_lang_String::operator=(const std::string &str) {
    ESAssert(!_javaString);  // One-time use for now
    _esStringInitialized = true;
    _esString = str;
    _javaString = NULL;
    _javaStringRetained = false;
    return *this;
}

ESJNI_java_lang_String &
ESJNI_java_lang_String::operator=(const char *str) {
    ESAssert(!_javaString);  // One-time use for now
    _esStringInitialized = true;
    _esString = str;
    _javaString = NULL;
    _javaStringRetained = false;
    return *this;
}

ESJNI_java_lang_String &
ESJNI_java_lang_String::operator=(jstring js) {
    ESAssert(!_javaString);  // One-time use for now
    _esStringInitialized = false;
    _javaString = js;
    _javaStringRetained = false;
    return *this;
}

ESJNI_java_lang_String &
ESJNI_java_lang_String::operator=(jobject js) {
    ESAssert(!_javaString);  // One-time use for now
    _esStringInitialized = false;
    _javaString = (jstring)js;
    _javaStringRetained = false;
    return *this;
}

ESJNI_java_lang_CharSequence
ESJNI_java_lang_String::toCharSequence(JNIEnv *jniEnv) {
    if (_javaString) {
        return ESJNI_java_lang_CharSequence(_javaString, _javaStringRetained);  // a String isa CharSequence
    } else {
        return ESJNI_java_lang_CharSequence(toJString(jniEnv), _javaStringRetained);  // a String isa CharSequence
    }
}
