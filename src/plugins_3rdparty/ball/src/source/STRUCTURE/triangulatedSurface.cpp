#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//

#include <BALL/STRUCTURE/triangulatedSurface.h>

#include <iterator>
#include <map>

namespace BALL
{
	TriangulatedSurface::TriangulatedSurface()
		:	number_of_points_(0),
			points_(),
			number_of_edges_(0),
			edges_(),
			number_of_triangles_(0),
			triangles_()
	{
	}


	TriangulatedSurface::TriangulatedSurface(const TriangulatedSurface& surface, bool)
		:	number_of_points_(0),
			points_(),
			number_of_edges_(0),
			edges_(),
			number_of_triangles_(0),
			triangles_()
	{
		copy(surface);
	}


	TriangulatedSurface::~TriangulatedSurface()
	{
		clear();
	}

	TriangulatedSurface* TriangulatedSurface::createTube(unsigned int num_vertices, unsigned int subdiv, bool closed, bool out)
	{
		TriangulatedSurface* result = new TriangulatedSurface();

		//Compute the number of elements
		result->number_of_points_ = num_vertices * (subdiv + 2);
		result->number_of_edges_ =  num_vertices * (3 * subdiv + 4);
		result->number_of_triangles_ = 2 * num_vertices * (subdiv + 1);

		//Allocate temporary storage for the elements
		std::vector<TrianglePoint*> points(result->number_of_points_);
		std::vector<TriangleEdge*> edges(result->number_of_edges_);
		std::vector<Triangle*> triangles(result->number_of_triangles_);

		//If the disk is closed there are more elements.
		//However we need no temporary storage for them
		result->number_of_points_    += closed ? 2 : 0;
		result->number_of_edges_     += closed ? 2*num_vertices : 0;
		result->number_of_triangles_ += closed ?   num_vertices : 0;

		const double angle = 2*M_PI/num_vertices;
		const double spacing = 1./(subdiv + 1);

		//First create all vertices
		for(unsigned int i = 0; i < num_vertices; ++i) {
			for(unsigned int j = 0; j < subdiv + 2; ++j) {
				TVector3<double> coords(cos(i*angle), sin(i*angle), j*spacing);
				TVector3<double> normal(cos(i*angle), sin(i*angle), 0);
				points[i + j*num_vertices] = new TrianglePoint(coords, out ? normal : -normal);
			}
		}

		//Create all edges and all triangles "pointing" upwards.
		for(unsigned int j = 0; j < subdiv + 1; ++j) {
			for(unsigned int i = 0; i < num_vertices - 1; ++i) {
				TriangleEdge* e1 = new TriangleEdge(points[num_vertices * j + i],       points[num_vertices * j + i + 1]);
				TriangleEdge* e2 = new TriangleEdge(points[num_vertices * j + i + 1],   points[num_vertices * (j + 1) + i + 1]);
				TriangleEdge* e3 = new TriangleEdge(points[num_vertices * (j + 1) + i + 1], points[num_vertices * j + i]);
				//Store the horizontal edges
				edges[num_vertices * j + i] = e1;
				//Store the vertical edges
				edges[(  subdiv+2 + j) * num_vertices + i] = e2;
				//Store the diagonal edges
				edges[(2*subdiv+3 + j) * num_vertices + i] = e3;

				Triangle* t = new Triangle(e1, e2, e3, out);

				triangles[2*num_vertices*j + 2*i] = t;
			}

			//Special case dealing with the ring closing
			TriangleEdge* e1 = new TriangleEdge(points[num_vertices * j + num_vertices - 1], points[num_vertices * j ]);
			TriangleEdge* e2 = new TriangleEdge(points[num_vertices * j],                    points[num_vertices * (j + 1) ]);
			TriangleEdge* e3 = new TriangleEdge(points[num_vertices * (j + 1)],              points[num_vertices * j + num_vertices - 1]);
			edges[num_vertices * j + num_vertices - 1] = e1;
			edges[(  subdiv+2 + j) * num_vertices + num_vertices - 1] = e2;
			edges[(2*subdiv+3 + j) * num_vertices + num_vertices - 1] = e3;

			Triangle* t = new Triangle(e1, e2, e3, out);
			e1->face_[0] = e2->face_[0] = e3->face_[0] = t;

			triangles[2*num_vertices*j + 2*(num_vertices - 1)] = t;
		}

		//Create the topmost horizontal edges
		for(unsigned int i = 0; i < num_vertices - 1; ++i) {
			edges[num_vertices * (subdiv + 1) + i] = new TriangleEdge(points[num_vertices * (subdiv+1) + i], points[num_vertices * (subdiv+1) + i +1]);
		}

		//Again ring closing code
		edges[num_vertices * (subdiv + 1) + num_vertices - 1] = new TriangleEdge(points[num_vertices * (subdiv+1) + num_vertices - 1],
		                                                                         points[num_vertices * (subdiv+1)]);

		//Create all triangles pointing downward
		//This special case is needed to set the edges faces appropriately
		for(int i = num_vertices - 1; i >= 0; --i) {
			TriangleEdge* e1 = edges[num_vertices * (subdiv + 1) + i];
			TriangleEdge* e2 = edges[(2*subdiv + 3 + subdiv) * num_vertices + i];
			TriangleEdge* e3 = edges[(  subdiv + 2 + subdiv) * num_vertices + i];

			Triangle* t = new Triangle(e1, e2, e3, !out);

			e1->face_[0] = e2->face_[0] = e3->face_[0] = t;

			triangles[2*num_vertices*subdiv + 2*i + 1] = t;
		}

		for(unsigned int j = subdiv; j > 0; --j) {
			for(int i = num_vertices - 1; i >= 0; --i) {
				TriangleEdge* e1 = edges[num_vertices * j + i];
				TriangleEdge* e2 = edges[(2*subdiv + 3 + j - 1) * num_vertices + i];
				TriangleEdge* e3 = edges[(  subdiv + 2 + j - 1) * num_vertices + i];

				Triangle* t = new Triangle(e1, e2, e3, !out);

				e1->face_[1] = e2->face_[1] = e3->face_[1] = t;

				triangles[2*num_vertices*(j-1) + 2*i + 1] = t;
			}
		}

		std::copy(triangles.begin(), triangles.end(), std::back_inserter(result->triangles_));

		//Build the two endcaps if necessary
		if(closed) {
			TrianglePoint* p1 = new TrianglePoint(TVector3<double>(0, 0, 0), out ? TVector3<double>(0,0,-1) : TVector3<double>(0,0,1));
			TrianglePoint* p2 = new TrianglePoint(TVector3<double>(0, 0, 1), out ? TVector3<double>(0,0,1) : TVector3<double>(0,0,-1));
			result->points_.push_back(p1);
			result->points_.push_back(p2);

			//Remember
			TriangleEdge* e_old1 = new TriangleEdge(p1, points[0]);
			TriangleEdge* e1 = e_old1;
			result->edges_.push_back(e1);

			TriangleEdge* e_old2 = new TriangleEdge(p2, points[num_vertices * (subdiv + 1)]);
			TriangleEdge* e2 = e_old2;
			result->edges_.push_back(e2);
			for(unsigned int i = 1; i < num_vertices; ++i) {
				TriangleEdge* e_new1 = new TriangleEdge(p1, points[i]);
				result->edges_.push_back(e_new1);

				TriangleEdge* e_top1 = edges[i - 1];
				Triangle* t = new Triangle(e_old1, e_top1, e_new1, !out);
				e_old1->face_[1] = e_top1->face_[1] = e_new1->face_[0] = t;
				//The triangles belonging to the lower cap should go to the front
				result->triangles_.push_front(t);

				TriangleEdge* e_new2 = new TriangleEdge(p2, points[num_vertices * (subdiv + 1) + i]);
				result->edges_.push_back(e_new2);

				TriangleEdge* e_top2 = edges[(subdiv + 1)*num_vertices + i - 1];
				t = new Triangle(e_old2, e_top2, e_new2, out);
				e_old2->face_[1] = e_top2->face_[1] = e_new2->face_[0] = t;
				//The triangles belonging to the lower cap should go to the back
				result->triangles_.push_back(t);

				e_old1 = e_new1;
				e_old2 = e_new2;
			}

			//Ring closing code
			TriangleEdge* e_top1 = edges[             num_vertices - 1];
			TriangleEdge* e_top2 = edges[(subdiv + 2)*num_vertices - 1];

			Triangle* t = new Triangle(e_old1, e_top1, e1, !out);
			e_old1->face_[1] = e_top1->face_[1] = e1->face_[0] = t;
			result->triangles_.push_front(t);

			t = new Triangle(e_old2, e_top2, e2, out);
			e_old2->face_[1] = e_top2->face_[1] = e2->face_[0] = t;
			result->triangles_.push_back(t);
		}

		std::copy(points.begin(), points.end(), std::back_inserter(result->points_));
		std::copy(edges.begin(), edges.end(), std::back_inserter(result->edges_));

		return result;
	}

