/**
 * @file
 * @brief Contains the TPZFYsmpMatrix class which implements a non symmetric sparse matrix.
 */
/******************************************************************************
 *
 * Class definition:    TPZFYsmpMatrix
 *
 * Class type:          Derived from TPZMatrix
 *
 * Purpose:             Define operations on general sparse matrices stored
 *                      in the (old) Yale Sparse Matrix Package format.
 *
 * Solvers:             SOR
 *
 *****************************************************************************/

#ifndef YSMPMATH
#define YSMPMATH
#ifdef USING_BLAS
extern "C"{
     #include "cblas.h"
     };
#endif

#include "pzmatrix.h"
#include "tpzverysparsematrix.h" 

template<class TVar>
class TPZFMatrix;

/**
 * @brief Implements a non symmetric sparse matrix (Yale Sparse Matrix Storage). \ref matrix "Matrix"
 * @ingroup matrix
 */
/**
 * Defines operations on general sparse matrices stored in the (old) Yale Sparse Matrix Package format.
 */
template<class TVar>
class TPZFYsmpMatrix : public TPZMatrix<TVar> {
	
	public :
	
	/** @brief An auxiliary structure to hold the data of the subset \n of equations used to multiply in a multitrheaded environment */
	/**
	 In future versions this structure should be defined in a derived class
	 */
	struct TPZMThread {
		const TPZFYsmpMatrix<TVar> *target;
		int fFirsteq;
		int fLasteq;
		const TPZFMatrix<TVar> *fX;
		TPZFMatrix<TVar> *fZ;
		TVar fAlpha;
		int fOpt;
		int fStride;
	};
	
private:
	
	static void * ExecuteMT(void *entrydata);
	
public: 
    TPZFYsmpMatrix(const int rows,const int cols );
	
    //TPZFYsmpMatrix(const TPZFYsmpMatrix &cp) : TPZMatrix(cp), fSymmetric(cp.fSymmetric)
	//    {
	//      int fjasize = fIA[Rows()];
	//      fIA = new int[Rows()+1];
	//      fDiag = new REAL[Rows()];
	//      fJA = new int[fjasize];
	//			fA = new REAL[fjasize];
	//			memcpy(fIA,cp.fIA,(Rows()+1)*sizeof(int));
	//			memcpy(fJA,cp.fJA,fjasize*sizeof(int));
	//			memcpy(fDiag,cp.fDiag,Rows()*sizeof(REAL));
	//			memcpy(fA,cp.fA,fjasize*sizeof(REAL));
	//		}
	
	// Replace the above destructor
	
	TPZFYsmpMatrix(const TPZVerySparseMatrix<TVar> &cp);
	
	TPZFYsmpMatrix &operator=(const TPZFYsmpMatrix<TVar> &copy);
	
	TPZFYsmpMatrix &operator=(const TPZVerySparseMatrix<TVar> &cp);
    
	CLONEDEF(TPZFYsmpMatrix)
	
	virtual ~TPZFYsmpMatrix();
	
	
	/** @brief Get the matrix entry at (row,col) without bound checking */
	virtual const TVar &GetVal(const int row,const int col ) const;
	
	int NumTerms() 
	{
		return fIA[this->Rows()];
	}
	
	int PutVal(const int row, const int col, const TVar &Value);
	
	virtual void MultAdd(const TPZFMatrix<TVar> &x,const TPZFMatrix<TVar> &y, TPZFMatrix<TVar> &z,
						 const TVar alpha=1.,const TVar beta = 0.,const int opt = 0,const int stride = 1 ) const;
	
	virtual void MultAddMT(const TPZFMatrix<TVar> &x,const TPZFMatrix<TVar> &y, TPZFMatrix<TVar> &z,
						   const TVar alpha=1.,const TVar beta = 0.,const int opt = 0,const int stride = 1 );
	
	virtual int GetSub(const int sRow,const int sCol,const int rowSize,
					   const int colSize, TPZFMatrix<TVar> & A ) const;
	
	void GetSub(const TPZVec<int> &indices,TPZFMatrix<TVar> &block) const;
	
