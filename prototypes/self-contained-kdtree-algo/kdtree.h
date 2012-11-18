#ifndef KDTREE_H
#define KDTREE_H
#include <vector>
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/tuple/tuple.hpp>

//#define DEBUG_KDTREE 1

namespace KDTREE{

	typedef double coordinate;

	// the collection of points
	typedef boost::numeric::ublas::matrix<coordinate> Points; 
	// output the points
	std::ostream& operator << (std::ostream& o, const Points & ps);

	// a point
	typedef boost::numeric::ublas::vector<coordinate> Point; 

	// a column, a row
	typedef boost::numeric::ublas::matrix_column<Points> Column;
	typedef boost::numeric::ublas::matrix_row<Points> Row;

	// index slice
	typedef std::vector<size_t> Ind;
	// output the ind
	std::ostream& operator << (std::ostream& o, const Ind & ind);

	// let know that there is a node
	struct Node{
	
		Node * _right;      // all the points in this node are < than the median contained here
		Node * _left;       // all the points in this node are < than the median contained here  
		coordinate _median; // median value stored in this node
		unsigned int _idx;  // index vector corresponding to the median 

		// create a new node
		Node(coordinate median, unsigned int idx) : 
			_median(median), _idx(idx), 
			_right(NULL), _left(NULL) {};
	};

	// tuple return value
	typedef boost::tuple<const Node *, double, unsigned int> NNreturn;


	
	// KDtree
	class KDtree{

	public:

		// random init the space
		KDtree(unsigned int numPoints, unsigned int dim)
			: _numPoints(numPoints), _dim(dim)
		{	
			_ps.resize(numPoints, dim, false); 
			for (unsigned int i = 0; i < numPoints; i++)
				for (unsigned int j = 0; j < dim; j++)	
					_ps(i, j) = (-1.0 + rand() * (2.0) / RAND_MAX);
		};

		// number of points in this space
		inline unsigned int numPoints() const { return _numPoints; };

		// dimension of this space
		inline unsigned int dim() const { return _dim; };

		// access the inner space
		inline Points & data() { return _ps; };

		// access the root
		inline Node * getRoot() const { return _root; };

		// build the kd-tree
		void build(void);

		// visit the tree
		void depthFirstVisit(Node *) const;

		//
		// return nearest
		//
		NNreturn returnNearest(const Point & p) const { return _returnNearest(_root, p, 0); };


	private:
		Points _ps;               // pointer space 
		unsigned int _numPoints;  // num points in this space
		unsigned int _dim;        // dim of the space  

		Node * _root;              // kdtree root    

		// aux build kd-tree
		Node * _build(Ind ind, unsigned int depth=0);

		// classical find median
		// some additional information is used
		//
		//		a permutation of ind is computed
		//
		void _find_median(Ind & ind, unsigned int axis, 
							unsigned int left, unsigned int right, unsigned int k);

		// classical partition ~ quicksort
		// some additional information is used
		//		ind is used to mantain the swapped indices
		//		axis is current selected axis
		//
		unsigned int _partition(Ind & ind, unsigned int axis, 
								  unsigned int left, unsigned int right);

		//
		// aux method to dump Ind
		//
		void __dump_data_with_ind(Ind & ind, unsigned int axis);

		//
		// return nearest
		//
		NNreturn KDtree::_returnNearest(const Node * n, const Point & p, unsigned int depth) const;

		// 
		// compute distance
		//
		double _distance(unsigned int i, const Point & p) const ;


	};
};

#endif