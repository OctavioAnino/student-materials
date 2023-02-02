#include <vector>
#include <deque>
#include <iostream>
#include <stack>
//#include <cstddef>


// TreeNode class should go in the "ufl_cap4053::fundamentals" namespace!
namespace ufl_cap4053 { namespace fundamentals {
	template<class T>
	class TreeNode {
	private:
		T element_;
		
	public:
		std::vector<TreeNode<T>*> *children_;

		TreeNode<T>() : element_(T()) {
			children_ = new std::vector<TreeNode<T>*>();
		}
		TreeNode<T>(T element) : element_(element) {
			children_ = new std::vector<TreeNode<T>*>();
			//std::cout << "In constructor with parameter. element = " << element << std::endl;
			//std::cout << "								element_ = " << element_ << std::endl;
		}
		~TreeNode() {
			delete(children_);
		}

		const T& getData() const {
			return element_;
		}
		std::size_t getChildCount() const {
			return children_->size();
		}
		TreeNode<T>* getChild(size_t index) {
			return children_->at(index);
		}
		TreeNode<T>* getChild(size_t index) const {
			return children_->at(index);
		}
		void addChild(TreeNode<T>* child) {
			children_->push_back(child);
		}
		void removeChild(size_t index)
		{
			children_->erase((children_->begin() + index));
		}
		void breadthFirstTraverse(void (*datafunction)(const T)) const {
			std::deque<TreeNode<T>*> *queue = new std::deque<TreeNode<T>*>();

			datafunction(element_);

			//std::cout << "ELEMENT_: " << this->element_ << std::endl;

			//std::cout << "deque size: " << queue->size() << std::endl;
			//std::cout << "children_ size: " << children_->size() << std::endl;

			////queue->push_back(this);
			size_t ch_size = children_->size();
			//std::cout << "ch_size size: " << ch_size << std::endl;
			for (int pos = 0; pos < ch_size; pos++) {
				queue->push_back(children_->at(pos));
				//std::cout << "children_ size: " << children_->size() << std::endl;
			}

			////TreeNode<T>** root = &(this);
			////queue->push_back(root);
			//
			while(!queue->empty()) {
				TreeNode<T> *curr = queue->front();
				queue->pop_front();
				datafunction(curr->element_);
				for(int pos = 0; pos<(curr->children_)->size(); pos++) {
					queue->push_back((curr->children_)->at(pos));
				}
			}

			delete(queue);
		}
		void preOrderTraverse(void (*datafunction)(const T)) const {
			datafunction(element_);

			for(int pos = 0; pos<children_->size(); pos++) 
				(children_->at(pos))->preOrderTraverse(datafunction);
		}
		void postOrderTraverse(void (*datafunction)(const T)) const {
			////std::cout << "Current node: " << element_ << std::endl;
			////std::cout << "Current node's children: " << children_->size() << std::endl;
			//for (int pos = 0; pos < children_->size(); pos++) {
			//	//std::cout << "About to visit: " << (children_->at(pos))->getData() << std::endl;
			//	(children_->at(pos))->preOrderTraverse(datafunction);
			//	//std::cout << "Leaving: " << (children_->at(pos))->getData() << std::endl;
			//}
			//
			//datafunction(element_);
			
			std::stack<TreeNode<T>*>* n_stack = new std::stack<TreeNode<T>*>();
			std::stack<int>* cc_stack = new std::stack<int>();
			for (int root_child = 0; root_child < children_->size(); root_child++) {
				n_stack->push(children_->at(root_child));
				cc_stack->push(0);
				while (!n_stack->empty()) {
					TreeNode<T>* s_top = n_stack->top();
					std::vector<TreeNode<T>*>* top_childs = s_top->children_;
					if (cc_stack->top() < top_childs->size()) 
					{
						n_stack->push((n_stack->top())->children_->at(cc_stack->top()));
						cc_stack->push(0);
					}
					else
					{
						datafunction(n_stack->top()->getData());
						n_stack->pop();
						cc_stack->pop();
						if(!cc_stack->empty()) (cc_stack->top())++;
					}
				}
			}
			datafunction(element_);

			delete(n_stack);
			delete(cc_stack);

		}
	};
}}  // namespace ufl_cap4053::fundamentals
