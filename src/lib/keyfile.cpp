/*
 * Copyright (C) 2022 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     weizhixiang <weizhixiang@uniontech.com>
 *
 * Maintainer: weizhixiang <weizhixiang@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "keyfile.h"
#include "dlocale.h"
#include "dstring.h"
#include "macro.h"

#include <cstring>
#include <string>
#include <iostream>

KeyFile::KeyFile(char separtor)
 : fp(nullptr)
 , modified(false)
 , listSeparator(separtor)
{
}

KeyFile::~KeyFile()
{
    if (fp) {
        fclose(fp);
        fp = nullptr;
    }
}

bool KeyFile::getBool(const std::string &section, const std::string &key, bool defaultValue)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return false;

    std::string valueStr = mainKeyMap[section][key];
    bool value = defaultValue;
    if (valueStr == "true")
        value = true;
    else if (valueStr == "false")
        value = false;

    return value;
}

// TODO
std::vector<bool> KeyFile::getBoolList(const std::string &section, const std::string &key, bool defaultValue)
{
    std::vector<bool> tmp;
    return tmp;
}

int KeyFile::getInt(const std::string &section, const std::string &key, int defaultValue)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return defaultValue;

    std::string valueStr = mainKeyMap[section][key];
    int value;
    try {
        value = std::stoi(valueStr);
    } catch (std::invalid_argument&) {
        value = defaultValue;
    }

    return value;
}

// TODO
std::vector<int> KeyFile::getIntList(const std::string &section, const std::string &key, int defaultValue)
{
    std::vector<int> tmp;
    return tmp;
}

// TODO
int64_t KeyFile::getInt64(const std::string &section, const std::string &key, int64_t defaultValue)
{
    return int64_t(0);
}

// TODO
float KeyFile::getFloat(const std::string &section, const std::string &key, float defaultValue)
{
    return 1.0;
}

std::string KeyFile::getStr(const std::string &section, const std::string &key, std::string defaultValue)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return defaultValue;

    std::string valueStr = mainKeyMap[section][key];
    if (valueStr.empty())
        valueStr = defaultValue;

    return valueStr;
}

bool KeyFile::containKey(const std::string &section, const std::string &key)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return false;

    return mainKeyMap[section].find(key) != mainKeyMap[section].end();
}

std::string KeyFile::getLocaleStr(const std::string &section, const std::string &key, std::string defaultLocale)
{
    std::vector<std::string> languages = defaultLocale.empty()
     ? Locale::instance()->getLanguageNames()
     : Locale::instance()->getLocaleVariants(defaultLocale);

    std::string translated;
    for (const auto &lang : languages) {
        translated.assign(getStr(section, key + "[" + lang + "]"));
        if (!translated.empty())
            return translated;
    }

    // NOTE: not support key Gettext-Domain
	// fallback to default key
	return getStr(section, key);
}

std::vector<std::string> KeyFile::getStrList(const std::string &section, const std::string &key)
{
    std::string value = getStr(section, key);
    return DString::splitStr(value, listSeparator);
}

std::vector<std::string> KeyFile::getLocaleStrList(const std::string &section, const std::string &key, std::string defaultLocale)
{
    std::vector<std::string> languages = defaultLocale.empty()
     ? Locale::instance()->getLanguageNames()
     : Locale::instance()->getLocaleVariants(defaultLocale);

    std::vector<std::string> translated;
    for (const auto &lang : languages) {
        translated = getStrList(section, key + "[" + lang + "]");
        if (translated.size() > 0)
            return translated;
    }

    //fallback to default key
    return getStrList(section, key);
}

// 修改keyfile内容
void KeyFile::setKey(const std::string &section, const std::string &key, const std::string &value)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        mainKeyMap.insert({section, KeyMap()});

    mainKeyMap[section].insert({key, value});
}

// 写入文件
bool KeyFile::saveToFile(const std::string &filePath)
{
    FILE *sfp = fopen(filePath.data(), "w+");
    if (!sfp)
        return false;


    for (const auto &im : mainKeyMap) {
        const auto &keyMap = im.second;
        std::string section = "[" + im.first + "]\n";
        fputs(section.c_str(), sfp);
        for (const auto &ik : keyMap) {
            std::string kv = ik.first + "=" + ik.second + "\n";
            fputs(kv.c_str(), sfp);
        }
    }

    fclose(sfp);
    return true;
}

bool KeyFile::loadFile(const std::string &filePath)
{
    mainKeyMap.clear();
    if (fp) {
        fclose(fp);
        fp = nullptr;
    }

    std::string lastSection;
    fp = fopen(filePath.data(), "r");
    if (!fp)
        return false;

    char line[MAX_LINE_LEN] = {0};
    while (fgets(line, MAX_LINE_LEN, fp)) {
        char *start = &line[0];
        char *end = start;
        while (!strneq(end, "\0", 1))
            end++;

        end--; // 返回'\0'前一个字符

        // 移除行首
        while (strneq(start, " ", 1) || strneq(start, "\t", 1))
            start++;

        // 过滤注释行
        if (strneq(start, "#", 1))
            continue;

        // 移除行尾
        while (strneq(end, "\n", 1) || strneq(end, "\r", 1)
            || strneq(end, " ", 1) || strneq(end, "\t", 1))
            end--;

        char *lPos = strchr(start, '[');
        char *rPos = strchr(start, ']');
        if (lPos && rPos && rPos - lPos > 0 && lPos == start && rPos == end) {
            // 主键
            std::string section(lPos + 1, size_t(rPos - lPos - 1));
            mainKeyMap.insert({section, KeyMap()});
            lastSection = section;
        } else {
            char *equal = strchr(start, '=');
            if (!equal)
                continue;

            // 文件格式错误
            if (lastSection.empty()) {
                std::cout << "failed to load file " << filePath << std::endl;
                return false;
            }

            // 子键
            std::string key(start, size_t(equal - start));
            std::string value(equal + 1, size_t(end - equal));
            for (auto &iter : mainKeyMap) {
                if (iter.first != lastSection)
                    continue;

                iter.second[key] = value;
            }
        }
    }
    fclose(fp);
    fp = nullptr;

    return true;
}

std::vector<std::string> KeyFile::getMainKeys()
{
    std::vector<std::string> mainKeys;
    for (const auto &iter : mainKeyMap)
        mainKeys.push_back(iter.first);

    return mainKeys;
}

void KeyFile::print()
{
    std::cout << "sectionMap: " << std::endl;
    for (auto sectionMap : mainKeyMap) {
        std::cout << "section=" << sectionMap.first << std::endl;
        KeyMap keyMap = sectionMap.second;

        for (auto iter : keyMap) {
            std::cout << iter.first << "=" << iter.second << std::endl;
        }

        std::cout << std::endl;
    }
}


