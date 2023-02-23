#include <vector>
#include <cstdlib>
#include <chrono>
#include <queue>

//#include "../Framework/TileSystem/TileMap.h"
#include "PathSearch.h"



namespace ufl_cap4053
{
	namespace searches
	{
		// CLASS DEFINITION GOES HERE

		struct PathSearch::Private {
			static int** getNeighbors(PathSearch& self, int* node) {
				if (node[0] % 2 == 0) // even row 
				{
					if()
				}
			}
		};

		PathSearch::PathSearch() {};
		PathSearch::~PathSearch() {};

		void PathSearch::load(TileMap* _tileMap) {
			mapCols_ = _tileMap->getColumnCount();
			mapRows_ = _tileMap->getRowCount();

			tiles_ = new Tile * [mapRows_ * mapCols_];
			connections_ = new int[mapCols_ * mapRows_];

			for (int r = 0; r < mapRows_; r++)
			{
				for (int c = 0; c < mapCols_; c++) {
					tiles_[r * mapCols_ + c] = _tileMap->getTile(r, c);
				}
			}
		}

		void PathSearch::initialize(int startRow, int startCol, int goalRow, int goalCol) {
			startRow_ = startRow;
			startCol_ = startCol;

			goalRow_ = goalRow;
			goalCol_ = goalCol;
		}

		/***
		* Called to allow the path planner to execute for the specified timeslice (in milliseconds). 
		Within this method the search should be performed until the time expires or the solution is 
		found. If timeslice is zero (0), this method should only do a single iteration of the 
		algorithm. Otherwise the update should only iterate for the indicated number of milliseconds. 
		This method is always preceded by at least one call to initialize().
		*/

		void PathSearch::update(long timeslice) {
			std::chrono::time_point start = std::chrono::steady_clock::now();

			std::queue<int*>* to_visit = new std::queue<int*>();
			
			int startTile[2] = { startRow_, startCol_ };

			to_visit->push(startTile);

			while (!to_visit->empty())
			{
				int* currentNode = to_visit->front();
				to_visit->pop();
				int** neighbords = Private::getNeighbors(*this, currentNode);


				if (std::chrono::steady_clock::now() - start > std::chrono::milliseconds(timeslice))
					break;
			}

		}

		void PathSearch::shutdown() {

		}

		void PathSearch::unload() {

		}

		bool PathSearch::isDone() const {
			return false;
		}

		std::vector<Tile const*> const PathSearch::getSolution() const {
			return std::vector<Tile const*>();
		}

	


	}
}  // close namespace ufl_cap4053::searches
