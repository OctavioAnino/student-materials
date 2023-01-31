#include <vector>

// TreeNode class should go in the "ufl_cap4053::fundamentals" namespace!
namespace ufl_cap4053 { namespace fundamentals {
	template<class T>
	class TreeNode {
	private:
		T *element_;
		std::vector *children_;

	public:
		TreeNode<T>() {
			element_ = new T();
			children_ = new std::vector<TreeNode<T>*>();
		}
		TreeNode<T>(T element) {
			element_ = element;
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
	};
}}  // namespace ufl_cap4053::fundamentals
