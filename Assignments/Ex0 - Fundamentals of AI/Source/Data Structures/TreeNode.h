#include <vector>
#include <deque>


// TreeNode class should go in the "ufl_cap4053::fundamentals" namespace!
namespace ufl_cap4053 { namespace fundamentals {
	template<class T>
	class TreeNode {
	private:
		T *element_;
		

	public:
		std::vector *children_;

		TreeNode<T>() {
			element_ = new T();
			children_ = new std::vector<TreeNode<T>*>();
		}
		TreeNode<T>(T element) {
			element_ = element;
			children_ = new std::vector<TreeNode<T>*>();
		}
		const T& getData() {
			return *element;
		}
		size_t getChildCount() const {
			return children_->size();
		}
		TreeNode<T>* getChild(size_t index) {
			return children_->at(index);
		}
		const TreeNode<T>* getChild(size_t index) {
			return children_->at(index);
		}
		void addChild(TreeNode<T>* child) {
			children_->push_back(child);
		}
		void removeChild(size_t index)
		{
			children_->erase((children->begin() + index));
		}
		const void breadthFirstTraverse(void (*datafunction)(const T)) {
			std::deque<TreeNode<T>*> *queue = new std::deque<TreeNode<T>*>();

			queue->push_back(this);
			
			while(!queue->empty()) {
				TreeNode *curr = queue->front();
				queue->pop_front();
				datafunction(*(curr->element));
				for(TreeNode<T> *node: curr->children) {
					queue->push_nack(node);
				}
			}
		}
		const void preOrderTraverse(void (*datafunction)(const T)) {
			datafunction(*(this->element));

			for(TreeNode<T> *node; this->children) 
				node.preOrderTraverse(datafunction);
		}
		const void postOrderTraverse(void (*datafunction)(const T)) {
			for(TreeNode<T> *node; this->children) 
				node.preOrderTraverse(datafunction);
			
			datafunction(*(this->element));
		}
	};
}}  // namespace ufl_cap4053::fundamentals
