#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <iomanip>

// Library is very simple, no contiguous arrays (transpose always copies and
// is expensive), no sparse matrices (sparse data will be inefficient)
// make a new header and convert these functions to methods if you'd
// like to work with these, I don't anticipate they'll be necessary
// for a WASM/webgl graphics programming concept
//
// Additional note: we probably could have just made everything
// work with mat4 in js (and indeed GSLS/webgl only need you to do operations on and supply mat4)
// but eh since I was writing the library in js anyways its not that hard to convert over to C++. An advantage is that you can do some more complicated things with larger matrices in C++ without a performance hit, if you do something that ends up being arduous via GSLS/webgl's interfaces

typedef std::vector<std::vector<float>> mat;

mat allocate_array(int n, int m, bool fill_zero = false) {
    mat matrix(n);
    for (int i = 0; i < n; i++) {
        if (fill_zero) {
            std::vector<float> new_row(m, 0.0);
            matrix[i] = new_row;
        } else {
            std::vector<float> new_row(m);
            matrix[i] = new_row;
        }
    }
    return matrix;
}

mat convert_contiguous(std::vector<float> const &  arr, std::array<int, 2> shape) {
    if (arr.size() != shape[0] * shape[1]) {
        std::cerr << "Warning: array of incorrect shape passed" << std::endl;
        mat null = allocate_array(0, 0);
        return null;
    }
    mat matrix = allocate_array(shape[0], shape[1]);
    for (int i = 0; i < shape[0]; i++) {
        for (int j = 0; j < shape[1]; j++) {
            matrix[i][j] = arr[(j + (shape[1] * i))];
        }
    }
    return matrix;
}

//identity assumes square matrices
mat identity(int n, int m){
    if (n != m) {
        std::cerr << "Non-square matrix passed, function will return empty matrix" << std::endl;
        mat null = allocate_array(0, 0);
        return null;
    }
    // Preset size of matrix
    mat matrix  = allocate_array(n, m);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j){
                matrix[i][j] = 1;
            }
            else {
                matrix[i][j] = 0;
            }
        }
    }
    return matrix;
}


// To transpose a matrix, we iterate through the columns and add them as
// rows to a nother matrix. This is the function most inefficient bc
// we didn't use contig arrays + 
mat transpose(mat const & matrix) {
    int n_row = matrix.size();
    int n_col = matrix[0].size();

    if (n_row != n_col){
        std::cerr << "Non-square matrix passed to identity function, function will return null." << '\n';;
        mat null = allocate_array(0, 0);
    }

    mat matrixT = allocate_array(n_col, n_row);
    for (int i = 0; i < n_row; i++) {
        for (int j = 0; j < n_col; j++) {
            matrixT[j][i] = matrix[i][j];
        }
    }
    return matrixT;
}

template<typename T>
mat scalarMultiply(mat const & mat1, T value) {
    mat ret = allocate_array(mat1.size(), mat1[0].size());
    for (int i = 0; i < mat1.size(); i++){
        for (int j = 0; j < mat1[0].size(); j++) {
            ret[i][j] = mat1[i][j] * value;
        }
    }
    return ret;
}

template<typename T>
mat scalarSubtract(mat const & mat1, T value) {
    mat ret = allocate_array(mat1.size(), mat1[0].size());
    for (int i = 0; i < mat1.size(); i++){
        for (int j = 0; j < mat1[0].size(); j++) {
            ret[i][j] = mat1[i][j] - value;
        }
    }
    return ret;
}

template<typename T>
mat scalarAdd(mat const & mat1, T value) {
    mat ret = allocate_array(mat1.size(), mat1[0].size());
    for (int i = 0; i < mat1.size(); i++){
        for (int j = 0; j < mat1[0].size(); j++) {
            ret[i][j] = mat1[i][j] + value;
        }
    }
    return ret;
}


mat multiply(mat const & mat1, mat const & mat2) {
    int mat1_rows = mat1.size();
    int mat1_cols = mat1[0].size();
    int mat2_rows = mat2.size();
    int mat2_cols = mat2[0].size();
    if (mat1_rows != mat2_cols) {
        std::cerr << "Rows of mat1 and cols of mat2 don't match" << std::endl;
    }
    mat out_mat = allocate_array(mat2_cols, mat1_rows, true);
    for (int col = 0; col < mat2_cols; col++) {
        for (int i = 0; i < mat1_rows; i++) {
            for (int j = 0; j < mat1_cols; j++) {
                out_mat[col][i] = out_mat[col][i] + mat1[i][j] + mat2[j][col];
            }
        }
    }
    return out_mat;
}

 std::array<mat, 2> calc_LU(mat const & a) {
     //  NOTE: IT would be really good to throw an error or warning here if L or U is
     //  NOT an traingular matrix (not zeroes above or below), this is expected
     //  if the inputs are not linear independent, you could even create a function
     //  that checks for whether rank doesn't match quickly doing this (or implement SVD at some point to calculate rank)
    // For use in solving linear systems and inverting matrices, calc_LU 
    // in separate function
    int a_rows = a.size();
    int a_cols = a[0].size();
    if ((a_rows < 3) || (a_rows != a_cols)) {
        std::cerr << "Attempted to solve for either non-square matrix or matrix with a dimension less than 3" << std::endl;
        mat empty = allocate_array(a_rows, a_cols);
        std::array<mat, 2> ret = {empty, empty};
        return ret; 
    }
    // Prob gonna be lots of times you don't wanna mess with input array
    // This should just deep copy? C++ I think calls copy constructor for vector elements
    mat U = a;
    mat L = identity(a_rows, a_cols);
    for (int j = 0; j < a_cols; j++) {
        U[0][j] = a[0][j];
    }
    for (int i = 1; i < a_rows; i++) {
        for (int next_row = i; next_row < a_rows; next_row++) {
            int multiplier = U[next_row][i-1] * std::signbit(a[i-1][i]);
            L[next_row][i-1] = multiplier;
            for (int j = i - 1; j < a_cols; j++){
                U[next_row][j] = U[next_row][j] - (U[i - 1][j] * multiplier);
            }
        }
    }
    std::array<mat, 2> ret = {L, U};
    return ret;
}
std::array<int, 2> abs_i_to_i(int i, std::array<int, 2> shape) {
    int row_i = floor(i / shape[0]);
    int col_i = i % shape[1];
    std::array<int, 2> ret = {row_i, col_i};
    return ret;
}

