/*
 * PagedVector.hpp
 *
 *  Created on: 29 Oct 2021
 *      Author: julianporter
 */

#ifndef PAGEDVECTOR_HPP_
#define PAGEDVECTOR_HPP_

#include "base.hpp"

namespace meromorph {
namespace switchnmix {

template<typename T>
class PagedVector {
public:
	using size_t = uint32;
	using ssize_t = int32;

	using iterator = T*;
private:
	size_t pageSize;
	uint32 nPages;
	size_t size;
	std::vector<T> buffer;

	size_t index(const uint32 page,const ssize_t offset) {
		auto o = offset+(ssize_t)(page*pageSize);
		while(o>size) o-=size;
		while(o<0) o+=size;
		return (size_t)o;
	}

	size_t pageIndex(const ssize_t page) {
		auto o = page*(ssize_t)pageSize;
		while(o<0) o+=size;
		while(o>size) o-=size;
		return (size_t)o;
	}

public:

	PagedVector(const size_t pageSize_,const uint32 nPages_,const T def) :
		pageSize(pageSize_), nPages(nPages_), size(pageSize*nPages),
		buffer(size,def)
		{}
	virtual ~PagedVector() = default;
	PagedVector(const PagedVector &other) = default;
	PagedVector(PagedVector &&other) = default;
	PagedVector& operator=(const PagedVector &other) = default;
	PagedVector& operator=(PagedVector &&other) = default;

	T & at(const ssize_t page,const size_t offset) { return buffer.at(pageIndex(page)+offset); }
	T * data(const ssize_t page) noexcept { return buffer.data()+pageIndex(page); }

	iterator begin(const ssize_t page) { return data(page); }
	iterator end(const ssize_t page) { return data(page)+pageSize; }

	void fillPage(const uint32 page,const T value) {
		auto b=begin(page);
		auto e=end(page);
		for(auto it=b;it<e;it++) *it=value;
	}


	void reset(const T value) {
		buffer.assign(size,value);
	}
};

template<typename T>
class AutoPagedVector {
public:
	using size_t = uint32;
	using ssize_t = int32;

	using iterator = T*;
private:
	size_t pageSize;
	uint32 nPages;
	size_t size;
	ssize_t page;
	std::vector<T> buffer;

	size_t index(const ssize_t offset) {
		auto o = offset+(ssize_t)(page*pageSize);
		while(o>size) o-=size;
		while(o<0) o+=size;
		return (size_t)o;
	}

	size_t pageIndex(const ssize_t pge=0) {
			auto o = (page+pge)*(ssize_t)pageSize;
			while(o<0) o+=size;
			while(o>size) o-=size;
			return (size_t)o;
		}

public:

	AutoPagedVector(const size_t pageSize_,const uint32 nPages_,const T def) :
		pageSize(pageSize_), nPages(nPages_), size(pageSize*nPages), page(0),
		buffer(size,def) {}
	virtual ~AutoPagedVector() = default;
	AutoPagedVector(const AutoPagedVector &other) = default;
	AutoPagedVector(AutoPagedVector &&other) = default;
	AutoPagedVector& operator=(const AutoPagedVector &other) = default;
	AutoPagedVector& operator=(AutoPagedVector &&other) = default;

	T & at(const ssize_t pge,const size_t offset) { return buffer.at(pageIndex(pge)+offset); }
	T & at(const size_t offset) { return buffer.at(pageIndex()+offset); }
	T & operator()(const ssize_t pge,const size_t offset) { return buffer[pageIndex(pge)+offset]; }
	T & operator()(const size_t offset) { return buffer[pageIndex()+offset]; }

	T & operator[](const size_t offset) { return buffer[pageIndex()+offset]; }
	T * data(const ssize_t pge=0) noexcept { return buffer.data()+pageIndex(pge); }

	iterator begin(const ssize_t pge=0) { return data(pge); }
	iterator end(const ssize_t pge=0) { return data(pge)+pageSize; }

	void step() { page=(page+1)%nPages; }

	void fillPage(const T value) {
		auto b=begin();
		auto e=end();
		for(auto it=b;it<e;it++) *it=value;

	}

	void reset(const T value) {
		buffer.assign(size,value);
		page=0;
	}
};

} /* namespace switchnmix */
} /* namespace meromorph */

#endif /* PAGEDVECTOR_HPP_ */
