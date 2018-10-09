#ifndef _OPTUTIL_

#include <eigen/Core>
#include <eigen/Dense>
#include <exception>
#include <vector>
#include <set>
#include <ctime>
#include <chrono>

//-------------------- static methods ---------------------
template<typename T> class Vector;
template<typename T> class Matrix;

template <typename T>
static Vector<T> Zeros(int len)
{
    return Vector<T>(len, (T)0);
};

template <typename T>
static Vector<T> Ones(int len)
{
    return Vector<T>(len, (T)1);
};

template <typename T>
static Matrix<T> Zeros(int nrow, int ncol)
{
    /* 
    Static function: returns a matrix of (T)0 values of size 'nrow' x 'ncol'
    */
    return Matrix<T>(nrow, Vector<T>(ncol, (T)0));
};

template <typename T>
static Matrix<T> Ones(int nrow, int ncol)
{
    /* 
    Static function: returns a matrix of (T)1 values of size 'nrow' x 'ncol'
    */
    return Matrix<T>(nrow, Vector<T>(ncol, (T)1));
};
//-------------------------------------------------------

//------------------------------------------------------------------------
template <typename T>
class Vector : public std::vector< T >
{
public:
    Vector() {};
    Vector(int len)
    {
        this->std::vector<T>::resize(len);
    };

    Vector(int len, const T& init)
    {
        this->std::vector<T>::resize(len, init);
    };
    
    T maxCoeff()
    {
        /* 
        Returns the maximum value in the vector
        */
        return *std::max_element(this->begin(), this->end());
    };

    T minCoeff()
    {
        /*
        Returns the minimum value in the vector
        */
        return *std::min_element(this->begin(), this->end());
    };

    int argMin()
    {
        return (int)( std::min_element(this->begin(), this->end()) - this->begin() );
    };

    int argMax()
    {
        return (int)( std::max_element(this->begin(), this->end()) - this->begin() );
    };

    Vector<T> Subset(Vector<int> &indices)
    {
        Vector<T> result;
        for(size_t i=0; i<indices.size(); i++)
        {
            int ind_i = indices(i);
            assert( ind_i > -1 && ind_i < this->size() );

            result.push_back(operator()(ind_i));
        }
        return result;
    };

    Vector<T>& Set(const Eigen::Matrix<T, Eigen::Dynamic, 1> &other)
    {
        for(int i=0; i< (int)other.rows(); i++)
            std::vector<T>::push_back( other(i) );
        return *this;
    };

    T& operator()(int index){return this->at(index);};

    T operator()(int index) const {return this->at(index);};
    
    Vector<T>& operator=(const Eigen::Matrix<T, Eigen::Dynamic, 1> &other)
    {

        for(size_t i=0; i<other.size(); i++)
            this->at(i) = other(i);
        return &(*this);
    };

    Vector<T>& operator=(const Vector<T>& other)
    {
        //Eigen::Matrix<T, Eigen::Dynamic, 1> res(this->size());
        clear();
        for(size_t i=0; i<other.size(); i++)
            std::vector<T>::push_back( other(i) );

        return *this;
    };

    //Vector<T>& operator=(Vector<T> &other)
    //{
    //    clear();
    //    for (size_t i = 0; i < other.size(); i++)
    //        push_back(other(i));
    //};

    Vector<T>& operator*(double scale)
    {
        for(int i=0; i<(int)this->size(); i++)
            this->at(i)*= scale;
        return *this;
    };

    Vector<T>& operator*=(double scale)
    {
        for(int i=0; i<this->size(); i++)
            this->at(i)*= scale;
        return *this;
    };

    Vector<T>& operator+(Vector<T> &rhs)
    {
        if( this->size() != rhs.size() )
            std::runtime_error("Vector addition size mismatch");
        for(int i=0; i<rhs.size(); i++)
            this->operator()(i) += rhs(i);
        return *this;
    };

    Vector<T>& operator-(Vector<T> &rhs)
    {
        if( this->size() != rhs.size() )
            std::runtime_error("Vector subtraction size mismatch");
        for(int i=0; i<(int)rhs.size(); i++)
            this->operator()(i) -= rhs(i);
        return *this;
    };

