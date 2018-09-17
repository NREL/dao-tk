#ifndef _OPTUTIL_

#include <eigen/Core>
#include <eigen/Dense>
#include <exception>
#include <vector>
#include <set>

template <typename T>
class Vector : public std::vector< T >
{
public:
    Vector(){};
    Vector(int len)
    {
        this->resize(len);
    };

    Vector(int len, const T& init)
    {
        this->resize(len, init);
    };
    
    static Vector<T> Zeros(int len)
    {
        return Vector<T>(len, (T)0);
    };

    static Vector<T> Ones(int len)
    {
        return Vector<T>(len, (T)1);
    };

    T maxCoeff()
    {
        return *std::max(this->begin(), this->end());
    };

    T minCoeff()
    {
        return *std::min(this->begin(), this->end());
    };

    T& operator()(int index){return this->at(index);};

    T operator()(int index) const {return this->at(index);};
    
    Vector<T>& operator=(const Eigen::Matrix<T, Eigen::Dynamic, 1> &other)
    {

        for(size_t i=0; i<other.size(); i++)
            this->at(i) = other(i);
        return &(*this);
    };

    Eigen::Matrix<T, Eigen::Dynamic, 1> operator=(const Vector<T>)
    {
        Eigen::Matrix<T, Eigen::Dynamic, 1> res(this->size());
        for(size_t i=0; i<this->size(); i++)
            res(i) = this->at(i);
        return res;
    };

    Vector<T>& operator*(double scale)
    {
        for(int i=0; i<this->size(); i++)
            this->at(i)*= scale;
    };

    Vector<T>& operator*=(double scale)
    {
        for(int i=0; i<this->size(); i++)
            this->at(i)*= scale;
    };

    T dot( const Vector<T> &rhs )
    {
        //dot product
        int n = rhs.size();
        
        if( this->size() != n )
            std::runtime_error("Dimension mismatch in vector-vector dot product");

        T result=(T)0;

        for(int i=0; i<n; i++)
            result += (*this)(i)*rhs(i);

        return result;
    };

};

template <typename T>
class Matrix : public std::vector< Vector< T > >
{
protected:
    int m_nrow, m_ncol;
public:
    Matrix(){};
    Matrix(int nrow, int ncol)
    {
        Resize(nrow,ncol);
        m_nrow=nrow;
        m_ncol=ncol;
    };

    Matrix(int nrow, int ncol, const T& init)
    {
        this->resize( Vector<T>(ncol, init) );
        m_nrow = nrow;
        m_ncol = ncol;
    };

    Matrix(int nrow, const Vector<T>& init)
    {
        this->resize( nrow, init );
        m_nrow = nrow;
        m_ncol = init.size();
    };

    Matrix(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &other)
    {
        Set(other);
    };

    int rows()
    {
        return m_nrow;
    };

    int cols()
    {
        return m_ncol;
    };

    void Resize(int nrow, int ncol)
    {
        this->resize(nrow, Vector<T>(ncol));
        m_nrow=nrow;
        m_ncol=ncol;
    };

    static Matrix<T> Zeros(int nrow, int ncol)
    {
        return Matrix<T>(nrow, Vector<T>(ncol, (T)0));
    };

    static Matrix<T> Ones(int nrow, int ncol)
    {
        return Matrix<T>(nrow, Vector<T>(ncol, (T)1));
    };

    T& operator() (int row, int col)
    {
        if( row > m_nrow-1 || row < 0 || col > m_ncol-1 || col < 0 )
            std::runtime_error("Index out of bounds in class Matrix()");

        return this->at(row).at(col);
    };

    Vector<T>& operator()(int row)
    {
        return this->at(row);
    };

    Matrix<T>& transpose()
    {
        Matrix<T> result(this->m_ncol, this->m_nrow);

        for(int i=0; i<this->m_nrow; i++)
            for(int j=0; j<this->m_ncol; j++)
                result(j,i) = (*this)(i,j);

        return result;
    };

    Matrix<T> &transposeInPlace()
    {
        Matrix<T> current = *this;

        Resize(this->m_ncol, this->m_nrow);

        *this = current.transpose();
    };

    Matrix<T>& Set(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &other)
    {
        Matrix<T> res(other.rows(), Vector<T>(other.cols()));
        for(size_t i=0; i<other.rows(); i++)
            for(size_t j=0; j<other.cols(); j++)
                (*this)(i,j) = other(i,j);
        return *this;
    };

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> AsEigenMatrixType()
    {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> res(m_nrow, m_ncol);
        for(size_t i=0; i<m_nrow; i++)
            for(size_t j=0; j<m_ncol; j++)
                res(i,j) = this->at(i).at(j);
        
        return res;
    };

    Matrix<T>& operator*(double scale)
    {
        for(int i=0; i<this->m_nrow; i++)
            this->at(i) *= scale;
    };

    Matrix<T> dot( const Vector<T> &rhs )
    {
        //dot product
        int n = rhs.size();
        
        if( this->cols() != n )
            std::runtime_error("Dimension mismatch in matrix-vector dot product");

        Vector<T> result(this->rows(),0.);
        for(int i=0; i<this->rows(); i++)
            for(int j=0; j<n; j++)
                result(i) += (*this)(i,j)*rhs(j);

        return result;
    };

    Matrix<T> dot( Matrix<T> &rhs)
    {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> A = this->AsEigenMatrixType();
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> B = rhs.AsEigenMatrixType();

        return Matrix<T>( A*B );
    };

};

