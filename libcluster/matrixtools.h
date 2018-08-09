#ifndef _matrixtools_h_
#define _matrixtools_h_

#include <vector>
#include <algorithm>
#include <cmath>


template<typename T> class matrix
{
private:
	std::vector < std::vector<T> > data;

public:
	size_t nrows;
	size_t ncols;

	matrix() { nrows = 0; ncols = 0; }

	matrix(size_t nr, size_t nc)
	{
		nrows = 0;
		ncols = 0;
		data.resize(nr);
		for (size_t r = 0; r < nr; r++)
			data[r].resize(nc);
	}

	matrix(size_t nr, size_t nc, T val)
	{
		nrows = nr;
		ncols = nc;
		data.resize(nr);
		for (size_t r = 0; r < nr; r++)
		{
			data[r].resize(nc);
			for (size_t c = 0; c < nc; c++)
				data[r][c] = val;
		}
	}

	matrix(std::vector<T> vals)
	{
		nrows = 1;
		ncols = vals.size();
		data.assign(1, vals);
	}

	void resize(size_t nr, size_t nc)
	{
		nrows = nr;
		ncols = nc;
		data.resize(nr);
		for (size_t r = 0; r < nr; r++)
			data[r].resize(nc);

		return;
	}

	void resize_fill(size_t nr, size_t nc, T val)
	{
		nrows = nr;
		ncols = nc;
		data.resize(nr);
		for (size_t r = 0; r < nr; r++)
		{
			data[r].resize(nc);
			for (size_t c = 0; c < nc; c++)
				data[r][c] = val;
		}
		return;
	}

	void clear() { resize(0, 0); return; }

	T &at(size_t r, size_t c) { return data[r][c]; }

	const T &at(size_t r, size_t c) const { return data[r][c]; }

	matrix<T> operator= (const matrix<T> &m)
	{
		data = m.data;
		nrows = m.nrows;
		ncols = m.ncols;
		return *this;
	}

	std::vector<T> to_vector()
	{
		size_t ncells = nrows * ncols;
		std::vector<T> alldata(ncells);
		for (size_t r = 0; r < nrows; r++)
		{
			for (size_t c = 0; c < ncols; c++)
				alldata[r*ncols + c] = data[r][c];
		}
		return alldata;
	}

	std::vector<T> sum_rows() const
	{
		std::vector<T> sum(ncols, 0.0);
		for (size_t c = 0; c < ncols; c++)
		{
			for (size_t r = 0; r < nrows; r++)
				sum[c] += data[r][c];
		}
		return sum;
	}

	std::vector<T> sum_cols() const
	{
		std::vector<T> sum(nrows, 0.0);
		for (size_t r = 0; r < nrows; r++)
		{
			for (size_t c = 0; c < ncols; c++)
				sum[r] += data[r][c];
		}
		return sum;
	}

	T min()
	{
		T min = std::numeric_limits<T>::quiet_NaN();
		if (nrows*ncols > 0)
		{
			std::vector<T> alldata = this->to_vector();
			std::sort(alldata.begin(), alldata.end());
			min = alldata[0];
		}
		return min;
	}

	T max()
	{
		T max = std::numeric_limits<T>::quiet_NaN();
		if (nrows*ncols > 0)
		{
			std::vector<T> alldata = this->to_vector();
			std::sort(alldata.begin(), alldata.end());
			max = alldata.back();
		}
		return max;
	}

	double median()
	{
		double median = std::numeric_limits<double>::quiet_NaN();
		size_t ncells = nrows * ncols;

		if (ncells == 1)
			median = (double)(data[0][0]);
		else
		{
			std::vector<T> alldata = this->to_vector();
			std::sort(alldata.begin(), alldata.end());
			if (ncells % 2 == 1)
				median = (double)alldata[(ncells - 1) / 2];
			else
			{
				median = (double)alldata[ncells / 2] / 2.0;
				median += (double)alldata[(ncells / 2) - 1] / 2.0;
			}
		}
		return median;
	}

	void sort_by_index(const std::vector<int> &pos, bool is_rows)
	{

		//Sort according to specified indicies
		//pos[i] = original position of sorted element i
		//is_rows: true = change order of rows, but leave columns in same order
		//	     false = change order of columns, but leave rows in same order


		std::vector<bool> done;
		size_t nsort, ndim;
		if (is_rows)
		{
			nsort = nrows;
			ndim = ncols;
		}
		else
		{
			nsort = ncols;
			ndim = nrows;
		}
		done.assign(nsort, 0);

		for (size_t i = 0; i < nsort; i++)
		{
			if (done[i])
				continue;
			done[i] = true;
			size_t prevj = i;
			size_t j = pos[i];
			while (i != j)
			{
				for (size_t f = 0; f < ndim; f++)
				{
					T val;
					if (is_rows)
					{
						val = data[prevj][f];
						data[prevj][f] = data[j][f];
						data[j][f] = val;
					}
					else
					{
						val = data[f][prevj];
						data[f][prevj] = data[f][j];
						data[f][j] = val;
					}
				}
				done[j] = true;
				prevj = j;
				j = pos[j];
			}
		}

		return;
	}

	std::vector<double> quartiles()
	{
		int percentile[] = { 25,50,75 };
		std::vector<double> vals;
		size_t ncells = nrows * ncols;

		std::vector<T> alldata = this->to_vector();
		std::sort(alldata.begin(), alldata.end());
		vals.push_back(alldata[0]);

		for (size_t p = 0; p < 3; p++)
		{
			double ind = ncells * percentile[p] / 100.;
			int i1, i2;
			if (ind - floor(ind) < 1e-6)
				i1 = i2 = (int)ind;
			else
			{
				i1 = (int)floor(ind);
				i2 = i1 + 1;
			}
			vals.push_back((alldata[i1] + alldata[i2]) / 2.0);
		}
		vals.push_back(alldata.back());
		return vals;
	}

};




#endif
