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

			static bool greedyCompare(struct Node* const& lhs, struct Node* const& rhs) // TO-DO
			{
				return lhs->pathCost_ > rhs->pathCost_;
			}
		};

		PathSearch::PathSearch() {};

		PathSearch::~PathSearch() {
			//unload();
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
			startNode->pathCost_ = estimatePathCost(0, startNode->estimatedDistanceToGoal_);
			startNode->tile_ = tiles_[get1DPos(startRow, startCol)];

			currentSearch_->toVisit_->push(startNode);
			(*currentSearch_->seen_)[get1DPos(startNode->coordinates_[0], startNode->coordinates_[1])] = startNode;

		}


		void PathSearch::update(long timeslice) {
			std::chrono::time_point start = std::chrono::steady_clock::now();

			struct Node* currentNode;

			PriorityQueue<struct Node*>* toVisit = currentSearch_->toVisit_;
			std::unordered_map<int, struct Node*>* seen = currentSearch_->seen_;
			std::unordered_map<int, struct Node*>* visited = currentSearch_->visited_;

			bool found = false;

			while (!toVisit->empty() && !found)
			{
				currentNode = toVisit->front();
				toVisit->pop();

				int* currentNodePos = currentNode->coordinates_;
				int currentNodeScalarPos = currentNodePos[0] * mapCols_ + currentNodePos[1];

				bool even_row = currentNodePos[0] % 2 == 0;
				int col_start = even_row ? -1 : 1;

				for (int n_col = col_start;
					loopBoundsCheck(currentNodePos[0], n_col);
					loopUpdateCol(currentNodePos[0], &n_col))
				{
					for (int n_row = -1; n_row < 2; n_row++) {
						// check for "incomplete" column
						bool col_check = even_row ? n_col > 0: n_col < 0;
						// check for self as well...
						if ((n_col == 0 && n_row == 0)
							|| (col_check && n_row != 0))
							continue;

						int row_pos = currentNodePos[0] + n_row;
						int col_pos = currentNodePos[1] + n_col;
						if (mapBoundsCheck(row_pos, col_pos)) continue;

						int neighborTilesPos = (row_pos) * (mapCols_) + (col_pos);
						Tile* n_neighbor_tile = tiles_[neighborTilesPos];
						if (n_neighbor_tile->getWeight() != 0)
						{
							bool neigh_visited = visited->find(neighborTilesPos) != visited->end();

							bool neigh_seen = seen->find(neighborTilesPos) != seen->end();

							if (!neigh_visited && !neigh_seen)
							{ // if neighbor hasnt been seen before

								// create a node for neighbor
								struct Node* n_neighbor = new Node(row_pos, col_pos);

								// calculate given cost to node
								n_neighbor->givenCost_ = currentNode->givenCost_ + currentNode->tile_->getWeight() * tileRadius_;
								n_neighbor->estimatedDistanceToGoal_ = hexDistance(currentNodePos[0], goalRow_,
									currentNodePos[1], goalCol_) * tileRadius_ * expectedTileWeight_;
								n_neighbor->pathCost_ = estimatePathCost(n_neighbor->givenCost_, n_neighbor->estimatedDistanceToGoal_);
								n_neighbor->tile_ = n_neighbor_tile;

								// push neighbor onto q
								toVisit->push(n_neighbor);

								// set neighbor tile as seen
								(*seen)[neighborTilesPos] = n_neighbor;
								tiles_[neighborTilesPos]->setFill(0xFF8080EE);

								// mark the path to this tile
								connections_[neighborTilesPos] = get1DPos(currentNodePos[0], currentNodePos[1]);

								if (row_pos == goalRow_ && col_pos == goalCol_)
								{
									found = true;
									//connections_[neighborTilesPos] = currentNodeTilesPos;
									break;
								}
							}
							else { // if the neighbor has been seen or visited
								// retrieve node information
								struct Node* neigh_node = neigh_visited ? visited->at(neighborTilesPos) : seen->at(neighborTilesPos);

								// calculate new given cost
								int n_given_cost = currentNode->givenCost_ + currentNode->tile_->getWeight() * tileRadius_;

								// if the new given cost is smaller than what it used to be
								if (n_given_cost < neigh_node->givenCost_) {
									// update the estimated cost and estimate cost for this node
									neigh_node->givenCost_ = n_given_cost;
									neigh_node->pathCost_ = estimatePathCost(neigh_node->givenCost_, neigh_node->estimatedDistanceToGoal_);

									if (neigh_visited) // if the neighbor had already been visited, we must remove it from visited
									{ // and place it in toVisit_ again. Also, add to seen.
										visited->erase(visited->find(neighborTilesPos));
										(*seen)[neighborTilesPos] = neigh_node;
									}
									else
									{  // remove node an re insert to be placed in right position
										toVisit->remove(neigh_node);
									}
									toVisit->push(neigh_node);
									// update the path to this neighbor
									connections_[neighborTilesPos] = get1DPos(currentNodePos[0], currentNodePos[1]);
								}
							}
						}
					}
				}

				// remove node from seen and place into visited
				seen->erase(seen->find(currentNodeScalarPos));
				(*visited)[(currentNodeScalarPos)] = currentNode;
				// update visual
				tiles_[currentNodeScalarPos]->setFill(0xFF1020FF);

				if (std::chrono::steady_clock::now() - start
				> std::chrono::milliseconds(timeslice))
					break;
			}

			if (found) 
				found_ = true;

		}

		double PathSearch::estimatePathCost(double const givenCost, double const estimatedDistanceToGoal) {
			return givenCost * 0.55 + estimatedDistanceToGoal * 0.45;
		}


		void PathSearch::shutdown() {
			if (connections_ != nullptr) delete[] connections_;
			connections_ = nullptr;
			if (currentSearch_ != nullptr)
				delete currentSearch_;
			currentSearch_ = nullptr;
		}

		void PathSearch::unload() {
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
