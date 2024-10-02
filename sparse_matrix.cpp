#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

class SparseMatrix {
private:
    struct Element {
        int row, col, value;
        Element(int r, int c, int v) : row(r), col(c), value(v) {}
    };

    std::vector<Element> elements;
    int numRows, numCols;

public:
    SparseMatrix(const char* matrixFilePath) {
        std::ifstream file(matrixFilePath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file");
        }

        std::string line;
        
        // Read number of rows
        if (!std::getline(file, line)) {
            throw std::invalid_argument("Input file has wrong format");
        }
        if (sscanf(line.c_str(), "rows=%d", &numRows) != 1) {
            throw std::invalid_argument("Input file has wrong format");
        }

        // Read number of columns
        if (!std::getline(file, line)) {
            throw std::invalid_argument("Input file has wrong format");
        }
        if (sscanf(line.c_str(), "cols=%d", &numCols) != 1) {
            throw std::invalid_argument("Input file has wrong format");
        }

        // Read matrix elements
        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
            
            int row, col, value;
            if (sscanf(line.c_str(), "(%d,%d,%d)", &row, &col, &value) != 3) {
                throw std::invalid_argument("Input file has wrong format");
            }
            
            elements.emplace_back(row, col, value);
        }
    }

    SparseMatrix(int rows, int cols) : numRows(rows), numCols(cols) {}

    int getElement(int currRow, int currCol) const {
        for (const auto& elem : elements) {
            if (elem.row == currRow && elem.col == currCol) {
                return elem.value;
            }
        }
        return 0;
    }

    void setElement(int currRow, int currCol, int value) {
        for (auto& elem : elements) {
            if (elem.row == currRow && elem.col == currCol) {
                elem.value = value;
                return;
            }
        }
        elements.emplace_back(currRow, currCol, value);
    }

    SparseMatrix add(const SparseMatrix& other) const {
        if (numRows != other.numRows || numCols != other.numCols) {
            throw std::invalid_argument("Matrix dimensions do not match for addition");
        }

        SparseMatrix result(numRows, numCols);
        
        for (const auto& elem : elements) {
            result.setElement(elem.row, elem.col, elem.value);
        }

        for (const auto& elem : other.elements) {
            int newValue = result.getElement(elem.row, elem.col) + elem.value;
            result.setElement(elem.row, elem.col, newValue);
        }

        return result;
    }

    SparseMatrix subtract(const SparseMatrix& other) const {
        if (numRows != other.numRows || numCols != other.numCols) {
            throw std::invalid_argument("Matrix dimensions do not match for subtraction");
        }

        SparseMatrix result(numRows, numCols);
        
        for (const auto& elem : elements) {
            result.setElement(elem.row, elem.col, elem.value);
        }

        for (const auto& elem : other.elements) {
            int newValue = result.getElement(elem.row, elem.col) - elem.value;
            result.setElement(elem.row, elem.col, newValue);
        }

        return result;
    }

    SparseMatrix multiply(const SparseMatrix& other) const {
        if (numCols != other.numRows) {
            throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
        }

        SparseMatrix result(numRows, other.numCols);

        for (const auto& elem1 : elements) {
            for (const auto& elem2 : other.elements) {
                if (elem1.col == elem2.row) {
                    int newValue = result.getElement(elem1.row, elem2.col) + elem1.value * elem2.value;
                    result.setElement(elem1.row, elem2.col, newValue);
                }
            }
        }

        return result;
    }

    void print() const {
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numCols; ++j) {
                std::cout << getElement(i, j) << " ";
            }
            std::cout << std::endl;
        }
    }

    int getNumRows() const { return numRows; }
    int getNumCols() const { return numCols; }
};

int main() {
    try {
        std::cout << "Enter the operation (add/subtract/multiply): ";
        std::string operation;
        std::cin >> operation;

        std::cout << "Enter the path for the first matrix file: ";
        std::string file1Path;
        std::cin >> file1Path;

        std::cout << "Enter the path for the second matrix file: ";
        std::string file2Path;
        std::cin >> file2Path;

        SparseMatrix matrix1(file1Path.c_str());
        SparseMatrix matrix2(file2Path.c_str());

        SparseMatrix result(1, 1);  // Placeholder initialization

        if (operation == "add") {
            result = matrix1.add(matrix2);
        } else if (operation == "subtract") {
            result = matrix1.subtract(matrix2);
        } else if (operation == "multiply") {
            result = matrix1.multiply(matrix2);
        } else {
            throw std::invalid_argument("Invalid operation");
        }

        std::cout << "Result:" << std::endl;
        result.print();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
