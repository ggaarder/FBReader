/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLNETWORKPOSTFORMREQUEST_H__
#define __ZLNETWORKPOSTFORMREQUEST_H__

#include "../ZLNetworkRequest.h"


class ZLNetworkReader;

class ZLNetworkPostFormRequest : public ZLNetworkPostRequest {

public:
	ZLNetworkPostFormRequest(const std::string &url, const ZLNetworkSSLCertificate &sslCertificate, 
		const std::vector<std::pair<std::string, std::string> > &formData, shared_ptr<ZLNetworkReader> reader);

private:
	bool handleHeader(void *ptr, size_t size);
	bool handleContent(void *ptr, size_t size);

	bool doBefore();
	void doAfter(bool success);

private:
	shared_ptr<ZLNetworkReader> myReader;
};

#endif /* __ZLNETWORKPOSTFORMREQUEST_H__ */