    void clear()
    {
        this->std::vector<T>::clear();
    };

    T dot( const Vector<T> &rhs )
    {
        //dot product
        int n = (int)rhs.size();
        
        if( (int)this->size() != n )
            std::runtime_error("Dimension mismatch in vector-vector dot product");

        T result=(T)0;

        for(int i=0; i<n; i++)
            result += this->operator()(i)*rhs(i);

        return result;
    };

    Eigen::Matrix<T, Eigen::Dynamic, 1> AsEigenVectorType()
    {
        Eigen::Matrix<T, Eigen::Dynamic, 1> res(this->size(), 1);

        for(size_t i=0; i<this->size(); i++)
            res(i,0) = this->at(i);
        
        return res;
    };


};   //-------------------------- end vector -

//------------------------------------------------------------------------
template <typename T>
class Matrix : public std::vector< Vector< T > >
{
protected:

public:
    Matrix() {};
    Matrix(int nrow, int ncol)
    {
        std::vector< Vector<T> >::resize(nrow, Vector<T>(ncol));
    };

    Matrix(int nrow, int ncol, const T& init)
    {
        std::vector< Vector<T> >::resize(nrow, Vector<T>(ncol, init));
    };

    Matrix(int nrow, const Vector<T>& init)
    {
        std::vector< Vector<T> >::resize(nrow, init);
    };

    Matrix(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &other)
    {
        Set(other);
    };

    int rows()
    {
        return (int)this->size();
    };

    int cols()
    {
        return (int)this->front().size();
    };

    Vector<T>& push_back( Vector<T> row )
    {
        std::vector< Vector<T> >::push_back( row );
        return this->back();
    };

    //void erase(int where, bool is_col=false)
    //{
    //    if( is_col )
    //    {
    //        assert(where < cols() && where > -1);
    //        transposeInPlace();
    //        std::vector< Vector<T> >::erase(this->begin()+where, this->begin()+where+1);
    //        transposeInPlace();
    //    }
    //    else
    //    {
    //        assert(where < this->size() && where > -1);
    //        
    //        std::vector< Vector<T> >::erase(this->begin()+where, this->begin()+where+1);
    //    }
    //    
    //    return;
    //}

    Matrix<T> Subset(Vector<int> &indices)
    {
        Matrix<T> result;
        for(size_t i=0; i<indices.size(); i++)
        {
            int ind_i = indices(i);
            assert( ind_i > -1 && ind_i < rows() );

            result.push_back(this->row(ind_i));
        }
        return result;
    };

    T& operator() (int row, int col)
    {
        /* 
        Returns a reference to the value stored at 'row','col'
        */
        if( row > rows()-1 || row < 0 || col > cols()-1 || col < 0 )
            std::runtime_error("Index out of bounds in class Matrix()");

        return this->operator[](row).operator[](col);
    };

    Vector<T>& operator()(int row)
    {
        /* 
        Returns a reference to the row vector at 'row'
        */
        return this->at(row);
    };

    void clear()
    {
        this->std::vector< Vector<T> >::clear();
    };

    void resize(int nrow, int ncol)
    {
        std::vector< Vector<T> >::resize(nrow, ncol);
    };

    void resize(int nrow, int ncol, T& init)
    {
        std::vector< Vector<T> >::resize(nrow, Vector<T>(ncol, init));
    };

    void resize(int nrow, Vector<T>& init)
    {
        std::vector< Vector<T> >::resize(nrow, init);
    };

    void resize(int nrow, Vector<T> init)
    {
        std::vector< Vector<T> >::resize(nrow, init);
    };

    Vector<T> row(int index)
    {
        /* 
        Returns a copy of the row values at 'index'
        */
        assert(index > -1 && index < rows());

        return this->at(index);
    };

    Vector<T> col(int index)
    {
        /*
        Returns a copy of the column values at 'index'
        */
        assert(index > -1 && index < cols() );

        Vector<T> newcol;
        for(int i=0; i<rows(); i++)
            newcol.push_back( this->operator()(i,index) );
        return newcol;
    };

    void set_col(int index, const Vector<T>& col_values)
    {
        /* 
        Assign values from 'col_values' to a specified column in at 'index'
        */
        assert(index > -1 && index < cols() && col_values.size() == rows());

        for(int i=0; i<rows(); i++)
            operator()(i, index) = col_values(i);
    };