	TriangulatedSurface* TriangulatedSurface::createDisk(unsigned int num_vertices, bool out)
	{
		TriangulatedSurface* result = new TriangulatedSurface();

		//Compute the amount of elements
		result->number_of_points_   = num_vertices + 1;
		result->number_of_edges_    = 2*num_vertices;
		result->number_of_triangles_= num_vertices;

		const double angle = 2*M_PI/num_vertices;
		const TVector3<double> normal(0, 0 , out ? 1 : -1);

		//Compute center vertex and store the first vertex/edge in an own variable
		TrianglePoint* center = new TrianglePoint(TVector3<double>(0, 0, 0), normal);
		result->points_.push_back(center);

		TrianglePoint* p_old = new TrianglePoint(TVector3<double>(1, 0, 0), normal);
		TrianglePoint* p1 = p_old;
		result->points_.push_back(p1);

		TriangleEdge* e1 = new TriangleEdge(p1, center);
		TriangleEdge* e_old = e1;
		result->edges_.push_back(e1);

		//Triangulate the disk
		for(unsigned int i = 1; i < num_vertices; ++i) {
			TrianglePoint* p     = new TrianglePoint(TVector3<double>(cos(i*angle), sin(i*angle), 0), normal);
			result->points_.push_back(p);

			TriangleEdge*  e     = new TriangleEdge(p, center);
			TriangleEdge*  e_top = new TriangleEdge(p, p_old);
			result->edges_.push_back(e);
			result->edges_.push_back(e_top);

			Triangle* t = new Triangle(e_old, e_top, e, !out);
			e_old->face_[1] = e->face_[0] = e_top->face_[0] = t;
			result->triangles_.push_back(t);

			e_old = e;
			p_old = p;
		}

		//Ring closing code
		TriangleEdge* e_top = new TriangleEdge(p1, p_old);

		Triangle* t = new Triangle(e_old, e_top, e1, !out);
		e_old->face_[1] = e1->face_[0] = e_top->face_[0] = t;
		result->triangles_.push_back(t);

		return result;
	}

