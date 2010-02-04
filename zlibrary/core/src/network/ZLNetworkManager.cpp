/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <map>
#include <set>

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLOutputStream.h>
#include <ZLResource.h>
#include <ZLibrary.h>
#include <ZLXMLReader.h>

#include "ZLNetworkManager.h"
#include "ZLNetworkRequest.h"
#include "ZLNetworkReader.h"
#include "ZLNetworkUtil.h"

#include "requests/ZLNetworkDownloadRequest.h"
#include "requests/ZLNetworkNoActionRequest.h"
#include "requests/ZLNetworkPostFormRequest.h"
#include "requests/ZLNetworkReadResponseRequest.h"
#include "requests/ZLNetworkReadToStringRequest.h"
#include "requests/ZLNetworkXMLParserRequest.h"


ZLNetworkManager *ZLNetworkManager::ourInstance = 0;

void ZLNetworkManager::deleteInstance() {
	if (ourInstance != 0) {
		delete ourInstance;
	}
}

ZLNetworkManager &ZLNetworkManager::Instance() {
	return *ourInstance;
}

std::string ZLNetworkManager::CacheDirectory() {
	return
		ZLibrary::ApplicationWritableDirectory() +
		ZLibrary::FileNameDelimiter +
		"cache";
}

std::string ZLNetworkManager::CookiesPath() {
	return ZLFile(CacheDirectory() + ZLibrary::FileNameDelimiter + "cookies").path();
}

ZLNetworkManager::ZLNetworkManager() {
}

static const std::string OPTIONS = "Options";

ZLIntegerRangeOption &ZLNetworkManager::ConnectTimeoutOption() const {
	if (myConnectTimeoutOption.isNull()) {
		myConnectTimeoutOption = new ZLIntegerRangeOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ConnectTimeout", 1, 1000, 10
		);
	}
	return *myConnectTimeoutOption;
}

ZLIntegerRangeOption &ZLNetworkManager::TimeoutOption() const {
	if (myTimeoutOption.isNull()) {
		myTimeoutOption = new ZLIntegerRangeOption(
			ZLCategoryKey::NETWORK, OPTIONS, "Timeout", 1, 1000, 15
		);
	}
	return *myTimeoutOption;
}

ZLBooleanOption &ZLNetworkManager::UseProxyOption() const {
	if (myUseProxyOption.isNull()) {
		myUseProxyOption = new ZLBooleanOption(
			ZLCategoryKey::NETWORK, OPTIONS, "UseProxy", false
		);
	}
	return *myUseProxyOption;
}

ZLStringOption &ZLNetworkManager::ProxyHostOption() const {
	if (myProxyHostOption.isNull()) {
		myProxyHostOption = new ZLStringOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ProxyHost", ""
		);
	}
	return *myProxyHostOption;
}

ZLStringOption &ZLNetworkManager::ProxyPortOption() const {
	if (myProxyPortOption.isNull()) {
		myProxyPortOption = new ZLStringOption(
			ZLCategoryKey::NETWORK, OPTIONS, "ProxyPort", "3128"
		);
	}
	return *myProxyPortOption;
}

ZLNetworkManager::~ZLNetworkManager() {
}

bool ZLNetworkManager::connect() const {
	return true;
}

void ZLNetworkManager::release() const {
}

bool ZLNetworkManager::providesProxyInfo() const {
	return false;
}

bool ZLNetworkManager::useProxy() const {
	return UseProxyOption().value();
}

std::string ZLNetworkManager::proxyHost() const {
	return ProxyHostOption().value();
}

std::string ZLNetworkManager::proxyPort() const {
	return ProxyPortOption().value();
}

std::string ZLNetworkManager::downloadFile(const std::string &url, const std::string &fileName, shared_ptr<ZLExecutionData::Listener> listener) const {
	return downloadFile(url, ZLNetworkSSLCertificate(), fileName, listener);
}

std::string ZLNetworkManager::downloadFile(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName, shared_ptr<ZLExecutionData::Listener> listener) const {
	ZLFile fileToWrite(fileName);
	shared_ptr<ZLOutputStream> stream = fileToWrite.outputStream(true);
	if (stream.isNull() || !stream->open()) {
		const ZLResource &errorResource =
			ZLResource::resource("dialog")["networkError"];
		return
			ZLStringUtil::printf(
				errorResource["couldntCreateFileMessage"].value(), fileName
			);
	}
	ZLExecutionData::Vector dataVector;
	shared_ptr<ZLExecutionData> data = createDownloadRequest(url, sslCertificate, fileName, stream);
	data->setListener(listener);
	dataVector.push_back(data);
	return perform(dataVector);
}



shared_ptr<ZLExecutionData> ZLNetworkManager::createDownloadRequest(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream) const {
	return new ZLNetworkDownloadRequest(url, ZLNetworkSSLCertificate(), fileName, stream);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createDownloadRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::string &fileName, shared_ptr<ZLOutputStream> stream) const {
	return new ZLNetworkDownloadRequest(url, sslCertificate, fileName, stream);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createNoActionRequest(const std::string &url) const {
	return new ZLNetworkNoActionRequest(url, ZLNetworkSSLCertificate());
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createNoActionRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate) const {
	return new ZLNetworkNoActionRequest(url, sslCertificate);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createPostFormRequest(const std::string &url, const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader) const {
	return new ZLNetworkPostFormRequest(url, ZLNetworkSSLCertificate(), formData, reader);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createPostFormRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader) const {
	return new ZLNetworkPostFormRequest(url, sslCertificate, formData, reader);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createReadResponseRequest(const std::string &url, shared_ptr<ZLNetworkReader> reader) const {
	return new ZLNetworkReadResponseRequest(url, ZLNetworkSSLCertificate(), reader);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createReadResponseRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLNetworkReader> reader) const {
	return new ZLNetworkReadResponseRequest(url, sslCertificate, reader);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createReadToStringRequest(const std::string &url, std::string &buffer) const {
	return new ZLNetworkReadToStringRequest(url, ZLNetworkSSLCertificate(), buffer);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createReadToStringRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, std::string &buffer) const {
	return new ZLNetworkReadToStringRequest(url, sslCertificate, buffer);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createXMLParserRequest(const std::string &url, shared_ptr<ZLXMLReader> reader) const {
	return new ZLNetworkXMLParserRequest(url, ZLNetworkSSLCertificate(), reader);
}

shared_ptr<ZLExecutionData> ZLNetworkManager::createXMLParserRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, shared_ptr<ZLXMLReader> reader) const {
	return new ZLNetworkXMLParserRequest(url, sslCertificate, reader);
}
