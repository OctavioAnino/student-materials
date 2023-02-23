#include <vector>
#include <cstdlib>
#include <chrono>
#include <queue>
#include <unordered_map>

//#include "../Framework/TileSystem/TileMap.h"
#include "PathSearch.h"



namespace ufl_cap4053
{
	namespace searches
	{
		// CLASS DEFINITION GOES HERE

		struct PathSearch::Private {

			static int** getNeighbors(PathSearch& self, int* node) {
				int** neighbors = new int* [7];
				int* neighbor_count = new int[1];
				neighbor_count = 0;
				neighbors[0] = neighbor_count;

				bool even_row = node[0] % 2 == 0;
				int col_start = even_row ? -1 : 1;
				
				for (int n_col = col_start; 
					loopBoundsCheck(node[0], col_start); 
					loopUpdateCol(node[0], &n_col))
				{
					for (int n_row = -1; n_row < 2; n_row++) {
						bool col_check = even_row ? n_col > 0: n_col < 0;
						if ((n_col == 0 && n_row == 0)
							|| (col_check && n_row!=0))
							continue;

						int row_pos = node[0] + n_row;
						int col_pos = node[1] + n_col;
						if (mapBoundsCheck(self, row_pos, col_pos)) continue;

						int neigh_pos = (row_pos) * (self.mapCols_) + (col_pos);
						Tile* neighbor_tile = self.tiles_[neigh_pos];
						if (neighbor_tile->getWeight() != 0)
							addTileToNeighbors(neighbor_tile, neighbor_count, neighbors);
					}
				}

				return neighbors;
			}
			
			static bool mapBoundsCheck(PathSearch& self, int row_pos, int col_pos) {
				return row_pos < 0 || row_pos >= self.mapRows_
					|| col_pos < 0 || col_pos >= self.mapCols_;
			}

			static void loopUpdateCol(int node_row, int* col) {
				if (node_row % 2 == 0)
					(*col)++;
				else
					(*col)--;
			}

			static bool loopBoundsCheck(int node_row, int col) {
				if (node_row % 2 == 0 && col < 2)
					return true;
				if (node_row % 2 == 1 && col > -2) 
					return true;
				return false;
			}

			static void addTileToNeighbors(Tile* neighbor_tile,
				int* neighbor_count,
				int** neighbors)
			{
				neighbor_count[0]++;
				int* valid_neighbor = new int[2];
				valid_neighbor[0] = neighbor_tile->getRow();
				valid_neighbor[1] = neighbor_tile->getColumn();
				neighbors[neighbor_count[0]] = valid_neighbor;
			}

		};

		PathSearch::PathSearch() {};
		PathSearch::~PathSearch() {};

		void PathSearch::load(TileMap* _tileMap) {
			mapCols_ = _tileMap->getColumnCount();
			mapRows_ = _tileMap->getRowCount();

			tiles_ = new Tile * [mapRows_ * mapCols_];

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

		/*
		* **
		* Called to allow the path planner to execute for the specified timeslice (in milliseconds).
		Within this method the search should be performed until the time expires or the solution is
		found. If timeslice is zero (0), this method should only do a single iteration of the
		algorithm. Otherwise the update should only iterate for the indicated number of milliseconds.
		This method is always preceded by at least one call to initialize().
		  **
		
		basically just do bfs for now

		*/

		void PathSearch::update(long timeslice) {
			std::chrono::time_point start = std::chrono::steady_clock::now();

			std::queue<int*>* to_visit = new std::queue<int*>();
			std::unordered_map<int, bool>* visited = new std::unordered_map<int, bool>();
			connections_ = new int[mapCols_ * mapRows_] {0};

			int startTile[2] = { startRow_, startCol_ };

			to_visit->push(startTile);

			while (!to_visit->empty())
			{
				int* currentNode = to_visit->front();
				to_visit->pop();
				int** neighbors = Private::getNeighbors(*this, currentNode);
				
				for (int neighbor_pos = 0; neighbor_pos < neighbors[0][0]; neighbor_pos++)
				{
					if()
				}

				(*visited)[(currentNode[0] * mapCols_ + currentNode[1])] = true;
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
