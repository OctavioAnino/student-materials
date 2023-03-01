#include <vector>
#include <cstdlib>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <cmath>

//#include "../Framework/TileSystem/TileMap.h"
#include "PathSearch.h"
#include "../PriorityQueue.h"



namespace ufl_cap4053
{
	namespace searches
	{
		// CLASS DEFINITION GOES HERE
		struct PathSearch::Private {

			static int** getNeighbors(PathSearch& self, int* node) {
				int** neighbors = new int* [7] {nullptr};
				int* neighbor_count = new int[1];
				neighbor_count[0] = 0;
				neighbors[0] = neighbor_count;

				bool even_row = node[0] % 2 == 0;
				int col_start = even_row ? -1 : 1;

				for (int n_col = col_start;
					loopBoundsCheck(node[0], n_col);
					loopUpdateCol(node[0], &n_col))
				{
					for (int n_row = -1; n_row < 2; n_row++) {
						bool col_check = even_row ? n_col > 0: n_col < 0;
						if ((n_col == 0 && n_row == 0)
							|| (col_check && n_row != 0))
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
				if (node_row % 2 == 0 && col < 2 || node_row % 2 == 1 && col > -2)
					return true;
				return false;
			}

			static void addTileToNeighbors(Tile* neighbor_tile,
				int* neighbor_count,
				int** neighbors)
			{
				neighbor_count[0] = neighbor_count[0] + 1;
				int* valid_neighbor = new int[2];
				valid_neighbor[0] = neighbor_tile->getRow();
				valid_neighbor[1] = neighbor_tile->getColumn();
				neighbors[neighbor_count[0]] = valid_neighbor;
			}

		};

		struct PathSearch::Node {
			int* coordinates_;
			double estimatedDistanceToGoal_;
			double givenCost_;
			double pathCost_;
			Tile* tile_;

			Node(int row, int col) {
				coordinates_ = new int[2] {row, col};
				givenCost_ = 0;
				estimatedDistanceToGoal_ = -1;
				pathCost_ = -1;
				tile_ = nullptr;
			}

			Node(int row, int col, double givenCost, double estimatedDistanceToGoal, double pathCost, Tile* tile) {
				coordinates_ = new int[2] {row, col};
				givenCost_ = givenCost;
				estimatedDistanceToGoal_ = estimatedDistanceToGoal;
				tile_ = tile;
				pathCost_ = pathCost;
			}

			~Node() {
				delete[] coordinates_;
				coordinates_ = nullptr;
				tile_ = nullptr;
			}

			void generatePathCost(double (*estimatePathCost)(double const, double const)) {
				pathCost_ = estimatePathCost(givenCost_, estimatedDistanceToGoal_);
			}
		};

		struct PathSearch::SearchState {

			PriorityQueue<struct Node*>* toVisit_;
			std::unordered_map<int, struct Node*>* seen_;
			std::unordered_map<int, struct Node*>* visited_;


			PathSearch self_;

			SearchState(PathSearch& self) {
				self_ = self;
				seen_ = new std::unordered_map<int, struct Node*>();
				visited_ = new std::unordered_map<int, struct Node*>();

				toVisit_ = new PriorityQueue<struct Node*>(&greedyCompare); 

			}

			~SearchState() {
				//clean toVisit_ q
				while (toVisit_ != nullptr && !toVisit_->empty())
				{
					struct Node* to_delete = toVisit_->front();
					toVisit_->pop();
					if (to_delete != nullptr) {
						int tile_pos = self_.get1DPos(to_delete->coordinates_[0], to_delete->coordinates_[1]);
						seen_->erase(seen_->find(tile_pos));
						delete to_delete;
					}
					//std::cout << "stuck in loop!" << std::endl;
				}
				if (toVisit_ != nullptr) delete toVisit_;
				toVisit_ = nullptr;

				//clean maps
				std::unordered_map<int, struct Node*>::iterator it = seen_->begin();
				while (!seen_->empty() && it != seen_->end())
				{
					struct Node* to_delete = it->second;
					if (to_delete != nullptr) delete to_delete;
					it = seen_->erase(it);
					//std::cout << "stuck in loop!" << std::endl;
				}
				if (seen_ != nullptr) delete seen_;
				seen_ = nullptr;

				it = visited_->begin();
				while (!visited_->empty() && it != visited_->end())
				{
					struct Node* to_delete = it->second;
					if (to_delete != nullptr) delete to_delete;
					it = visited_->erase(it);
					//std::cout << "stuck in loop!" << std::endl;
				}
				if (visited_ != nullptr) delete visited_;
				visited_ = nullptr;

			}


			void updateToVisitQ() {
				PriorityQueue<struct Node*>* n_toVisit_ = new PriorityQueue<struct Node*>(&greedyCompare);
				while (!toVisit_->empty())
				{
					n_toVisit_->push(toVisit_->front());
					toVisit_->pop();
				}
				delete toVisit_;
				toVisit_ = n_toVisit_;
			}

			static bool greedyCompare(struct Node * const& lhs, struct Node * const& rhs) // TO-DO
			{
				return lhs->pathCost_ > rhs->pathCost_;
			}
		};

	

		PathSearch::PathSearch() {};

		PathSearch::~PathSearch() {

		};

		void PathSearch::load(TileMap* _tileMap) {
			mapCols_ = _tileMap->getColumnCount();
			mapRows_ = _tileMap->getRowCount();

			tiles_ = new Tile * [mapRows_ * mapCols_]; //deleted in unload
			tileRadius_ = _tileMap->getTileRadius();
			weightSumSquare_ = _tileMap->getWeightSumSquared();

			expectedTileWeight_ = std::sqrt(weightSumSquare_) / (mapCols_ * mapRows_);

			for (int r = 0; r < mapRows_; r++)
			{
				for (int c = 0; c < mapCols_; c++) {
					tiles_[r * mapCols_ + c] = _tileMap->getTile(r, c);
				}
			}
		}

		void PathSearch::initialize(int startRow, int startCol, int goalRow, int goalCol) {

			// set member fields
			startRow_ = startRow;
			startCol_ = startCol;

			goalRow_ = goalRow;
			goalCol_ = goalCol;

			connections_ = new int[mapCols_ * mapRows_] {-1};

			found_ = false;

			// initialize search state

			currentSearch_ = new SearchState(*this);
			//currentSearch_->setLastVisitedNode(startRow, startCol);

			struct Node* startNode = new Node(startRow, startCol);

			//int heuristic_cost = (int)std::floor(std::sqrt(std::pow(goalCol-startCol,2)+pow(goalRow-startRow,2)));
			startNode->estimatedDistanceToGoal_ = (hexDistance(startRow, goalRow, startCol, goalCol)) * tileRadius_ * expectedTileWeight_;
			startNode->pathCost_ = estimatePathCost(0,startNode->estimatedDistanceToGoal_); 
			startNode->tile_ = tiles_[get1DPos(startRow, startCol)];

			currentSearch_->toVisit_->push(startNode);
			(*currentSearch_->seen_)[get1DPos(startNode->coordinates_[0], startNode->coordinates_[1])] = startNode;

		}

		/*
		* **
		* Called to allow the path planner to execute for the specified timeslice (in milliseconds).
		Within this method the search should be performed until the time expires or the solution is
		found. If timeslice is zero (0), this method should only do a single iteration of the
		algorithm. Otherwise the update should only iterate for the indicated number of milliseconds.
		This method is always preceded by at least one call to initialize().
		  **

		basically just do bfs for now...

		*/

		void PathSearch::update(long timeslice) {
			std::chrono::time_point start = std::chrono::steady_clock::now();
			bool searchInterrupted = false;

			struct Node* currentNode;



			bool found = false;

			while (!(currentSearch_->toVisit_)->empty() && !found)
			{
				currentNode = (currentSearch_->toVisit_)->front();
				(currentSearch_->toVisit_)->pop();

				int* currentNodePos = currentNode->coordinates_;

				int** neighbors = Private::getNeighbors(*this, currentNodePos);

				for (int neighbor_pos = 1; neighbor_pos <= neighbors[0][0]; neighbor_pos++)
				{
					int* neighbor = neighbors[neighbor_pos];
					int neighborTilesPos = get1DPos(neighbor[0], neighbor[1]);

					bool neigh_visited = (currentSearch_->visited_)->find(neighborTilesPos)
						!= (currentSearch_->visited_)->end();

					bool neigh_seen = (currentSearch_->seen_)->find(neighborTilesPos)
						!= (currentSearch_->seen_)->end();

					Tile* n_neighbor_tile = tiles_[neighborTilesPos];

					if (!neigh_visited && !neigh_seen)
					{ // if neighbor hasnt been seen before

						// create a node for neighbor
						struct Node* n_neighbor = new Node(neighbor[0], neighbor[1]);

						// calculate given cost to node
						n_neighbor->givenCost_ = currentNode->givenCost_ + currentNode->tile_->getWeight() * tileRadius_;
						n_neighbor->estimatedDistanceToGoal_ = hexDistance(currentNode->coordinates_[0], goalRow_,
							currentNode->coordinates_[1], goalCol_) * tileRadius_ * expectedTileWeight_;
						n_neighbor->generatePathCost(&estimatePathCost);
						n_neighbor->tile_ = tiles_[get1DPos(neighbor[0], neighbor[1])];

						// push neighbor onto q
						(currentSearch_->toVisit_)->push(n_neighbor);

						// set neighbor tile as seen
						(*currentSearch_->seen_)[neighborTilesPos] = n_neighbor;
						tiles_[neighborTilesPos]->setFill(0xFF8080EE);

						// !! remove reference to neighbor from neighbors since it now belongs to a node
						neighbors[neighbor_pos] = nullptr;

						// mark the path to this tile
						connections_[neighborTilesPos] = get1DPos(currentNodePos[0], currentNodePos[1]);

						if (neighbor[0] == goalRow_ && neighbor[1] == goalCol_)
						{
							found = true;
							//connections_[neighborTilesPos] = currentNodeTilesPos;
							break;
						}
					}
					else { // if the neighbor has been seen or visited
						// retrieve node information
						struct Node* neigh_node = neigh_visited ? (currentSearch_->visited_)->at(neighborTilesPos)
							: (currentSearch_->seen_)->at(neighborTilesPos);

						// calculate new given cost
						int n_given_cost = currentNode->givenCost_ + currentNode->tile_->getWeight() * tileRadius_;

						// if the new given cost is smaller than what it used to be
						if (n_given_cost < neigh_node->givenCost_) {
							// update the estimated cost and estimate cost for this node
							neigh_node->givenCost_ = n_given_cost;
							neigh_node->generatePathCost(&estimatePathCost);

							if (neigh_visited) // if the neighbor had already been visited, we must remove it from visited
							{ // and place it in toVisit_ again. Also, add to seen.
								(currentSearch_->visited_)->erase((currentSearch_->visited_)->find(neighborTilesPos));
								(currentSearch_->toVisit_)->push(neigh_node);
								(*currentSearch_->seen_)[neighborTilesPos] = neigh_node;
							}
							else
							{  // remove node an re insert to be placed in right position
								currentSearch_->toVisit_->remove(neigh_node);
								currentSearch_->toVisit_->push(neigh_node);
							}

							// update the path to this neighbor
							connections_[neighborTilesPos] = get1DPos(currentNodePos[0], currentNodePos[1]);
						}
					}
				}


				//clean neighbors[][]
				for (int i = 0; i < 7; i++)
				{
					if (neighbors[i] != nullptr)
						delete[] neighbors[i];
				}
				if (neighbors != nullptr) delete[] neighbors;
				neighbors = nullptr;

				// remove node from seen and place into visited
				(currentSearch_->seen_)->erase(
					(currentSearch_->seen_)->find(
						currentNodePos[0] * mapCols_ + currentNodePos[1]));

				(*currentSearch_->visited_)[(currentNodePos[0] * mapCols_ + currentNodePos[1])] = currentNode;
				tiles_[currentNodePos[0] * mapCols_ + currentNodePos[1]]->setFill(0xFF1020FF);
				//currentSearch_->setLastVisitedNode(currentNodePos[0], currentNodePos[1]);

				//if (currentNodePos != nullptr) delete[] currentNodePos;
				//currentNodePos = nullptr;

				if (std::chrono::steady_clock::now() - start
				> std::chrono::milliseconds(timeslice))
				{
					//searchPaused_ = true;
					searchInterrupted = true;
					break;
				}
			}


			if (found) {
				found_ = true;
			}

		}

		double PathSearch::estimatePathCost(double const givenCost, double const estimatedDistanceToGoal) {
			return givenCost*0.55 + estimatedDistanceToGoal*0.45;
		}


		void PathSearch::shutdown() {
			if (connections_ != nullptr) delete[] connections_;
			connections_ = nullptr;
			if (currentSearch_ != nullptr)
				delete currentSearch_;
			currentSearch_ = nullptr;

		}

		void PathSearch::unload() {
			shutdown();
			if (tiles_ != nullptr) delete[] tiles_;
		}

		bool PathSearch::isDone() const {
			return found_;
		}

		std::vector<Tile const*> const PathSearch::getSolution() const {
			if (!found_) return std::vector<Tile const*>();
			std::vector<Tile const*> answer = std::vector<Tile const*>();
			int currentNode = goalRow_ * mapCols_ + goalCol_;
			int goalNode = startRow_ * mapCols_ + startCol_;
			while (currentNode != goalNode)
			{
				answer.push_back(tiles_[currentNode]);
				currentNode = connections_[currentNode];
				//std::cout << "In while loop!" << std::endl;
			}
			answer.push_back(tiles_[goalNode]);

			return answer;
		}

	}
}  // close namespace ufl_cap4053::searches
