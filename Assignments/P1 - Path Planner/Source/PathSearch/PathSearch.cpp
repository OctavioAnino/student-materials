#include <vector>
#include <cstdlib>
//#include "../Framework/TileSystem/TileMap.h"
#include "PathSearch.h"


namespace ufl_cap4053
{
	namespace searches
	{
		// CLASS DEFINITION GOES HERE

		PathSearch::PathSearch() {};
		PathSearch::~PathSearch() {};

		void PathSearch::load(TileMap* _tileMap) {

		}

		void PathSearch::initialize(int startRow, int startCol, int goalRow, int goalCol) {

		}

		void PathSearch::update(long timeslice) {

		}

		void PathSearch::shutdown() {

		}

		void PathSearch::unload() {

		}

		bool PathSearch::isDone() const {
			return false;
		}

		std::vector<Tile const*> const PathSearch::getSolution() const {
			return tiles_;
		}

	}
}  // close namespace ufl_cap4053::searches