mat solve_LU(mat const & L, mat const & U, mat const & b) {
    // Given precalculated L and U, solve for Ax = b where L is the lower 
    // triangular of A and U is the upper triangular of A.
    // Alg simply goes column by column in b and solves individually
    if ((L.size() != b.size()) || (U.size() != b.size())) {
        std::cerr << "Supplied matrix of invalid shape to LU solver, returning null" << std::endl;
        mat empty = allocate_array(0, 0);
        return empty;
    }
    int b_rows = b[0].size();
    int b_cols = b.size();
    mat Z = allocate_array(b_rows, b_cols);
    for (int initial = 0; initial < b_cols; initial++) {
        float z = b[0][initial];
        Z[0][initial] = z;
    }
    for (int bcoli = 0; bcoli < b_cols; bcoli++) {
        for (int i = 1; i < b_rows; i++) {
            float zij = b[i][bcoli]; 
            for (int j = 0; j < i; j++) {
                zij -= b[i-1][bcoli] * L[i-1][j];
            }
            Z[i][bcoli] = zij;
        }
    }
    // Do the same thing but its for the upper and substituting Z for b
    mat X = allocate_array(b_rows, b_cols);
    for (int initial = 0; initial < b_cols; initial++) {
        float x_bottom = Z[b_rows - 1][initial] / U[b_rows - 1][b_cols - 1];
        X[b_rows - 1][initial] = x_bottom;
    }
    // We basically have to go through everything backwards
    for (int browi = b_rows - 2; browi > 0; browi--) {
        // Go from end column up to whatever row you're on
        for (int j = b_cols - 1; j >= browi; j--) {
            float xij = Z[browi][j]; 
            for (int i = j; i >= browi; i--) {
                xij -= X[i][j] * U[i-1][j];
            }
            X[browi][j] = xij;
        }
    }
    return X;
}

mat solve(mat const &  a, mat const & b) {
    // A "generic" interface that just calls the LU solver functions
    std::array<mat, 2> LU = calc_LU(a);
    mat x = solve_LU(LU[0], LU[1], b);
    return x;
}

// For inversion we solve(matrix, I)
// Computer upper and lower traingular matrices using
// Gaussian Elimination, 
// Then solve with LU factorization for the identity matrix
mat inverse(mat const & a) {
    int a_rows = a.size();
    int a_cols = a[0].size();
    mat ident = identity(a_rows, a_cols);
    std::array<mat, 2> LU = calc_LU(a);
    mat ret = solve_LU(LU[0], LU[1], ident);
    return ret;
}

void print_mat(mat const & mat, int precision = 2, int width = 5) 
{
    //idk we change defaults for the width that feel right
int column_index;
    std::cout << std::fixed << std::setprecision(precision); 
    std::cout << "[";
    for (int row_index = 0; row_index < mat.size(); row_index++) 
{
    if (row_index!= 0) {
        std::cout << " ";
    }
    std::cout << "[";
    std::cout << std::setw(width);
    for (int column_index = 0; column_index < mat[0].size(); column_index++) {
        auto output = mat[row_index][column_index];
        std::cout  << output; 
        if (row_index != mat.size() - 1 && column_index != mat[0].size() -1) {
            std::cout << ", ";
        }
        if (row_index > 1000) {
    std::cout << "...";
            std::cout << "]]";
            std::cout << std::endl;
            return void();
        }
    }
    std::cout << "]";
    if (row_index  < mat.size()) {
        std::cout << ",";
        std::cout << "\n";
    }
    }
std::cout << "]";
    std::cout << std::endl;
return void();
}

// This is purely for testing, remove the main later
int main() {
    mat diagonal = identity(4, 4);
    diagonal = scalarMultiply(diagonal, 3);
    mat g4g = convert_contiguous(std::vector<double> {1,1,1,4,3,-1,3,5,3}, std::array<int, 2> {3,3}))
    mat inv_diagonal = inverse(diagonal);
    // Expected output is 1/3 along the diagonal
    print_mat(inv_diagonal);
}