static bool increment(Vector<int> &limits,  Vector<int> &indices )
{
    /* 
    Take a vector of ints (indices) that correspond to a set of indices in 'n' dimensions, where n=|indices|.
    Increment indices by 1, starting with the last index, and rolling over prior indices.

    Returns true if the indices are less than the greatest cumulative index allowed in 'limits'. Returns 
    false if the incremented index is now greater than the limit.

    Example:
    limits=[2,3,2]
    call    indices     returns
    0       [0,0,0]     
    1       [0,0,1]     true
    3       [0,1,0]     true
    4       [0,1,1]     true
    3       [0,2,0]     true
    3       [0,2,1]     true
    7       [1,0,0]     true
    8       [1,0,1]     true
    ...
    11      [1,2,1]     true
    12      [2,0,0]     false
    */
    int nd = indices.size();

    indices.back() ++;
    
    for(int i=nd-1; i>-1; i--)
    {
        if( indices.at(i) == limits.at(i) )
        {
            if(i==0)
                return false;
            indices.at(i-1)++;
            indices.at(i) = 0;
        }
    }
    return true;
};

template <typename T>
static void combinations( const Vector<T> &indices, int nd, Matrix<T> &result, int force_col_width=-1 )
{
    std::vector< int > ctr(nd);

    //initialize
    for(int i=0; i<nd; i++)
        ctr.at(i)=i;
    
    int ni = indices.size();

    // result.resize(ni*ni, force_col_width > nd ? force_col_width : nd);

    int ct=0;
    while(true)
    {
        for( int i=nd-1; i>0; i-- )
        {
            if(ctr.at(i) > (ni-1)-(nd-1-i))
            {
                ctr.at(i-1) ++;
                for(int j=i; j<nd; j++)
                    ctr.at(j) = ctr.at(j-1)+1;
            }
        }

        if( ctr.front() > ni - nd)
            break;
        
        result.push_back( Vector<T>(force_col_width > nd ? force_col_width : nd) );
        for( int i=0; i<nd; i++ )
        {
            // std::cout << ctr.at(i) << "\t";
            result(ct,i) = indices( ctr.at(i) );
        }
        // std::cout << "\n";

        ctr.back()++;
        ct++;
    }
    // result.conservativeResize(ct, force_col_width > nd ? force_col_width : nd);

    return;
};

template <typename T>
static int argmin( Vector<T> & v, bool ignore_nan=false)
{
    /* 
    return index of the minimum value in vector 'v'. If ignore_nan, then don't consider nan values.
    */
    int i_fmin=-1;
    double fmin=9e39;
    for( int i=0; i<v.size(); i++ )
    {
        if( ignore_nan )
            if( v(i) != v(i) )
                continue;
        if( (double)v(i) < fmin )
        {
            fmin = (double)v(i);
            i_fmin = i;
        }
    }
    return i_fmin;
};

template <typename T>
static float nanmin( Vector<T> & v)
{
    double fmin=9e39;
    for( int i=0; i<v.size(); i++ )
    {
        if( v(i) != v(i) )
            continue;
        if( (double)v(i) < fmin )
        {
            fmin = (double)v(i);
        }
    }
    return (T)fmin;
};

static Vector<double> nanfilter(const Vector<double> &v, Vector<int> *not_nan_indices=0)
{
    /* 
    Return a vector omitting all nan's

    Also fill a vector "not_nan_indices" containing the indices of 'v' with not nan
    */

    Vector<double> res; //(v.size());

    if( not_nan_indices )
        not_nan_indices->clear();
        // not_nan_indices->resize(v.size());  //oversize for now

    
    int ind=0;
    for(int i=0; i<v.size(); i++)
    {
        if( v(i) != v(i) )
            continue;
        else
        {
            if( not_nan_indices )
                // (*not_nan_indices)(ind) = i;
                not_nan_indices->push_back(i);

            // res(ind++) = v(i);
            res.push_back(v(i));
        }
    }
    // if( not_nan_indices )
    //     not_nan_indices->conservativeResize(ind); //resize to fit contents

    return res;
};

static Vector<int> range(int lb, int ub)
{
    if( ub < lb )
        std::runtime_error("Lower bound of range() exceeds upper bound.");

    Vector<int> res(ub-lb);

    for(int i=0; i<ub-lb; i++)
        res(i) = lb+i;

    return res;
}

template <typename T>
static Matrix<T> zip(const Vector<T> &A, const Vector<T> &B)
{
    if( A.size() != B.size() )
        std::runtime_error("Vector length mismatch in zip()");

    int n = A.size();

    Matrix<T> result(n, 2);
    for(int i=0; i<n; i++)
    {
        result(i,0) = A(i);
        result(i,1) = B(i);
    }
    return result;
}

template <typename T>
static void assign_where(Vector<T> &dest, const Vector<T> &compare, bool (*ftest)(const T &citem, void* data) )
{
    /* 
    Take 2 vectors of type Vector<T> or Vector<P> where both vectors have the same length 'm'. Assign values from 
    'compare' to corresponding positions in 'dest' when the user-defined function 'ftest(int,void*)' is true. 
    */

    int m=dest.size();
    if( compare.size() != m )
        std::runtime_error("Attempting to compare to vectors of unequal length.");
    
    for(int i=0; i<m; i++)
    {
        if( ftest(compare(i),0) )
            dest(i) = compare(i);
    }
};

template <typename T>
static Vector<int> filter_where(Vector<T> &source, T compare, bool (*ftest)(T item, T cval))
{
    /*
    like numpy::where()

    Take the vector 'compare' and run the function 'ftest' element-wise. When the function returns true,
    include the corresponding value index from 'compare' in the result vector.
    */
    Vector<int> res;
    for(int i=0; i<source.size(); i++)
        if( ftest(source(i), compare) )
            res.push_back(i);

    return res;
};

#endif