	/** @brief Pass the data to the class. */
	virtual void SetData( int *IA, int *JA, TVar *A );
	
	/** @brief Print the matrix along with a identification title */
	virtual void Print(const char *title, std::ostream &out = std::cout , const MatrixOutputFormat form = EFormatted) const;
	
	/**
	 * @name Solvers
	 * @brief Linear system solvers. \n
	 */
	 /** For symmetric decompositions lower triangular matrix is used. \n
	 * Solves a system A*X = B returning X in B
	 */  
	//@{
	/**
	 * @brief Solves the linear system using Jacobi method. \n
	 * @param numiterations The number of interations for the process.
	 * @param F The right hand side of the system.
	 * @param result The solution.
	 * @param residual Returns F - A*U which is the solution residual.
	 * @param scratch Available manipulation area on memory.
	 * @param tol The tolerance value.
	 * @param FromCurrent It starts the solution based on FromCurrent. Obtaining solution FromCurrent + 1.
	 */
	virtual void SolveJacobi(int & numiterations, const TPZFMatrix<TVar> & F, TPZFMatrix<TVar> & result,
							 TPZFMatrix<TVar> * residual, TPZFMatrix<TVar> & scratch, TVar & tol, const int FromCurrent = 0) ;
	
	void SolveSOR(int &numiterations, const TPZFMatrix<TVar> &rhs, TPZFMatrix<TVar> &x,
				  TPZFMatrix<TVar> *residual, TPZFMatrix<TVar> &scratch,
				  const TVar overrelax, TVar &tol,
				  const int FromCurrent = 0,const int direction = 1 ) ;    
	// @}
	
	/**
	 * @brief Add a contribution of a stiffness matrix
	 * putting it on destination indexes position
	 */
	virtual void AddKelOld(
						   TPZFMatrix<TVar> & elmat //! Member stiffness matrix beeing added
						   , TPZVec < int > & destinationindex //! Positioning of such members on global stiffness matrix
						   );    
	
	void AddKel(TPZFMatrix<TVar> & elmat, TPZVec<int> & destinationindex);    
	
	void AddKel(TPZFMatrix<TVar> & elmat, TPZVec<int> & sourceindex, TPZVec<int> & destinationindex);    
	
	void MultiplyDummy(TPZFYsmpMatrix<TVar> & B, TPZFYsmpMatrix<TVar> & Res);
	
	virtual int Zero();
	
	/**
	 * @name Factorization
	 * @brief Those member functions are related to matrices factorization
	 */
	//@{
	/**
	 * @brief Decomposes the current matrix using LU decomposition.
	 */
	virtual int Decompose_LU(std::list<int> &singular);
	virtual int Decompose_LU();
	
	//@}
	
	/**
	 * @name Substitutions
	 * @brief Substitutions forward and backward
	 */
	//@{  
	/**
	 * @brief Computes Forward and Backward substitution for a "LU" decomposed matrix.
	 * @param B right hand side and result after all
	 */
	virtual int Substitution( TPZFMatrix<TVar> * B ) const;
	
	//@}
	
	
private:
	
	void ComputeDiagonal();
	
	/*
	 * @brief Perform row update of the sparse matrix
	 */
	void RowLUUpdate(int sourcerow, int destrow);
	
protected:
	int  *fIA;
	int  *fJA;
	TVar *fA;
	
	TVar *fDiag;
	
	int   fSymmetric;
	
protected:
	
	/**
	 * @brief Implements a initialization method for the sparse structure. It sets the initial value for the fIA and fJA.
	 */ 
	/**
	 * -fIA will contain the initial positions for all the equations
	 * -fJA will contain (-1) on all its positions
	 * -fA will contain 0 on all its value 
	 */
	void InitializeData();
};


template<class TVar>
inline void TPZFYsmpMatrix<TVar>::SetData( int *IA, int *JA, TVar *A ) {
	// Pass the data to the class.
	fIA = IA;
	fJA = JA;
	fA  =  A;
	ComputeDiagonal();
}

#endif
