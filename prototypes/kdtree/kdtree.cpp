#include "kdtree.h"
#include <iostream>

namespace KDTREE{

	std::ostream& operator << (std::ostream& o, const Points & ps)
	{
		unsigned int numPoints = ps.size1();
		unsigned int dim = ps.size2();

		for (unsigned int i = 0; i < numPoints; i++){
			o << "index=" << i << " "; 
		  for (unsigned int j = 0; j < dim; j++)	
				o << ps(i, j) << " ";
		  o << "\n";
		}
		return o;
	};

	// output the ind
	std::ostream& operator << (std::ostream& o, const Ind & ind)
	{
		o << "\t\t\t ind=[";
		Ind::const_iterator it = ind.begin();
		Ind::const_iterator itend= ind.end();
		for ( ; it != itend; ++it)
		{
			o << *it << ","; 
		}
		o << "]\n";
		return o;
	}


	// 
	// build the kd-tree
	// start with all the indexes
	//
	void KDtree::build(void)
	{
		Ind ind;
		for (size_t i = 0; i<numPoints(); i++)   
			ind.push_back(i);

		_root = _build(ind);
	}

	// recursively build the kd-tree
	//
	Node * KDtree::_build(Ind ind, unsigned int depth)
	{
#ifdef DEBUG_KDTREE
		std::cout << "depth = " << depth << " ind size = " << ind.size() << "\n";
#endif

		unsigned int i, axis, medianPos;       
		axis = depth % dim();   // select the axis

#ifdef DEBUG_KDTREE
		std::cout << "Selected axis = " << axis << "\n";
		for (i = 0; i < ind.size(); i++) // consider all the assigned points
			std::cout<< "\tindex=" << ind[i] << " " << _ps(ind[i], axis) << "\n";
#endif

		Node * node = new Node(_ps(ind[0], axis), ind[0]); // this local node
		      
		if (ind.size() == 1)                       // no more points to partition
		{
#ifdef DEBUG_KDTREE
			std::cout << "\tLeaf\n";
#endif
			return node;
		}
	
		// find the median	
		medianPos = ind.size() / 2;
		_find_median(ind, axis, 0, ind.size()-1, medianPos);  // fnd the median and permute ind
		node->_idx  = ind[medianPos];                         // store this permutation idx
		node->_median = _ps(ind[medianPos], axis);            // store median value

#ifdef DEBUG_KDTREE
		std::cout << "Median = " << medianPos << " value " << _ps(ind[medianPos], axis) << "\n";
#endif
		// partition the remaining points for depth+1
		//
		Ind indLeft, indRight;               // left, right indices  
		for (i = 0 ; i < medianPos; i++)
			indLeft.push_back(ind[i]);
		for (i = medianPos+1 ; i < ind.size(); i++)
			indRight.push_back(ind[i]);

		// recurse
		//
		if (indLeft.size()>0)
			node->_left = _build(indLeft, depth+1);   // recursively build left

		if (indRight.size()>0)
			node->_right = _build(indRight, depth+1);     // recursively buuld right
		
		return node;
	}

	// visit the tree
	void KDtree::depthFirstVisit(Node * n) const
	{
		std::cout << "index=" << n->_idx << " ";
		for (unsigned int j=0; j < dim(); j++)
		{
			std::cout << _ps(n->_idx, j) << " ";
		}
		std::cout << "\n";

		if (n->_left != NULL)
			depthFirstVisit(n->_left);
		if (n->_right != NULL)
			depthFirstVisit(n->_right);
	}

