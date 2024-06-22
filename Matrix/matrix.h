#ifndef MAT_H
#define MAT_H
#include <vector>
template<typename T>
class mat {
	int rows_;
	int cols_;
	std::vector<T> data_;
public:
	mat(int r = 0, int c = 0) : rows_(r), cols_(c), data_(r* c) {}

	int rows() const { return rows_; }
	int cols() const { return cols_; }
	int size() const { return rows_ * cols_; }
	int rawsize() const { return rows_ * cols_ * sizeof(T); }

	bool empty() const { return rows_ == 0 && cols_ == 0; }

	T& operator[](int i) { return data_[i]; }
	const T& operator[](int i) const { return data_[i]; }

	T& operator()(int r, int c) { return data_[r * cols_ + c]; }
	const T& operator()(int r, int c) const { return data_[r * cols_ + c]; }

	auto& data() { return data_; }
	auto& data() const { return data_; }

	void resize(int r, int c) {
		cols_ = c;
		rows_ = r;
		data_.resize(r * c);
	}

	char* rawdata() { return reinterpret_cast<char*> (data_.data()); }
	const char* rawdata() const { return reinterpret_cast<const char*> (data_.data()); }

	auto begin() { return data_.begin(); }
	auto begin() const { return data_.begin(); }
	auto end() { return data_.end(); }
	auto end() const { return data_.end(); }


};
#endif // !MAT_H