	void TriangulatedSurface::clear()
	{
		std::list<TrianglePoint*>::iterator p;
		for (p = points_.begin(); p != points_.end(); p++)
		{
			delete *p;
		}
		std::list<TriangleEdge*>::iterator e;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			delete *e;
		}
		std::list<Triangle*>::iterator t;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			delete *t;
		}
		points_.clear();
		edges_.clear();
		triangles_.clear();
		number_of_points_ = 0;
		number_of_edges_ = 0;
		number_of_triangles_ = 0;
	}


	void TriangulatedSurface::set(const TriangulatedSurface& surface, bool)
		
	{
		if (this != &surface)
		{
			copy(surface);
		}
	}


	TriangulatedSurface& TriangulatedSurface::operator = (const TriangulatedSurface& surface)
		
	{
		if (this != &surface)
		{
			copy(surface);
		}
		return *this;
	}


	void TriangulatedSurface::insert(TrianglePoint* p)
		
	{
		points_.push_back(p);
		number_of_points_++;
	}


	void TriangulatedSurface::insert(TriangleEdge* e)
		
	{
		edges_.push_back(e);
		number_of_edges_++;
	}


	void TriangulatedSurface::insert(Triangle* t)
		
	{
		triangles_.push_back(t);
		number_of_triangles_++;
	}


	Size TriangulatedSurface::numberOfPoints() const
		
	{
		return number_of_points_;
	}

	Size TriangulatedSurface::getNumberOfPoints() const
		
	{
		return number_of_points_;
	}


	Size TriangulatedSurface::numberOfEdges() const
		
	{
		return number_of_edges_;
	}

	Size TriangulatedSurface::getNumberOfEdges() const
		
	{
		return number_of_edges_;
	}

	Size TriangulatedSurface::numberOfTriangles() const
		
	{
		return number_of_triangles_;
	}

	Size TriangulatedSurface::getNumberOfTriangles() const
		
	{
		return number_of_triangles_;
	}

	void TriangulatedSurface::remove(TrianglePoint* point, bool deep)
		
	{
		if (deep)
		{
			HashSet<Triangle*> delete_triangles = point->faces_;
			HashSet<Triangle*>::Iterator t;
			for (t = delete_triangles.begin(); t != delete_triangles.end(); t++)
			{
				(*t)->vertex_[0]->faces_.erase(*t);
				(*t)->vertex_[1]->faces_.erase(*t);
				(*t)->vertex_[2]->faces_.erase(*t);
				(*t)->edge_[0]->remove(*t);
				(*t)->edge_[1]->remove(*t);
				(*t)->edge_[2]->remove(*t);
				triangles_.remove(*t);
				number_of_triangles_--;
				delete *t;
			}
			HashSet<TriangleEdge*> delete_edges = point->edges_;
			HashSet<TriangleEdge*>::Iterator e;
			for (e = delete_edges.begin(); e != delete_edges.end(); e++)
			{
				(*e)->vertex_[0]->edges_.erase(*e);
				(*e)->vertex_[1]->edges_.erase(*e);
				edges_.remove(*e);
				number_of_edges_--;
				delete *e;
			}
		}
		points_.remove(point);
		number_of_points_--;
		delete point;
	}


	void TriangulatedSurface::remove(PointIterator point, bool deep)
		
	{
		if (deep)
		{
			HashSet<Triangle*> delete_triangles = (*point)->faces_;
			HashSet<Triangle*>::Iterator t;
			for (t = delete_triangles.begin(); t != delete_triangles.end(); t++)
			{
				(*t)->vertex_[0]->faces_.erase(*t);
				(*t)->vertex_[1]->faces_.erase(*t);
				(*t)->vertex_[2]->faces_.erase(*t);
				(*t)->edge_[0]->remove(*t);
				(*t)->edge_[1]->remove(*t);
				(*t)->edge_[2]->remove(*t);
				triangles_.remove(*t);
				number_of_triangles_--;
				delete *t;
			}
			HashSet<TriangleEdge*> delete_edges = (*point)->edges_;
			HashSet<TriangleEdge*>::Iterator e;
			for (e = delete_edges.begin(); e != delete_edges.end(); e++)
			{
				(*e)->vertex_[0]->edges_.erase(*e);
				(*e)->vertex_[1]->edges_.erase(*e);
				edges_.remove(*e);
				number_of_edges_--;
				delete *e;
			}
		}
		points_.erase(point);
		number_of_points_--;
		delete *point;
	}


	void TriangulatedSurface::remove(TriangleEdge* edge, bool deep)
		
	{
		if (deep)
		{
			if (edge->face_[0] != NULL)
			{
				remove(edge->face_[0],true);
			}
			if (edge->face_[0] != NULL)
			{
				remove(edge->face_[0],true);
			}
			edge->vertex_[0]->edges_.erase(edge);
			edge->vertex_[1]->edges_.erase(edge);
		}
		edges_.remove(edge);
		number_of_edges_--;
		delete edge;
	}


	void TriangulatedSurface::remove(EdgeIterator e, bool deep)
		
	{
		TriangleEdge& edge = **e;
		if (deep)
		{
			if (edge.face_[0] != NULL)
			{
				remove(edge.face_[0],true);
			}
			if (edge.face_[0] != NULL)
			{
				remove(edge.face_[0],true);
			}
			edge.vertex_[0]->edges_.erase(*e);
			edge.vertex_[1]->edges_.erase(*e);
		}
		edges_.erase(e);
		number_of_edges_--;
		delete &edge;
	}


	void TriangulatedSurface::remove(Triangle* triangle, bool deep)
		
	{
		if (deep)
		{
			triangle->vertex_[0]->faces_.erase(triangle);
			triangle->vertex_[1]->faces_.erase(triangle);
			triangle->vertex_[2]->faces_.erase(triangle);
			triangle->edge_[0]->remove(triangle);
			triangle->edge_[1]->remove(triangle);
			triangle->edge_[2]->remove(triangle);
		}
		triangles_.remove(triangle);
		number_of_triangles_--;
		delete triangle;
	}


	void TriangulatedSurface::remove(TriangleIterator t, bool deep)
		
	{
		Triangle& tri = **t;
		if (deep)
		{
			tri.vertex_[0]->faces_.erase(*t);
			tri.vertex_[1]->faces_.erase(*t);
			tri.vertex_[2]->faces_.erase(*t);
			tri.edge_[0]->remove(*t);
			tri.edge_[1]->remove(*t);
			tri.edge_[2]->remove(*t);
		}
		triangles_.erase(t);
		number_of_triangles_--;
		delete &tri;
	}


	void TriangulatedSurface::exportSurface(Surface& surface)
		
	{
		std::list<TrianglePoint*>::iterator p;
		Index i = 0;
		Vector3 point;
		Vector3 normal;
		for (p = points_.begin(); p != points_.end(); p++)
		{
			TrianglePoint& tri_point = **p;
			point.set((float)tri_point.point_.x,
								(float)tri_point.point_.y,
								(float)tri_point.point_.z);
			normal.set((float)tri_point.normal_.x,
								 (float)tri_point.normal_.y,
								 (float)tri_point.normal_.z);
			surface.vertex.push_back(point);
			surface.normal.push_back(normal);
			tri_point.index_ = i;
			i++;
		}
		std::list<Triangle*>::iterator t;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			Surface::Triangle triangle;
			triangle.v1 = (*t)->vertex_[0]->index_;
			triangle.v2 = (*t)->vertex_[1]->index_;
			triangle.v3 = (*t)->vertex_[2]->index_;
			surface.triangle.push_back(triangle);
		}
	}


	TriangulatedSurface& TriangulatedSurface::operator += (const TriangulatedSurface& surface)
		
	{
		std::list<TrianglePoint*>::const_iterator p;
		for (p = surface.points_.begin(); p != surface.points_.end(); p++)
		{
			points_.push_back(*p);
		}
		std::list<TriangleEdge*>::const_iterator e;
		for (e = surface.edges_.begin(); e != surface.edges_.end(); e++)
		{
			edges_.push_back(*e);
		}
		std::list<Triangle*>::const_iterator t;
		for (t = surface.triangles_.begin(); t != surface.triangles_.end(); t++)
		{
			triangles_.push_back(*t);
		}
		number_of_points_ += surface.number_of_points_;
		number_of_edges_ += surface.number_of_edges_;
		number_of_triangles_ += surface.number_of_triangles_;
		return *this;
	}


	void TriangulatedSurface::join(TriangulatedSurface& source)
	{
		points_.splice(points_.end(),source.points_);
		edges_.splice(edges_.end(),source.edges_);
		triangles_.splice(triangles_.end(),source.triangles_);
		number_of_points_ += source.number_of_points_;
		number_of_edges_ += source.number_of_edges_;
		number_of_triangles_ += source.number_of_triangles_;
		source.number_of_points_ = 0;
		source.number_of_edges_ = 0;
		source.number_of_triangles_ = 0;
	}

	void TriangulatedSurface::shift(const TVector3<double>& c)
	{
		std::list<TrianglePoint*>::iterator i;
		for (i = points_.begin(); i != points_.end(); i++)
		{
			(*i)->point_ += c;
		}
	}

	void TriangulatedSurface::blowUp(const double& r)
	{
		std::list<TrianglePoint*>::iterator i;
		for (i = points_.begin(); i != points_.end(); i++)
		{
			(*i)->point_ *= r;
		}
	}

	void TriangulatedSurface::setIndices()
	{
		Index i = 0;
		std::list<TrianglePoint*>::iterator p;
		for (p = points_.begin(); p != points_.end(); p++)
		{
			(*p)->index_ = i;
			i++;
		}
		i = 0;
		std::list<TriangleEdge*>::iterator e;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			(*e)->index_ = i;
			i++;
		}
		i = 0;
		std::list<Triangle*>::iterator t;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			(*t)->index_ = i;
			i++;
		}
	}

	void TriangulatedSurface::setDensity(const double& density)
	{
		density_ = density;
	}
	
	double TriangulatedSurface::getDensity() const
	{
		return density_;
	}
	
	void TriangulatedSurface::cut(const TPlane3<double>& plane, const double& fuzzy)
	{
		// delete all points on the wrong side of the plane
		std::list<TrianglePoint*>::iterator p;
		std::list<TrianglePoint*>::iterator next_point;
		double test_value;
		test_value = plane.n*plane.p+fuzzy;
		p = points_.begin();
		while (p != points_.end())
		{
			if (Maths::isLessOrEqual(plane.n*(*p)->point_,test_value))
			{
				next_point = p;
				next_point++;
				delete *p;
				if (next_point == points_.end())
				{
					points_.erase(p);
					p = points_.end();
				}
				else
				{
					points_.erase(p);
					p = next_point;
				}
				number_of_points_--;
			}
			else
			{
				p++;
			}
		}
	}


	void TriangulatedSurface::shrink()
	{
		// delete all border triangles
		std::list<Triangle*> delete_triangles;
		std::list<Triangle*>::iterator t;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			if (((*t)->edge_[0]->face_[0] == NULL) || ((*t)->edge_[0]->face_[1] == NULL) ||
					((*t)->edge_[1]->face_[0] == NULL) || ((*t)->edge_[1]->face_[1] == NULL) ||
					((*t)->edge_[2]->face_[0] == NULL) || ((*t)->edge_[2]->face_[1] == NULL)		)
			{
				delete_triangles.push_back(*t);
			}
		}
		for (t = delete_triangles.begin(); t != delete_triangles.end(); t++)
		{
			remove(*t,true);
		}
		// delete all "isolated" edges (edges with no triangles)
		std::list<TriangleEdge*>::iterator e = edges_.begin();
		std::list<TriangleEdge*>::iterator next_edge;
		while (e != edges_.end())
		{
			if (((*e)->face_[0] == NULL) && ((*e)->face_[1] == NULL))
			{
				next_edge = e;
				next_edge++;
				(*e)->vertex_[0]->edges_.erase(*e);
				(*e)->vertex_[1]->edges_.erase(*e);
				delete *e;
				if (next_edge == edges_.end())
				{
					edges_.erase(e);
					e = edges_.end();
				}
				else
				{
					edges_.erase(e);
					e = next_edge;
				}
				number_of_edges_--;
			}
			else
			{
				e++;
			}
		}
	}


	void TriangulatedSurface::deleteIsolatedEdges()
		
	{
		std::list<TriangleEdge*>::iterator e1;
		std::list<TriangleEdge*>::iterator e2;
		e1 = edges_.begin();
		while (e1 != edges_.end())
		{
			if ((*e1)->face_[0] == NULL)
			{
				e2 = e1;
				e2++;
				if (e2 == edges_.end())
				{
					remove(e1);
					e1 = edges_.end();
				}
				else
				{
					remove(e1);
					e1 = e2;
				}
			}
			else
			{
				e1++;
			}
		}
	}


	void TriangulatedSurface::deleteIsolatedPoints()
		
	{
		std::list<TrianglePoint*>::iterator p1;
		std::list<TrianglePoint*>::iterator p2;
		p1 = points_.begin();
		while (p1 != points_.end())
		{
			if ((*p1)->faces_.size() == 0)
			{
				delete *p1;
				p2 = points_.erase(p1);
				p1 = p2;
				number_of_points_--;
			}
			else
			{
				p1++;
			}
		}
	}


	void TriangulatedSurface::getBorder(std::list<TriangleEdge*>& border)
	{
		std::list<TriangleEdge*>::iterator e;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			if (((*e)->face_[0] == NULL) || ((*e)->face_[1] == NULL))
			{
				border.push_back(*e);
			}
		}
	}


	TriangulatedSurface::PointIterator
			TriangulatedSurface::beginPoint()
		
	{
		return points_.begin();
	}


	TriangulatedSurface::ConstPointIterator
			TriangulatedSurface::beginPoint() const
		
	{
		return points_.begin();
	}


	TriangulatedSurface::PointIterator
			TriangulatedSurface::endPoint()
		
	{
		return points_.end();
	}


	TriangulatedSurface::ConstPointIterator
			TriangulatedSurface::endPoint() const
		
	{
		return points_.end();
	}


	TriangulatedSurface::EdgeIterator
			TriangulatedSurface::beginEdge()
		
	{
		return edges_.begin();
	}


	TriangulatedSurface::ConstEdgeIterator
			TriangulatedSurface::beginEdge() const
		
	{
		return edges_.begin();
	}


	TriangulatedSurface::EdgeIterator
			TriangulatedSurface::endEdge()
		
	{
		return edges_.end();
	}


	TriangulatedSurface::ConstEdgeIterator
			TriangulatedSurface::endEdge() const
		
	{
		return edges_.end();
	}


	TriangulatedSurface::TriangleIterator
			TriangulatedSurface::beginTriangle()
		
	{
		return triangles_.begin();
	}


	TriangulatedSurface::ConstTriangleIterator
			TriangulatedSurface::beginTriangle() const
		
	{
		return triangles_.begin();
	}


	TriangulatedSurface::TriangleIterator
			TriangulatedSurface::endTriangle()
		
	{
		return triangles_.end();
	}


	TriangulatedSurface::ConstTriangleIterator
			TriangulatedSurface::endTriangle() const
		
	{
		return triangles_.end();
	}


	bool TriangulatedSurface::canBeCopied() const
		
	{
		std::list<TrianglePoint*>::const_iterator p;
		Index i = 0;
		for (p = points_.begin(); p != points_.end(); p++)
		{
			if (*p == NULL)
			{
				Log.error() << "Error: TriangulatedSurface contains null pointer!" << std::endl;
				return false;
			}
			if ((*p)->index_ != i)
			{
				Log.error() << "Error: TriangulatedSurface contains index mismatch!" << std::endl;
				return false;
			}
			i++;
		}
		std::list<TriangleEdge*>::const_iterator e;
		i = 0;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			if (*e == NULL)
			{
				return false;
			}
			if ((*e)->index_ != i)
			{
				return false;
			}
			i++;
		}
		std::list<Triangle*>::const_iterator t;
		i = 0;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			if (*t == NULL)
			{
				return false;
			}
			if ((*t)->index_ != i)
			{
				return false;
			}
			i++;
		}
		return true;
	}


	void TriangulatedSurface::copy(const TriangulatedSurface& surface)
		
	{
		if (surface.canBeCopied())
		{
			number_of_points_ = surface.number_of_points_;
			number_of_edges_ = surface.number_of_edges_;
			number_of_triangles_ = surface.number_of_triangles_;
			std::vector<TrianglePoint*> point_vector(number_of_points_);
			std::list<TrianglePoint*>::const_iterator p;
			Position i = 0;
			for (p = surface.points_.begin(); p != surface.points_.end(); p++)
			{
				point_vector[i] = new TrianglePoint(**p,false);
				points_.push_back(point_vector[i]);
				i++;
			}
			std::vector<TriangleEdge*> edge_vector(number_of_edges_);
			std::list<TriangleEdge*>::const_iterator e;
			i = 0;
			for (e = surface.edges_.begin(); e != surface.edges_.end(); e++)
			{
				edge_vector[i] = new TriangleEdge(**e,false);
				edges_.push_back(edge_vector[i]);
				i++;
			}
			std::vector<Triangle*> triangle_vector(number_of_triangles_);
			std::list<Triangle*>::const_iterator t;
			i = 0;
			for (t = surface.triangles_.begin(); t != surface.triangles_.end(); t++)
			{
				triangle_vector[i] = new Triangle(**t,false);
				triangles_.push_back(triangle_vector[i]);
				i++;
			}
			HashSet<TriangleEdge*>::ConstIterator he;
			HashSet<Triangle*>::ConstIterator ht;
			i = 0;
			for (p = surface.points_.begin(); p != surface.points_.end(); p++)
			{
				for (he = (*p)->edges_.begin(); he != (*p)->edges_.end(); he++)
				{
					point_vector[i]->edges_.insert(edge_vector[(*he)->index_]);
				}
				for (ht = (*p)->faces_.begin(); ht != (*p)->faces_.end(); ht++)
				{
					point_vector[i]->faces_.insert(triangle_vector[(*ht)->index_]);
				}
				i++;
			}
			i = 0;
			for (e = surface.edges_.begin(); e != surface.edges_.end(); e++)
			{
				edge_vector[i]->vertex_[0] = point_vector[(*e)->vertex_[0]->index_];
				edge_vector[i]->vertex_[1] = point_vector[(*e)->vertex_[1]->index_];
				if ((*e)->face_[0] != NULL)
				{
					edge_vector[i]->face_[0] = triangle_vector[(*e)->face_[0]->index_];
				}
				if ((*e)->face_[1] != NULL)
				{
					edge_vector[i]->face_[1] = triangle_vector[(*e)->face_[1]->index_];
				}
				i++;
			}
			i = 0;
			for (t = surface.triangles_.begin(); t != surface.triangles_.end(); t++)
			{
				triangle_vector[i]->vertex_[0] = point_vector[(*t)->vertex_[0]->index_];
				triangle_vector[i]->vertex_[1] = point_vector[(*t)->vertex_[1]->index_];
				triangle_vector[i]->vertex_[2] = point_vector[(*t)->vertex_[2]->index_];
				triangle_vector[i]->edge_[0] = edge_vector[(*t)->edge_[0]->index_];
				triangle_vector[i]->edge_[1] = edge_vector[(*t)->edge_[1]->index_];
				triangle_vector[i]->edge_[2] = edge_vector[(*t)->edge_[2]->index_];
				i++;
			}
		}
		else
		{
			Log.error() << "Error: surface can not be copied!" << std::endl;
		}
	}

