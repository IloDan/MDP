#ifndef MAT_H
#define MAT_H
#include <vector>
#include <cassert>
template <typename T>
struct mat {
	int rows_, cols_;
	std::vector<T> data_;

	mat(int rows = 0, int cols = 0) : rows_(rows), cols_(cols), data_(rows* cols) {}

	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return rows_ * cols_; }
	size_t raw_size() const { return rows_ * cols_ * sizeof(T); }

	void resize(int rows, int cols) {
		rows_ = rows;
		cols_ = cols;
		data_.resize(rows * cols);
	}

	const T& operator()(int r, int c) const {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	T& operator()(int r, int c) {
		assert(r >= 0 && r < rows_ && c >= 0 && c < cols_);
		return data_[r * cols_ + c];
	}

	const char* raw_data() const {
		return reinterpret_cast<const char*>(&data_[0]);
	}
	
	char* raw_data() {
		return reinterpret_cast<char*>(&data_[0]);
	}
};

#endif // !MAT_H
