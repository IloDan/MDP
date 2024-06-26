#pragma once

#include <iostream>
#include <vector>

template <typename T>
class mat {
	int rows_, cols_;
	std::vector<T> data_;
	T default_val = T();
public:
	mat(int rows=0, int cols=0) : rows_(rows), cols_(cols), data_(rows * cols) {}

	auto rows() const { return rows_; }
	auto cols() const { return cols_; }
	auto size() const { return rows_ * cols_; }

	void resize(int rows, int cols) {
		rows_ = rows;
		cols_ = cols;
		data_.resize(rows * cols);
	}

	auto rawsize() const {
		return rows_ * cols_ * sizeof(T);
	}

	T& operator[](int i) { return data_[i]; }
	const T& operator[](int i) const { return data_[i]; }

	T& operator()(int r, int c) { 
		if (r<0 || r>=rows_ || c<0 || c>=cols_)
			return default_val;
		return data_[r * cols_ + c]; }
	const T& operator()(int r, int c) const {
		if (r<0 || r>=rows_ || c<0 || c>=cols_)
			return default_val;
		return data_[r * cols_ + c];
	}

	auto rawData() { return reinterpret_cast<char*>(data_.data()); }
	const auto rawData() const { return reinterpret_cast<const char*>(data_.data()); }

};