    Matrix<T>& operator=(Matrix<T> rhs)
    {
        this->std::vector< Vector<T> >::resize(rhs.rows(), rhs.cols());
        for(int i=0; i<rhs.rows(); i++)
            for(int j=0; j<rhs.cols(); j++)
                this->operator()(i,j) = rhs(i,j);       //deep copy

        return *this;
    };

    Matrix<T> transpose()
    {
        Matrix<T> result(cols(), rows());

        for(int i=0; i<rows(); i++)
            for(int j=0; j<cols(); j++)
                result(j,i) = operator()(i,j);

        return result;
    };

    Matrix<T> &transposeInPlace()
    {
        Matrix<T> temp = (*this).transpose();

        clear();
        resize(temp.rows(), temp.cols());
        for(int i=0; i<temp.rows(); i++)
            for(int j=0; j<temp.cols(); j++)
                this->operator()(i,j) = temp(i,j);

        return *this;
    };

    Matrix<T>& Set(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &other)
    {
        for(int i=0; i< (int)other.rows(); i++)
        {
            Vector<T> rowi;            
            for(int j=0; j< (int)other.cols(); j++)
                rowi.push_back( other(i, j) );
            this->push_back(rowi);
        }
        return *this;
    };

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> AsEigenMatrixType()
    {
        Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> res(rows(), cols());
        for(int i=0; i<rows(); i++)
            for(int j=0; j<cols(); j++)
                res(i,j) = this->at(i).at(j);
        
        return res;
    };

    Matrix<T>& operator*(double scale)
    {
        for(int i=0; i<rows(); i++)
            this->at(i) *= scale;
    };

    template <typename U>
    Vector<T> dot( Vector<U> rhs )
    {
        //dot product
        int n = rhs.size();
        
        if( this->cols() != n )
            std::runtime_error("Dimension mismatch in matrix-vector dot product");

        Vector<T> result(this->rows(),0.);
        for(int i=0; i<this->rows(); i++)
            for(int j=0; j<n; j++)
                result(i) += this->operator()(i,j)*rhs(j);

        return result;
    };

    Matrix<T> dot( Matrix<T> rhs)
    {
        assert(cols() == rhs.rows());

        Matrix<T> result = Zeros<T>( rows(), rhs.cols() );

        for(int i=0; i<rows(); i++)
            for(int k=0; k<rhs.cols(); k++)
                for(int j=0; j<cols(); j++)
                    result(i,k) += this->operator()(i,j)*rhs(j,k);
        return result;
    };

};  //--------------------- end matrix

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
    int nd = (int)indices.size();

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
    
    int ni = (int)indices.size();

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
    for( int i=0; i<(int)v.size(); i++ )
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
static T nanmin( Vector<T> & v)
{
    T minval = std::numeric_limits<T>::max();
    for( int i=0; i<(int)v.size(); i++ )
    {
        if( v(i) != v(i) )
            continue;
        if( v(i) < minval)
            minval = v(i);
    }
    return minval;
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

    
    for(int i=0; i<(int)v.size(); i++)
    {
        if( v(i) != v(i) )
            continue;
        else
        {
            if( not_nan_indices )
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

    int m= (int)dest.size();
    if( (int)compare.size() != m )
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
    Take the vector 'compare' and run the function 'ftest' element-wise. When the function returns true,
    include the corresponding value index from 'compare' in the result vector.
    */
    Vector<int> res;
    for(int i=0; i<(int)source.size(); i++)
        if( ftest(source(i), compare) )
            res.push_back(i);

    return res;
};

template <typename T>
static Vector<T> where(Vector<T> &A, Vector<T> &B, bool (*ftest)(T& aval, T& bval))
{
    /* 
    like numpy::where()

    Compare elements of A and B. When true, fill result with element in A. When False, fill with B.
    */

    if( A.size() != B.size() )
        std::runtime_error("Vector size mismatch in where()");

    Vector<int> res;
    for(int i=0; i<A.size(); i++)
        res.push_back( ftest(A(i), B(i)) ? A(i) : B(i) );

    return res;    
};


#endif