///////////////////////////////////////////////////////////////////////////////


	TriangulatedSphere::TriangulatedSphere()
		:	TriangulatedSurface()
	{
	}


	TriangulatedSphere::TriangulatedSphere(const TriangulatedSphere& sphere, bool)
		:	TriangulatedSurface(sphere)
	{
	}


	TriangulatedSphere::~TriangulatedSphere()
	{
	}


	void TriangulatedSphere::set(const TriangulatedSphere& sphere, bool)
	{
		if (this != &sphere)
		{
			copy(sphere);
		}
	}


	TriangulatedSphere& TriangulatedSphere::operator = (const TriangulatedSphere& sphere)
	{
		if (this != &sphere)
		{
			copy(sphere);
		}
		return *this;
	}


	void TriangulatedSphere::refine(Position iterations, bool out)
	{
		for (Position i = 0; i < iterations; i++)
		{
			refine(out);
		}
		std::list<Triangle*>::iterator t;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			TVector3<double> norm(((*t)->vertex_[1]->point_-(*t)->vertex_[0]->point_) %
											 ((*t)->vertex_[2]->point_-(*t)->vertex_[0]->point_)	);
			if ((Maths::isGreater(norm*(*t)->vertex_[0]->point_,0) &&
					 (out == false)																				) ||
					(Maths::isLess(norm*(*t)->vertex_[0]->point_,0) &&
					 (out == true)																				)		)
			{
				TrianglePoint* temp = (*t)->vertex_[1];
				(*t)->vertex_[1] = (*t)->vertex_[2];
				(*t)->vertex_[2] = temp;
			}
		}
		setIncidences();
	}


	void TriangulatedSphere::refine(bool out)
	{
		std::vector<Face> faces(number_of_triangles_);
		std::list<Triangle*>::iterator t;
		Position i = 0;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			(*t)->index_ = i;
			faces[i].p[0] = (*t)->vertex_[0];
			faces[i].p[1] = (*t)->vertex_[1];
			faces[i].p[2] = (*t)->vertex_[2];
			faces[i].pcount = 3;
			faces[i].ecount = 0;
			i++;
		}
		std::list<TriangleEdge*> new_edges;
		std::list<TriangleEdge*>::iterator e;
		int counter = 0;
		for (e = edges_.begin(); e != edges_.end(); e++, counter++)
		{
			TrianglePoint* point1 = (*e)->vertex_[0];
			TrianglePoint* point2 = (*e)->vertex_[1];
			TrianglePoint* new_point = new TrianglePoint;
			new_point->point_ = (point1->point_+point2->point_).normalize();
			if (out == true)
			{
				new_point->normal_ = new_point->point_;
			}
			else
			{
				new_point->normal_ = -new_point->point_;
			}
			TriangleEdge* new_edge1 = *e;
			new_edge1->vertex_[0] = point1;
			new_edge1->vertex_[1] = new_point;
			TriangleEdge* new_edge2 = new TriangleEdge;
			new_edge2->vertex_[0] = point2;
			new_edge2->vertex_[1] = new_point;
			i = (*e)->face_[0]->index_;
			faces[i].p[faces[i].pcount] = new_point;
			faces[i].pcount++;
			faces[i].e[faces[i].ecount] = new_edge1;
			faces[i].e[faces[i].ecount+1] = new_edge2;
			faces[i].ecount += 2;
			i = (*e)->face_[1]->index_;
			faces[i].p[faces[i].pcount] = new_point;
			faces[i].pcount++;
			faces[i].e[faces[i].ecount] = new_edge1;
			faces[i].e[faces[i].ecount+1] = new_edge2;
			faces[i].ecount += 2;
			new_edge1->face_[0] = NULL;
			new_edge1->face_[1] = NULL;
			new_edge2->face_[0] = NULL;
			new_edge2->face_[1] = NULL;
			points_.push_back(new_point);
			new_edges.push_back(new_edge2);
		}
		edges_.splice(edges_.end(),new_edges);
		i = 0;
		std::list<Triangle*> new_triangles;
		for (t = triangles_.begin(); t != triangles_.end(); t++)
		{
			// create four new triangles
			Triangle* triangle[3];
			for (Position k = 0; k < 3; k++)
			{
				triangle[k] = new Triangle;
			}
			// create three new edges
			for (Position k = 6; k < 9; k++)
			{
				faces[i].e[k] = new TriangleEdge;
				faces[i].e[k]->vertex_[0] = faces[i].p[k-3];
				faces[i].e[k]->vertex_[1] = faces[i].p[(k-5)%3+3];
				faces[i].e[k]->face_[0] = NULL;
				faces[i].e[k]->face_[1] = NULL;
				edges_.push_back(faces[i].e[k]);
			}
			// build the four triangles replacing the old one
			buildFourTriangles(faces[i],
												 triangle[0],triangle[1],
												 triangle[2],*t);
			new_triangles.push_back(triangle[0]);
			new_triangles.push_back(triangle[1]);
			new_triangles.push_back(triangle[2]);
			i++;
		}
		triangles_.splice(triangles_.end(),new_triangles);
		number_of_points_ += number_of_edges_;
		number_of_edges_ *= 4;
		number_of_triangles_ *= 4;
	}


	void TriangulatedSphere::buildFourTriangles
			(Face face,
			 Triangle* face0,
			 Triangle* face1,
			 Triangle* face2,
			 Triangle* face3)
		
	{
		Triangle* triangle[3];
		triangle[0] = face0;
		triangle[1] = face1;
		triangle[2] = face2;
		TriangleEdge* edge[3];
		edge[0] = NULL;
		edge[1] = NULL;
		edge[2] = NULL;
		for (Position k = 0; k < 3; k++)
		{
			// create a smaller triangle containing face.p[k]
			TriangleEdge* first = NULL;
			TriangleEdge* second = NULL;
			TrianglePoint* p1 = NULL;
			TrianglePoint* p2 = NULL;
			TrianglePoint* p3 = face.p[k];
			Position i = 0;
			while (first == NULL)
			{
				if (face.e[i]->vertex_[0] == p3)
				{
					first = face.e[i];
					p1 = face.e[i]->vertex_[1];
				}
				else
				{
					if (face.e[i]->vertex_[1] == p3)
					{
						first = face.e[i];
						p1 = face.e[i]->vertex_[0];
					}
				}
				i++;
			}
			while (second == NULL)
			{
				if (face.e[i]->vertex_[0] == p3)
				{
					second = face.e[i];
					p2 = face.e[i]->vertex_[1];
				}
				else
				{
					if (face.e[i]->vertex_[1] == p3)
					{
						second = face.e[i];
						p2 = face.e[i]->vertex_[0];
					}
				}
				i++;
			}
			i = 6;
			while (edge[k] == NULL)
			{
				if (((face.e[i]->vertex_[0] == p1) &&
						 (face.e[i]->vertex_[1] == p2)		) ||
						((face.e[i]->vertex_[0] == p2) &&
						 (face.e[i]->vertex_[1] == p1)		)		)
				{
					edge[k] = face.e[i];
				}
				i++;
			}
			triangle[k]->vertex_[0] = p1;
			triangle[k]->vertex_[1] = p2;
			triangle[k]->vertex_[2] = p3;
			triangle[k]->edge_[0] = first;
			triangle[k]->edge_[1] = second;
			triangle[k]->edge_[2] = edge[k];
			if (first->face_[0] == NULL)
			{
				first->face_[0] = triangle[k];
			}
			else
			{
				first->face_[1] = triangle[k];
			}
			if (second->face_[0] == NULL)
			{
				second->face_[0] = triangle[k];
			}
			else
			{
				second->face_[1] = triangle[k];
			}
			edge[k]->face_[0] = triangle[k];
			edge[k]->face_[1] = face3;
		}
		face3->vertex_[0] = face.p[3];
		face3->vertex_[1] = face.p[4];
		face3->vertex_[2] = face.p[5];
		face3->edge_[0] = edge[0];
		face3->edge_[1] = edge[1];
		face3->edge_[2] = edge[2];
	}


	void TriangulatedSphere::setIncidences()
	{
		std::list<TrianglePoint*>::iterator p;
		for (p = points_.begin(); p != points_.end(); p++)
		{
			(*p)->edges_.clear();
			(*p)->faces_.clear();
		}
		std::list<TriangleEdge*>::iterator e;
		for (e = edges_.begin(); e != edges_.end(); e++)
		{
			(*e)->vertex_[0]->edges_.insert(*e);
			(*e)->vertex_[0]->faces_.insert((*e)->face_[0]);
			(*e)->vertex_[0]->faces_.insert((*e)->face_[1]);
			(*e)->vertex_[1]->edges_.insert(*e);
			(*e)->vertex_[1]->faces_.insert((*e)->face_[0]);
			(*e)->vertex_[1]->faces_.insert((*e)->face_[1]);
		}
	}

	struct PointerPairComparator {
		typedef std::pair<TrianglePoint*, TrianglePoint*> Input;

		bool operator()(const Input& p1, const Input& p2) const {
			Input a = p1;
			if((unsigned long)a.first > (unsigned long)a.second) {
				std::swap(a.first, a.second);
			}

			Input b = p2;
			if((unsigned long)b.first > (unsigned long)b.second) {
				std::swap(b.first, b.second);
			}

			return (a.first < b.first) || ((a.first == b.first) && (a.second < b.second));
		}

	};


	typedef std::map<std::pair<TrianglePoint*, TrianglePoint*>, TriangleEdge*, PointerPairComparator> EdgeMap;

	TriangleEdge* getEdge_(EdgeMap& edges, TrianglePoint* a, TrianglePoint* b)
	{
		EdgeMap::iterator res = edges.find(std::make_pair(a, b));
		if(res == edges.end()) {
			res = edges.insert(std::make_pair(std::make_pair(a, b), new TriangleEdge(a, b))).first;
		}

		return res->second;
	}

	void TriangulatedSphere::pentakisDodecaeder(bool out)
	{
		icosaeder(out);

		number_of_points_ = 32;
		number_of_edges_ = 90;
		number_of_triangles_ = 60;

		std::map<Triangle*, TrianglePoint*> new_points;
		for(std::list<Triangle*>::iterator it = triangles_.begin(); it != triangles_.end(); ++it) {
			TrianglePoint* p = new TrianglePoint(((*it)->vertex_[0]->point_ + (*it)->vertex_[1]->point_ + (*it)->vertex_[2]->point_).normalize());

			p->normal_ = out ? p->point_ : -p->point_;

			new_points[*it] = p;

			delete *it;
		}

		EdgeMap edge_map;
		std::list<Triangle*> new_triangles;
		for(std::list<TrianglePoint*>::iterator pt = points_.begin(); pt != points_.end(); ++pt) {
			BALL::HashSet<TriangleEdge*> p_edges((*pt)->edges_);
			for(TrianglePoint::EdgeIterator et = p_edges.begin(); et != p_edges.end(); ++et) {
				TrianglePoint* p1 = new_points[(*et)->getTriangle(0)];
				TrianglePoint* p2 = new_points[(*et)->getTriangle(1)];

				TriangleEdge* e1 = getEdge_(edge_map, *pt, p1);
				TriangleEdge* e2 = getEdge_(edge_map, p1, p2);
				TriangleEdge* e3 = getEdge_(edge_map, p2, *pt);

				Triangle* tri = new Triangle(e1, e2, e3);

				if(e1->getTriangle(0) == 0) {
					e1->setTriangle(0, tri);
				} else {
					e1->setTriangle(1, tri);
				}

				if(e2->getTriangle(0) == 0) {
					e2->setTriangle(0, tri);
				} else {
					e2->setTriangle(1, tri);
				}

				if(e3->getTriangle(0) == 0) {
					e3->setTriangle(0, tri);
				} else {
					e3->setTriangle(1, tri);
				}

				new_triangles.push_back(tri);

				(*pt)->remove((*et)->getTriangle(0));
				(*pt)->remove((*et)->getTriangle(1));
				(*pt)->remove(*et);
			}
		}

		for(std::list<TriangleEdge*>::iterator et = edges_.begin(); et != edges_.end(); ++et) {
			delete *et;
		}

		edges_.clear();

		for(EdgeMap::iterator it = edge_map.begin(); it != edge_map.end(); ++it) {
			edges_.push_back(it->second);
		}

		for(std::map<Triangle*, TrianglePoint*>::iterator it = new_points.begin(); it != new_points.end(); ++it) {
			points_.push_back(it->second);
		}

		std::swap(triangles_, new_triangles);
	}

	void TriangulatedSphere::icosaeder(bool out)
	{
		clear();

		number_of_points_ = 12;
		number_of_edges_ = 30;
		number_of_triangles_ = 20;

		std::vector<TrianglePoint*> points_tmp(number_of_points_);
		points_tmp[0]  = new TrianglePoint(TVector3<double>( 0.0     , 0.0     , 1.0      ));
		points_tmp[1]  = new TrianglePoint(TVector3<double>( 0.894427, 0.0     , 0.4472135));
		points_tmp[2]  = new TrianglePoint(TVector3<double>( 0.276393, 0.850651, 0.4472135));
		points_tmp[3]  = new TrianglePoint(TVector3<double>(-0.723607, 0.525731, 0.4472135));
		points_tmp[4]  = new TrianglePoint(TVector3<double>(-0.723607,-0.525731, 0.4472135));
		points_tmp[5]  = new TrianglePoint(TVector3<double>( 0.276393,-0.850651, 0.4472135));
		points_tmp[6]  = new TrianglePoint(TVector3<double>( 0.723607, 0.525731,-0.4472135));
		points_tmp[7]  = new TrianglePoint(TVector3<double>(-0.276393, 0.850651,-0.4472135));
		points_tmp[8]  = new TrianglePoint(TVector3<double>(-0.894427, 0.0     ,-0.4472135));
		points_tmp[9]  = new TrianglePoint(TVector3<double>(-0.276393,-0.850651,-0.4472135));
		points_tmp[10] = new TrianglePoint(TVector3<double>( 0.723607,-0.525731,-0.4472135));
		points_tmp[11] = new TrianglePoint(TVector3<double>( 0.0     , 0.0     ,-1.0      ));

		for (Position i=0; i < number_of_points_; ++i) {
			points_tmp[i]->normal_ = out ? points_tmp[i]->point_ : -1.*points_tmp[i]->point_;
		}

		std::vector<TriangleEdge*> edges_tmp(number_of_edges_);
		edges_tmp[0] = new TriangleEdge(points_tmp[2], points_tmp[0]);
		edges_tmp[1] = new TriangleEdge(points_tmp[0], points_tmp[1]);
		edges_tmp[2] = new TriangleEdge(points_tmp[1], points_tmp[2]);
		edges_tmp[3] = new TriangleEdge(points_tmp[3], points_tmp[0]);
		edges_tmp[4] = new TriangleEdge(points_tmp[2], points_tmp[3]);
		edges_tmp[5] = new TriangleEdge(points_tmp[4], points_tmp[0]);
		edges_tmp[6] = new TriangleEdge(points_tmp[3], points_tmp[4]);
		edges_tmp[7] = new TriangleEdge(points_tmp[5], points_tmp[0]);
		edges_tmp[8] = new TriangleEdge(points_tmp[4], points_tmp[5]);
		edges_tmp[9] = new TriangleEdge(points_tmp[5], points_tmp[1]);
		edges_tmp[10] = new TriangleEdge(points_tmp[1], points_tmp[6]);
		edges_tmp[11] = new TriangleEdge(points_tmp[6], points_tmp[2]);
		edges_tmp[12] = new TriangleEdge(points_tmp[7], points_tmp[2]);
		edges_tmp[13] = new TriangleEdge(points_tmp[6], points_tmp[7]);
		edges_tmp[14] = new TriangleEdge(points_tmp[7], points_tmp[3]);
		edges_tmp[15] = new TriangleEdge(points_tmp[8], points_tmp[3]);
		edges_tmp[16] = new TriangleEdge(points_tmp[7], points_tmp[8]);
		edges_tmp[17] = new TriangleEdge(points_tmp[8], points_tmp[4]);
		edges_tmp[18] = new TriangleEdge(points_tmp[9], points_tmp[4]);
		edges_tmp[19] = new TriangleEdge(points_tmp[8], points_tmp[9]);
		edges_tmp[20] = new TriangleEdge(points_tmp[9], points_tmp[5]);
		edges_tmp[21] = new TriangleEdge(points_tmp[10], points_tmp[5]);
		edges_tmp[22] = new TriangleEdge(points_tmp[9], points_tmp[10]);
		edges_tmp[23] = new TriangleEdge(points_tmp[1], points_tmp[10]);
		edges_tmp[24] = new TriangleEdge(points_tmp[10], points_tmp[6]);
		edges_tmp[25] = new TriangleEdge(points_tmp[6], points_tmp[11]);
		edges_tmp[26] = new TriangleEdge(points_tmp[11], points_tmp[7]);
		edges_tmp[27] = new TriangleEdge(points_tmp[11], points_tmp[8]);
		edges_tmp[28] = new TriangleEdge(points_tmp[11], points_tmp[9]);
		edges_tmp[29] = new TriangleEdge(points_tmp[11], points_tmp[10]);

		std::vector<Triangle*> triangles_tmp(number_of_triangles_);
		triangles_tmp[0]  = new Triangle(edges_tmp[0],  edges_tmp[1],  edges_tmp[2]);
		triangles_tmp[1]  = new Triangle(edges_tmp[0],  edges_tmp[3],  edges_tmp[4]);
		triangles_tmp[2]  = new Triangle(edges_tmp[3],  edges_tmp[5],  edges_tmp[6]);
		triangles_tmp[3]  = new Triangle(edges_tmp[5],  edges_tmp[7],  edges_tmp[8]);
		triangles_tmp[4]  = new Triangle(edges_tmp[1],  edges_tmp[7],  edges_tmp[9]);
		triangles_tmp[5]  = new Triangle(edges_tmp[2],  edges_tmp[10], edges_tmp[11]);
		triangles_tmp[6]  = new Triangle(edges_tmp[11], edges_tmp[12], edges_tmp[13]);
		triangles_tmp[7]  = new Triangle(edges_tmp[4],  edges_tmp[12], edges_tmp[14]);
		triangles_tmp[8]  = new Triangle(edges_tmp[14], edges_tmp[15], edges_tmp[16]);
		triangles_tmp[9]  = new Triangle(edges_tmp[6],  edges_tmp[15], edges_tmp[17]);
		triangles_tmp[10] = new Triangle(edges_tmp[17], edges_tmp[18], edges_tmp[19]);
		triangles_tmp[11] = new Triangle(edges_tmp[8],  edges_tmp[18], edges_tmp[20]);
		triangles_tmp[12] = new Triangle(edges_tmp[20], edges_tmp[21], edges_tmp[22]);
		triangles_tmp[13] = new Triangle(edges_tmp[10], edges_tmp[23], edges_tmp[24]);
		triangles_tmp[14] = new Triangle(edges_tmp[9],  edges_tmp[21], edges_tmp[23]);
		triangles_tmp[15] = new Triangle(edges_tmp[13], edges_tmp[25], edges_tmp[26]);
		triangles_tmp[16] = new Triangle(edges_tmp[16], edges_tmp[26], edges_tmp[27]);
		triangles_tmp[17] = new Triangle(edges_tmp[19], edges_tmp[27], edges_tmp[28]);
		triangles_tmp[18] = new Triangle(edges_tmp[22], edges_tmp[28], edges_tmp[29]);
		triangles_tmp[19] = new Triangle(edges_tmp[24], edges_tmp[25], edges_tmp[29]);

		edges_tmp[0]->face_[0]  = triangles_tmp[0];  edges_tmp[0]->face_[1]  = triangles_tmp[1];
		edges_tmp[1]->face_[0]  = triangles_tmp[0];  edges_tmp[1]->face_[1]  = triangles_tmp[4];
		edges_tmp[2]->face_[0]  = triangles_tmp[0];  edges_tmp[2]->face_[1]  = triangles_tmp[5];
		edges_tmp[3]->face_[0]  = triangles_tmp[1];  edges_tmp[3]->face_[1]  = triangles_tmp[2];
		edges_tmp[4]->face_[0]  = triangles_tmp[1];  edges_tmp[4]->face_[1]  = triangles_tmp[7];
		edges_tmp[5]->face_[0]  = triangles_tmp[2];  edges_tmp[5]->face_[1]  = triangles_tmp[3];
		edges_tmp[6]->face_[0]  = triangles_tmp[2];  edges_tmp[6]->face_[1]  = triangles_tmp[9];
		edges_tmp[7]->face_[0]  = triangles_tmp[3];  edges_tmp[7]->face_[1]  = triangles_tmp[4];
		edges_tmp[8]->face_[0]  = triangles_tmp[3];  edges_tmp[8]->face_[1]  = triangles_tmp[11];
		edges_tmp[9]->face_[0]  = triangles_tmp[4];  edges_tmp[9]->face_[1]  = triangles_tmp[14];
		edges_tmp[10]->face_[0] = triangles_tmp[5];  edges_tmp[10]->face_[1] = triangles_tmp[13];
		edges_tmp[11]->face_[0] = triangles_tmp[5];  edges_tmp[11]->face_[1] = triangles_tmp[6];
		edges_tmp[12]->face_[0] = triangles_tmp[6];  edges_tmp[12]->face_[1] = triangles_tmp[7];
		edges_tmp[13]->face_[0] = triangles_tmp[6];  edges_tmp[13]->face_[1] = triangles_tmp[15];
		edges_tmp[14]->face_[0] = triangles_tmp[7];  edges_tmp[14]->face_[1] = triangles_tmp[8];
		edges_tmp[15]->face_[0] = triangles_tmp[8];  edges_tmp[15]->face_[1] = triangles_tmp[9];
		edges_tmp[16]->face_[0] = triangles_tmp[8];  edges_tmp[16]->face_[1] = triangles_tmp[16];
		edges_tmp[17]->face_[0] = triangles_tmp[9];  edges_tmp[17]->face_[1] = triangles_tmp[10];
		edges_tmp[18]->face_[0] = triangles_tmp[10]; edges_tmp[18]->face_[1] = triangles_tmp[11];
		edges_tmp[19]->face_[0] = triangles_tmp[10]; edges_tmp[19]->face_[1] = triangles_tmp[17];
		edges_tmp[20]->face_[0] = triangles_tmp[11]; edges_tmp[20]->face_[1] = triangles_tmp[12];
		edges_tmp[21]->face_[0] = triangles_tmp[12]; edges_tmp[21]->face_[1] = triangles_tmp[14];
		edges_tmp[22]->face_[0] = triangles_tmp[12]; edges_tmp[22]->face_[1] = triangles_tmp[18];
		edges_tmp[23]->face_[0] = triangles_tmp[13]; edges_tmp[23]->face_[1] = triangles_tmp[14];
		edges_tmp[24]->face_[0] = triangles_tmp[13]; edges_tmp[24]->face_[1] = triangles_tmp[19];
		edges_tmp[25]->face_[0] = triangles_tmp[15]; edges_tmp[25]->face_[1] = triangles_tmp[19];
		edges_tmp[26]->face_[0] = triangles_tmp[15]; edges_tmp[26]->face_[1] = triangles_tmp[16];
		edges_tmp[27]->face_[0] = triangles_tmp[16]; edges_tmp[27]->face_[1] = triangles_tmp[17];
		edges_tmp[28]->face_[0] = triangles_tmp[17]; edges_tmp[28]->face_[1] = triangles_tmp[18];
		edges_tmp[29]->face_[0] = triangles_tmp[18]; edges_tmp[29]->face_[1] = triangles_tmp[19];

		std::copy(points_tmp.begin(), points_tmp.end(), std::back_inserter(points_));
		std::copy(triangles_tmp.begin(), triangles_tmp.end(), std::back_inserter(triangles_));
		std::copy(edges_tmp.begin(), edges_tmp.end(), std::back_inserter(edges_));
	}


	std::ostream& operator << (std::ostream& s,
														 const TriangulatedSurface& surface)
	{
		s << "Points: " << surface.getNumberOfPoints() << "\n";
		TriangulatedSurface::ConstPointIterator p;
		for (p = surface.beginPoint(); p != surface.endPoint(); p++)
		{
			s << **p << "\n";
		}
		s << "Edges: " << surface.getNumberOfEdges() << "\n";
		TriangulatedSurface::ConstEdgeIterator e;
		for (e = surface.beginEdge(); e != surface.endEdge(); e++)
		{
			s << **e << "\n";
		}
		s << "Triangles: " << surface.getNumberOfTriangles() << "\n";
		TriangulatedSurface::ConstTriangleIterator t;
		for (t = surface.beginTriangle(); t != surface.endTriangle(); t++)
		{
			s << **t << "\n";
		}
		return s;
	}


}	// namespace BALL
