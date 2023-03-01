#include "../platform.h" // This file will make exporting DLL symbols simpler for students.
#include "../Framework/TileSystem/Tile.h"
#include "../Framework/TileSystem/TileMap.h"

namespace ufl_cap4053
{
	namespace searches
	{
		class PathSearch
		{
			// CLASS DECLARATION GOES HERE
			Tile** tiles_ = nullptr;
			int* connections_ = nullptr;
			
			int mapCols_;
			int mapRows_;

			int startRow_, startCol_;
			int goalRow_, goalCol_;

			double tileRadius_;
			unsigned int weightSumSquare_;
			double expectedTileWeight_;

			bool found_ = false;
			//bool searchPaused_ = false;

			struct Private;
			struct SearchState;
			struct Node;

			struct SearchState* currentSearch_ = nullptr;

			int get1DPos(int row, int col)
			{
				return row * mapCols_ + col;
			}

			static double lineDistance(int x1, int x2, int y1, int y2) {
				return std::sqrt(std::pow(x2 - x1, 2) + pow(y2 - y1, 2));
			}

			static int hexDistance(int x1, int x2, int y1, int y2) {
				return std::round(std::sqrt(std::pow((x2 - x1), 2) + pow(y2 - y1, 2)));
			}

			static double estimatePathCost(double givenCost, double estimatedDistanceToGoal);


		public:
			DLLEXPORT PathSearch(); // EX: DLLEXPORT required for public methods - see platform.h

			DLLEXPORT ~PathSearch();

			DLLEXPORT void load(TileMap* _tileMap);

			DLLEXPORT void initialize(int startRow, int startCol, int goalRow, int goalCol);

			DLLEXPORT void update(long timeslice);

			DLLEXPORT void shutdown();

			DLLEXPORT void unload();

			DLLEXPORT bool isDone() const;

			DLLEXPORT std::vector<Tile const*> const getSolution() const;

		};
	}
}  // close namespace ufl_cap4053::searches