	// return nearest
	NNreturn KDtree::_returnNearest(const Node * n, const Point& p, unsigned int depth) const
	{
		unsigned axis;       
		double d, d2, d3;
		NNreturn ret;
		const Node *bestGuess; const Node *bestGuess2; const Node *bestGuess3;
	
		d = d2 = d3 = 0.0;

		if (n->_left == NULL)
		{
			std::cout << "leaf\n";
			d = _distance(n->_idx, p);
			return boost::make_tuple(n, d, 0);  // reached a leaf
		}

		axis = depth % dim();               // select the axis

		// go down recursively
		if (n->_median < p[axis])
		{
			std::cout << "left\n";
			ret = _returnNearest(n->_left, p, depth+1);
		}
		else 
		{
			std::cout << "right\n";
			ret = _returnNearest(n->_right, p, depth+1);
		}

		if (ret.get<2>() >= 2) // the height
		{
				// check the sibling
			if (n->_median < p[axis]) 
				ret = _returnNearest(n->_right, p, depth+1);
			else 
				ret = _returnNearest(n->_left, p, depth+1);			
				
			bestGuess2 = ret.get<0>(); 
			d2 = ret.get<1>();
			std::cout << "node=" << ret.get<0>() << " d=" << d2 << " h=" << ret.get<2>() << "\n";
		}

		// check current node
		d3 = _distance(n->_idx, p);
		// is this improving current best guess?
		if (d3 < d2)
		{
			d2 = d3;
			bestGuess2;
		}
		if (d2 < d)
		{
			d = d2;
			bestGuess = bestGuess2;
		}
		
		return boost::make_tuple(n, d, 0);
	}
	// 
	// compute distance
	//
	double KDtree::_distance(unsigned int i, const Point & p) const
	{
		double d = 0.0;
		std::cout << "p size=" << p.size() << "\n";
		for (unsigned int j = 0; j < p.size(); ++j){
		
			d += ((_ps(i, j) - p[j]) * (_ps(i, j) - p[j]));
			std::cout << "  distance d=" << d << "\n";
		}
		return d;
	}



	// classical find median
	// some additional information is used
	//
	//		a permutation of ind is computed
	//
	void KDtree::_find_median(Ind & ind, unsigned int axis, 
							unsigned int left, unsigned int right, unsigned int k)
	{
		unsigned int i;
		while (right > left) {
			i = _partition(ind, axis, left, right);
			if (i <= k)
				left = i + 1;
			else 
				right = i - 1;
		}
	}
	// classical partition ~ quicksort
	// some additional information is used
	//		ind is used to mantain the swapped indices
	//		axis is current selected axis
	//
	unsigned int KDtree::_partition(Ind & ind, unsigned int axis, 
									unsigned int left, unsigned int right)
	{
		coordinate x;
		unsigned int storeIndex, i, t;
		unsigned int pivot = left;    // FIXME; select a random value in [left, right]

#ifdef DEBUG_KDTREE
		std::cout << "\t\tPartition  (" << ind[pivot] << ", " << axis << ") left="
			<< left << " right=" << right << "\n";
		__dump_data_with_ind(ind, axis);
#endif

		// indirect access using ind to the value of the pivot position
		x = _ps(ind[pivot], axis);
		storeIndex = left;

#ifdef DEBUG_KDTREE
		std::cout << "\t\tPivot value = " << x << "\n";
#endif

		// swap pivot at the end in ind
		 t=ind[pivot]; ind[pivot]=ind[right]; ind[right]=t;
	
		 for (i = left; i < right; i++){
		
			 if (_ps(ind[i], axis) <= x){	

#ifdef DEBUG_KDTREE
				 std::cout << "\t\t\t(Swapping i=" << i << " storeIndex=" << storeIndex << ")\n" ;
#endif

				 // mantain the invariant					
				 t=ind[i]; ind[i]=ind[storeIndex]; ind[storeIndex]=t;
				storeIndex++;
			}
		}
        // pivot in final position
		t=ind[storeIndex]; ind[storeIndex]=ind[right]; ind[right]=t;

#ifdef DEBUG_KDTREE
		__dump_data_with_ind(ind, axis);
#endif

		return storeIndex;
	};

	void KDtree::__dump_data_with_ind(Ind & ind, unsigned int axis){

		std::cout << ind;
		for (unsigned int i=0 ; i < ind.size(); i++)
			std::cout << _ps(ind[i], axis) << " ";
		std::cout << "\n";
	}
};